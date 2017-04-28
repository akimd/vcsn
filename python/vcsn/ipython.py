# pylint: disable=protected-access, too-many-instance-attributes
from html import escape
from threading import Thread, Event
import re

import ipywidgets as widgets
from IPython.core.magic import (Magics, magics_class,
                                line_magic, line_cell_magic)
from IPython.core.magic_arguments import (argument, magic_arguments, parse_argstring)
from IPython.display import display
import vcsn

from vcsn import d3Widget

import vcsn.demo as demo


ip = get_ipython()

def formatError(error):
    res = '<pre style="color: maroon; background: #FDD; padding: 5px;">'
    res += escape(str(error))
    res += '</pre>'
    return res

class AsyncUpdater:
    '''Asynchronous calls to `update` manager.

    Parameters:
        - widget: the widget to update,
        - wait: the time to wait before updating.
    '''
    def __init__(self, widget, wait):
        self.widget = widget
        self.wait = wait
        self.thread = None
        # Used to restart timer in the thread.
        self.event = Event()
        self.running = False

    def launch(self):
        '''Restarts the countdown.
        If no thread has been started, starts one.
        If a thread has been started and is running, restarts its countdown.
        If a thread has been started and has finished, starts a new one.'''
        if self.thread is not None \
           and self.thread.is_alive():
            # Unblock the thread a restart its countdown.
            self.event.set()
        else:
            self.thread = Thread(target=self.run)
            self.thread.start()

    def abort(self):
        '''Aborts the thread if it is running.'''
        if self.thread is not None \
           and self.thread.is_alive():
            self.running = False
            self.event.set()

    def run(self):
        '''Thread function.
        Loops as long as other threads call
        `self.event.set()` before the timeout'''
        self.running = True
        while self.running:
            self.event.clear()
            # If the unblocking is due to the timeout and not `event.set()`
            # i.e. we haven't been woken up to restart.
            if not self.event.wait(self.wait):
                break
        # If we are not being aborted.
        if self.running:
            self.widget.update()

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
        self.text.observe(self.asyncUpdate, 'value')
        self.latex = widgets.Label(width='50%', padding='5px 0 0 0')
        self.toLatex = toLatex
        self.updater = AsyncUpdater(self, 0.5)
        self.update()

    def asyncUpdate(self, *_):
        self.updater.launch()

    def update(self, *_):
        self.updater.abort()
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
                if not isinstance(self.ipython.shell.user_ns[self.name],
                                  vcsn.context):
                    raise TypeError(
                            '`{}` exists but is not a context'.format(name))
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
        self.widget.text.observe(self.asyncUpdate, 'value')

        # Binding for D3widget
        self.text = self.widget.text

        self.error = widgets.HTML(value='')

        # Display the widget if it is not use into the d3 widget.
        if isinstance(self.name, str):
            ctx = widgets.HBox(children=self.widget.tolist())
            box = widgets.VBox(children=[ctx, self.error])
            display(box)

        self.updater = AsyncUpdater(self, 0.5)

    def asyncUpdate(self, *_):
        self.updater.launch()

    def update(self, *_):
        self.updater.abort()
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

    def __init__(self, ipython, name, format, horizontal=True):
        '''A wrapper of widgets to edit an automaton interactively.

        Parameters:
          - ipython: a reference to the caller shell
          - name: the name of the variable to edit
          - format: the format in which we want to edit it
          - horizontal (optional): layout the widgets horizontally, default: True
        '''
        if not name.isidentifier():
            raise NameError(
                    '`{}` is not a valid variable name'.format(name))
        self.ipython = ipython
        self.name = name
        self.format = format
        if self.name in self.ipython.shell.user_ns:
            if not isinstance(self.ipython.shell.user_ns[self.name],
                              vcsn.automaton):
                raise TypeError(
                        '`{}` exists but is not an automaton'.format(name))
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
        self.mode = widgets.Dropdown(options=vcsn.automaton.display.modes,
                                     description='Mode:',
                                     value='pretty')
        self.engine = widgets.Dropdown(options=vcsn.automaton.display.engines,
                                       description='Engine:')

        self.out = widgets.Output()
        self.err = widgets.HTML()

        self.text.observe(self.asyncUpdate, 'value')
        self.mode.observe(self.update, 'value')
        self.engine.observe(self.update, 'value')

        dropdowns = widgets.HBox(children=[self.mode, self.engine])
        displayer = widgets.VBox(children=[dropdowns, self.out])
        if horizontal:
            interface = widgets.HBox(children=[self.text, displayer])
        else:
            interface = widgets.VBox(children=[self.text, displayer])
            self.text.width = '100%'
        box = widgets.VBox(children=[interface, self.err])
        display(box)
        self.updater = AsyncUpdater(self, 1)
        self.update()

    def asyncUpdate(self, *_):
        self.updater.launch()

    def update(self, *_):
        self.updater.abort()
        self.err.value = ''
        self.out.clear_output()
        try:
            txt = self.text.value
            a = vcsn.automaton(txt, self.format, strip=False)
            self.ipython.shell.user_ns[self.name] = a
            # There is currently no official documentation on this,
            # so please check out `ipywidgets.Output`'s docstring.
            with self.out:
                a._display(self.mode.value, self.engine.value)
        except RuntimeError as e:
            self.err.value = formatError(e)


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
                AutomatonText(self, args.var, args.format, args.mode != 'v')
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
    # pylint: disable=too-many-locals
    def __init__(self, ipython, name):
        if not name.isidentifier():
            raise NameError(
                    '`{}` is not a valid variable name'.format(name))
        self.ipython = ipython
        self.name = name
        if self.name in self.ipython.shell.user_ns:
            if not isinstance(self.ipython.shell.user_ns[self.name],
                              vcsn.expression):
                raise TypeError(
                        '`{}` exists but is not an expression'.format(name))
            exp = self.ipython.shell.user_ns[self.name]
            text = exp.format('utf8')
            identities = exp.identities()
            cont = exp.context().format('sname')
        else:
            text = r'\e'
            identities = 'linear'
            cont = 'lal, b'

        ctx = vcsn.context(cont)
        exp = ctx.expression(text, identities)
        algos = vcsn.expression.automaton.algos
        ids = vcsn.expression.identities_list


        ctxToLatex = lambda txt: vcsn.context(txt)._repr_latex_()
        self.ctx = TextLatexWidget('Context: ', cont, ctxToLatex)

        # The expression relies on the context and identities
        # so it can't generate its latex by itself.
        self.exp = TextLatexWidget('Expression: ', text)
        self.exp.latex.value = exp._repr_latex_()

        self.algo = widgets.Dropdown(options=algos,
                                     description='Algo:',
                                     value='auto')
        self.ids = widgets.Dropdown(options=ids,
                                    description='Identities:',
                                    value=identities)
        self.mode = widgets.Dropdown(options=vcsn.automaton.display.modes,
                                     description='Mode:',
                                     value='pretty')
        self.engine = widgets.Dropdown(options=vcsn.automaton.display.engines,
                                       description='Engine:')

        self.out = widgets.Output()
        self.err = widgets.HTML()

        self.ctx.text.on_submit(self.update)
        self.exp.text.on_submit(self.update)
        self.ctx.text.observe(self.asyncUpdate, 'value')
        self.exp.text.observe(self.asyncUpdate, 'value')
        self.algo.observe(self.update, 'value')
        self.ids.observe(self.update, 'value')
        self.mode.observe(self.update, 'value')
        self.engine.observe(self.update, 'value')


        # Labels.
        labs = widgets.VBox(children=[self.ctx.label, self.exp.label])
        # Context editor and latex.
        ctx_box = widgets.HBox(children=[self.ctx.text, self.ctx.latex])
        # Expression editor and latex.
        exp_box = widgets.HBox(children=[self.exp.text, self.exp.latex])
        # Append all 3 above.
        editors = widgets.VBox(children=[ctx_box, exp_box])
        editors_box = widgets.HBox(children=[labs, editors])
        # Algorithm and identities dropdowns.
        algs_box = widgets.HBox(children=[self.algo, self.ids])
        # Mode and engine dropdowns.
        render_box = widgets.HBox(children=[self.mode, self.engine])

        # All the interactive bits.
        interactive = widgets.VBox(children=[editors_box, algs_box, render_box])
        # Append the output and error widgets.
        box = widgets.VBox(children=[interactive, self.out, self.err])

        display(box)

        self.updater = AsyncUpdater(self, 0.5)
        self.update()

    def update(self, *_):
        # Abort asynchronous updates.
        self.updater.abort()
        self.err.value = ''
        self.out.clear_output()
        try:
            cont = self.ctx.getvalue()
            text = self.exp.getvalue()
            algo = self.algo.value
            idt = self.ids.value

            ctx = vcsn.context(cont)
            exp = ctx.expression(text, idt)
            aut = exp.automaton(algo=algo)

            self.exp.latex.value = exp._repr_latex_()
            # There is currently no official documentation on this,
            # so please check out `ipywidgets.Output`'s docstring.
            with self.out:
                aut._display(self.mode.value, self.engine.value)
            self.ipython.shell.user_ns[self.name] = exp
        except RuntimeError as e:
            self.exp.latex.value = ''
            self.err.value = formatError(e)

    def asyncUpdate(self, *_):
        self.updater.launch()


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
            return s._repr_html_()
        except AttributeError:
            pass
        # Try LaTeX first: when printing a table with expressions, we
        # don't want to display some SVG, but rather the LaTeX
        # version.
        try:
            return s._repr_latex_()
        except AttributeError:
            pass
        try:
            return s.SVG()
        except AttributeError:
            pass
        return str(s)

    def to_latex_expression(self, e):
        '''Turn an expression in a LaTex representation that uses
        our conventions.'''
        # FIXME: use vcsn.configs instead.
        res = e if isinstance(e, str) else e.format('latex')
        res = re.sub(r' *\\, *', '', res)
        res = re.sub(r'\\left\\langle *(.*?) *\\right\\rangle', r'\\bra{\1}', res)
        res = re.sub(r'\\left\( *(.*?) *\\right\)', r'\\paren{\1}', res)
        res = res \
            .replace(r'\left. ', '') \
            .replace(r'\right. ', '') \
            .replace(r'\middle|', r'\tuple') \
            .replace(r'{*}', '*') \
            .replace('varepsilon', 'eword') \
            .replace('{*}', '*')
        return res

    def to_latex(self, s):
        if isinstance(s, vcsn.expression):
            return self.to_latex_expression(s)
        try:
            return s._repr_latex_()
        except AttributeError:
            pass
        return str(s)

    def _repr_html_(self):
        res = ["<table>"]
        for row in self:
            res.append("<tr>")

            for cell in row:
                res.append("<td>{0}</td>".format(self.to_html(cell)))

            res.append("</tr>")
        res.append("</table>")
        return '\n'.join(res)

    def _repr_latex_(self):
        ncol = len(self[0])
        res = [r'\begin{array}{' + 'c' * ncol + '}']
        for row in self:
            res.append('  '
                       + ' & '.join([self.to_latex(c) for c in row])
                       + r' \\')
        res.append(r'\end{array}')
        return '\n'.join(res)
