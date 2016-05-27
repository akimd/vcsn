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

# The class MUST call this class decorator at creation time.


class ContextText:

    def __init__(self, ipython, name=None):
        self.ipython = ipython
        self.name = name
        if self.name:
            # Here the way to differenciate the use into the d3 widget.
            if not isinstance(self.name, str):
                ctx = name
            elif self.name in self.ipython.shell.user_ns:
                ctx = vcsn.context(
                    format(self.ipython.shell.user_ns[self.name]))
            else:
                ctx = vcsn.context('lal_char, b')
                self.ipython.shell.user_ns[self.name] = ctx
        else:
            ctx = vcsn.context('lal_char, b')
        text = format(ctx)
        self.text = widgets.Textarea(description='Context: ', value=text)
        self.width = '250px'
        self.height = '25px'
        self.text.lines = '500'
        self.text.on_trait_change(lambda: self.update())
        self.error = widgets.HTML(value=' ')
        self.svg = widgets.Latex(value=ctx._repr_latex_())
        # Display the widget if it is not use into the d3 widget
        if isinstance(self.name, str):

            wc1 = widgets.VBox()
            wc1.children = [self.text]

            wc2 = widgets.HBox()
            wc2.children = [wc1, self.svg]
            wc3 = widgets.VBox()
            wc3.children = [wc2, self.error]
            display(wc3)

    def update(self):
        try:
            self.error.value = ''
            txt = self.text.value.encode('utf-8')
            c = vcsn.context(txt)
            if isinstance(self.name, str):
                self.ipython.shell.user_ns[self.name] = c
            self.svg.value = c._repr_latex_()
        except RuntimeError as e:
            self.error.value = escape(str(e))


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

ip = get_ipython() # pylint: disable=undefined-variable
ip.register_magics(EditContext)


class AutomatonText:

    def __init__(self, ipython, name, format, mode):
        self.ipython = ipython
        self.name = name
        self.format = format
        if self.name in self.ipython.shell.user_ns:
            # Strip the automaton, as we cannot preserve the state names
            # anyway.
            aut = self.ipython.shell.user_ns[self.name].strip()
            text = aut.format(self.format)
        else:
            c = 'lan, b'
            ctx = vcsn.context(c)
            aut = ctx.expression(r'\e').standard()
            self.ipython.shell.user_ns[self.name] = aut
            text = aut.format(self.format).replace(ctx.format('sname'), c)

        self.text = widgets.Textarea(value=text)
        self.height = self.text.value.count('\n')
        self.text.lines = '500'
        self.text.on_trait_change(lambda: self.update())
        self.error = widgets.HTML(value='')
        self.svg = widgets.HTML(value=aut._repr_svg_())
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
            self.error.value = escape(str(e))


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


@magics_class
class DemoAutomaton(Magics):

    ''' Usage: %demo variable algorithm

        variable     either an expression or an automaton.
        algorithm    one that is valid for the variable: [eliminate_state,
        automaton]

        Type %demo help to display this message.

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
        if var == 'help':
            print(self.__doc__)
            return

        if var in self.shell.user_ns:
            if algo == 'eliminate_state':
                a = demo.EliminateState(self.shell.user_ns[var])
            elif algo == 'automaton':
                a = demo.Automaton(self.shell.user_ns[var])

        try:
            a.show()
        except Exception:
            print(self.__doc__)

ip.register_magics(EditAutomaton)
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
            return s._repr_svg_()
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
