# pylint: disable=protected-access
from html import escape

try:
    import ipywidgets as widgets
except ImportError:
    from IPython.html import widgets # pylint: disable=no-name-in-module
from IPython.core.magic import (Magics, magics_class,
                                line_magic, line_cell_magic)
from IPython.core.magic_arguments import (argument, magic_arguments, parse_argstring)
from IPython.display import display
import vcsn

from vcsn.dot import _dot_to_svg, _dot_pretty, daut_to_dot
from vcsn import d3Widget

import vcsn.demo as demo


ip = get_ipython()

def formatError(error):
    res = '<pre style="color: maroon; background: #FDD; padding: 5px;">'
    res += escape(str(error))
    res += '</pre>'
    return res

class TextLatexWidget:
    '''A class to eliminate redundancy over our widgets.
    Parameters:
        name: the name for the label.
        value: the default value of the text box.
        toLatex: optional, the function to render the text in LaTeX.
    '''
    def __init__(self, name, value, toLatex=None):
        self.label = widgets.Label(value=name, padding='5px 0 7px 0')
        self.text = widgets.Text(value=value)
        self.text.on_submit(self.update)
        self.latex = widgets.Label(width='50%', padding='5px 0 0 0')
        self.toLatex = toLatex
        self.update()

    def update(self, *_):
        if self.toLatex:
            try:
                self.latex.value = self.toLatex(self.getvalue())
            except RuntimeError:
                self.latex.value = ''

    def getvalue(self):
        return self.text.value.encode('utf-8')

    def tolist(self):
        return [self.label, self.text, self.latex]


class ContextText:

    def __init__(self, ipython, name=None):
        self.ipython = ipython
        self.name = name
        if self.name:
            # A named context (i.e., not used in the d3 widget).
            if not isinstance(self.name, str):
                ctx = name
                text = ctx.format('sname')
            elif not name.isidentifier():
                raise NameError(
                        '`{}` is not a valid variable name'.format(name))
            elif self.name in self.ipython.shell.user_ns:
                ctx = self.ipython.shell.user_ns[self.name]
                text = ctx.format('sname')
            else:
                text = 'lal, b'
                ctx = vcsn.context(text)
        else:
            # An unnamed context (i.e., used in the d3 widget).
            ctx = vcsn.context('lal_char, b')

        toLatex = lambda txt: vcsn.context(txt)._repr_latex_()
        self.widget = TextLatexWidget('Context:', text, toLatex)
        self.widget.text.on_submit(self.update)

        self.error = widgets.HTML(value='')

        # Display the widget if it is not use into the d3 widget.
        if isinstance(self.name, str):
            ctx = widgets.HBox(children=self.widget.tolist())
            box = widgets.VBox(children=[ctx, self.error])
            display(box)

    def update(self, *_):
        try:
            self.error.value = ''
            txt = self.widget.getvalue()
            c = vcsn.context(txt)
            if isinstance(self.name, str):
                self.ipython.shell.user_ns[self.name] = c
        except RuntimeError as e:
            self.error.value = formatError(e)


@magics_class
class EditContext(Magics):

    @magic_arguments()
    @argument('var', type=str, nargs='?', default=None,
              help='The name of the context to edit')
    @line_cell_magic
    def context(self, line, cell=None):
        args = parse_argstring(self.context, line)
        if cell is None:
            ContextText(self, args.var)

ip.register_magics(EditContext)


class AutomatonText:

    def __init__(self, ipython, name, format, mode):
        if not name.isidentifier():
            raise NameError(
                    '`{}` is not a valid variable name'.format(name))
        self.ipython = ipython
        self.name = name
        self.format = format
        if self.name in self.ipython.shell.user_ns:
            # Strip the automaton, as we cannot preserve the state names
            # anyway.
            aut = self.ipython.shell.user_ns[self.name].strip()
            text = aut.format(self.format)
        else:
            # Use a prebuilt context.
            c = 'lal, b'
            ctx = vcsn.context(c)
            aut = ctx.expression(r'\e').standard()
            self.ipython.shell.user_ns[self.name] = aut
            text = aut.format(self.format).replace(ctx.format('sname'), c)

        self.text = widgets.Textarea(value=text)
        self.height = self.text.value.count('\n')
        self.text.lines = '500'
        self.text.on_trait_change(self.update)
        self.error = widgets.HTML(value='')
        self.svg = widgets.HTML(value=aut.SVG())
        if mode == "h":
            wc1 = widgets.Box()
            wc1.children = [self.text]
            wc2 = widgets.HBox()
            wc2.children = [wc1, self.svg]
            wc3 = widgets.Box()
            wc3.children = [wc2, self.error]
            display(wc3)
        elif mode == "v":
            wc = widgets.HBox()
            wc.children = [self.svg, self.error, self.text]
            display(wc)

    def update(self):
        try:
            self.error.value = ''
            txt = self.text.value
            a = vcsn.automaton(txt, self.format, strip=False)
            self.ipython.shell.user_ns[self.name] = a
            dot = daut_to_dot(
                txt) if self.format == "daut" else a.format('dot')
            self.svg.value = _dot_to_svg(_dot_pretty(dot))
        except RuntimeError as e:
            self.error.value = formatError(e)


@magics_class
class EditAutomaton(Magics):

    @magic_arguments()
    @argument('-s', '--strip', action='store_true', default=False,
              help='''Whether to strip the result (i.e., discard user names
              and use the "real" state numbers).''')
    @argument('var', type=str, help='The name of the variable to edit.')
    @argument('format', type=str, nargs='?', default='auto',
              help='''The name of the format to edit the automaton in
              (auto, daut, dot, efsm, fado, grail).  Default: auto.''')
    @argument('mode', type=str, nargs='?', default='h',
              help='''The name of the visual mode to display the automaton
              (h for horizontal and v for vertical).  Default: h.''')
    @line_cell_magic
    def automaton(self, line, cell=None):
        args = parse_argstring(self.automaton, line)
        if not args.var.isidentifier():
            raise NameError(
                    '`{}` is not a valid variable name'.format(args.var))
        if cell is None:
            # Line magic.
            if args.format == 'auto':
                args.format = 'daut'
            if args.format == 'gui':
                a = d3Widget.VcsnD3DataFrame(self, args.var)
                a.show()
            else:
                AutomatonText(self, args.var, args.format, args.mode)
        else:
            # Cell magic.
            a = vcsn.automaton(cell, format=args.format, strip=args.strip)
            self.shell.user_ns[args.var] = a
            display(a)

ip.register_magics(EditAutomaton)


class ExpressionText:
    '''A widgets that allows us to edit an expression and its context, save it
    under chosen identities, and render an automaton of it using a chosen
    algorithm.'''
    # pylint: disable=too-many-instance-attributes,too-many-locals
    def __init__(self, ipython, name):
        if not name.isidentifier():
            raise NameError(
                    '`{}` is not a valid variable name'.format(name))
        self.ipython = ipython
        self.name = name
        if self.name in self.ipython.shell.user_ns:
            exp = self.ipython.shell.user_ns[self.name]
            if not isinstance(exp, vcsn.expression):
                raise NameError(
                        '`{}` is defined but is not an expression'.format(name))
            text = exp.format('utf8')
            identities = exp.identities()
            cont = exp.context().format('sname')
        else:
            text = None
            identities = 'linear'
            cont = 'lal, b'

        ctx = vcsn.context(cont)
        exp = ctx.expression((text if text else r'\e'), identities)
        aut = exp.automaton()
        algos = vcsn.expression.automaton.algos
        ids = vcsn.expression.identities_list


        ctxToLatex = lambda txt: vcsn.context(txt)._repr_latex_()
        self.ctx = TextLatexWidget('Context: ', cont, ctxToLatex)

        # The expression relies on the context and identities
        # so it can't generate its latex by itself.
        self.exp = TextLatexWidget('Expression: ', text)
        self.exp.latex.value = exp._repr_latex_()

        algo_lab = widgets.Label(value='Algorithm:', padding='5px 0 0 0')
        self.algo = widgets.Dropdown(options=algos, value='auto')
        self.ids = widgets.Dropdown(options=ids, description='Identity: ',
                                    value=identities)

        self.aut = widgets.HTML(value=aut.SVG())

        self.ctx.text.on_submit(self.update)
        self.exp.text.on_submit(self.update)
        self.algo.on_trait_change(self.update)
        self.ids.on_trait_change(self.update)


        # Labels
        labs = widgets.VBox(children=[self.ctx.label, self.exp.label, algo_lab])
        # Context editor and latex
        ctx_box = widgets.HBox(children=[self.ctx.text, self.ctx.latex])
        # Expression editor and latex
        exp_box = widgets.HBox(children=[self.exp.text, self.exp.latex])
        # Algorithm and identities dropdowns
        algs_box = widgets.HBox(children=[self.algo, self.ids])

        # All 3 above
        interactive = widgets.VBox(children=[ctx_box, exp_box, algs_box])
        # Append the labels to the interactive bits
        panel = widgets.HBox(children=[labs, interactive])
        # Append the automaton/error widget
        box = widgets.VBox(children=[panel, self.aut])

        display(box)

    def update(self, *_):
        try:
            cont = self.ctx.getvalue()
            text = self.exp.getvalue()
            algo = self.algo.value
            idt = self.ids.value
            if text == b'':
                text = r'\e'
            ctx = vcsn.context(cont)
            exp = ctx.expression(text, idt)
            aut = exp.automaton(algo=algo)

            self.exp.latex.value = exp._repr_latex_()
            self.aut.value = aut.SVG()
            self.ipython.shell.user_ns[self.name] = exp
        except RuntimeError as e:
            self.exp.latex.value = ''
            self.aut.value = formatError(e)


@magics_class
class EditExpression(Magics):

    @magic_arguments()
    @argument('var', type=str, help='The name of the variable to edit.')
    @line_cell_magic
    def expression(self, line, cell=None):
        args = parse_argstring(self.expression, line)
        if not args.var.isidentifier():
            raise NameError(
                    '`{}` is not a valid variable name'.format(args.var))
        if cell is None:
            ExpressionText(self, args.var)

ip.register_magics(EditExpression)

@magics_class
class DemoAutomaton(Magics):

    ''' Usage: %demo variable algorithm

        variable     either an expression or an automaton.
        algorithm    one that is valid for the variable: [eliminate_state,
        automaton]

        Type %demo --help or %demo? to display this message.

        Example:

        e = vcsn.B.expression('(ab?){2}')
        a = e.standard().lift()
        %demo a eliminate_state
        '''
    @line_magic
    def demo(self, line):
        args = line.split(' ')
        var = args[0]
        algo = args[1] if len(args) > 1 else ''

        # The help message.
        if var == '--help' or algo == '' or var == '':
            print(self.__doc__)
            return

        if var not in self.shell.user_ns:
            raise NameError('`{}` does not exist'.format(var))

        var = self.shell.user_ns[var]

        if isinstance(var, vcsn.automaton):
            if algo == 'eliminate_state':
                demo.EliminateState(var)
            else:
                raise NameError(
                    '`{}` is not a valid algorithm for automaton'.format(algo))
        elif isinstance(var, vcsn.expression):
            if algo == 'automaton':
                demo.Automaton(var)
            else:
                raise NameError(
                    '`{}` is not a valid algorithm for expression'.format(algo))
        else:
            raise TypeError(
                '`{}` is not an automaton nor an expression'.format(args[0]))

    # This makes demo.__doc__, DemoAutomaton.__doc__ and self.__doc__
    # all equals, so that `%demo --help` and `%demo?` both work.
    demo.__doc__ = __doc__

ip.register_magics(DemoAutomaton)


def interact_h(_interact_f, *args, **kwargs):
    '''Similar to IPython's interact function, but with widgets
    packed horizontally.'''
    f = _interact_f
    w = widgets.interactive(f, *args, **kwargs)
    f.widget = w
    # Weirdly enough, be sure to display before changing the class.
    display(w)
    return f


class table(list):

    ''' Overridden list class which takes a 2-dimensional list of
        the form [[1,2,3],[4,5,6]], and renders an HTML Table in
        IPython Notebook. '''

    def to_html(self, s):
        try:
            return s.SVG()
        except AttributeError:
            pass
        try:
            return s._repr_latex_()
        except AttributeError:
            pass
        try:
            return s._repr_html_()
        except AttributeError:
            pass
        return s

    def _repr_html_(self):
        html = ["<table>"]
        for row in self:
            html.append("<tr>")

            for col in row:
                html.append("<td>{0}</td>".format(self.to_html(col)))

            html.append("</tr>")
        html.append("</table>")
        return ''.join(html)
