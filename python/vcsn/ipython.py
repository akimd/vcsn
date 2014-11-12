from __future__ import print_function

import re

from IPython.core.magic import (Magics, magics_class, line_cell_magic)
from IPython.core.magic_arguments import (argument, magic_arguments, parse_argstring)
from IPython.display import display, HTML, Latex, SVG
from IPython.html import widgets
from IPython.html.widgets import interactive
from IPython.utils.warn import info, error

import vcsn

from vcsn.dot import _dot_to_svg, _dot_pretty, to_dot, from_dot

# The class MUST call this class decorator at creation time
class AutomatonTextWidget:
    def __init__(self, ipython, name, format, mode):
        self.ipython = ipython
        self.name = name
        self.format = format
        if self.name in self.ipython.shell.user_ns:
            # Strip the automaton, as we cannot preserve the state names
            # anyway.
            aut = self.ipython.shell.user_ns[self.name].strip()
        else:
            aut = vcsn.context('lal_char(abc), b').ratexp(r'\e').standard()
            self.ipython.shell.user_ns[self.name] = aut

        text = aut.format(self.format)
        self.text = widgets.TextareaWidget(value = text)
        height = self.text.value.count('\n')
        self.text.set_css({'lines': '500'})
        self.text.on_trait_change(lambda: self.update())
        self.error = widgets.HTMLWidget(value = '')
        self.svg = widgets.HTMLWidget(value = aut._repr_svg_())
        if mode == "h":
            wc1 = widgets.ContainerWidget()
            wc1.children = [self.text]

            wc2 = widgets.ContainerWidget()
            wc2.children = [wc1, self.svg]
            wc3 = widgets.ContainerWidget()
            wc3.children = [wc2, self.error]
            display(wc3)
            wc2.remove_class('vbox')
            wc2.add_class('hbox')
        elif mode == "v":
            wc = widgets.ContainerWidget()
            wc.remove_class('vbox')
            wc.add_class('hbox')
            wc.children = [self.svg, self.error, self.text]
            display(wc)

    def update(self):
        try:
            self.error.value = ''
            txt = self.text.value.encode('utf-8')
            a = vcsn.automaton(txt, self.format)
            self.ipython.shell.user_ns[self.name] = a
            dot = to_dot(txt) if self.format == "daut" else a.format('dot')
            self.svg.value = _dot_to_svg(_dot_pretty(dot))
        except RuntimeError as e:
            self.error.value = str(e)

@magics_class
class EditAutomaton(Magics):
    @magic_arguments()
    @argument('var', type=str, help='The name of the variable to edit.')
    @argument('format', type=str, nargs ='?', default = 'daut',
              help='''The name of the format to edit the automaton in
              (dot, daut, efsm...).  Default: daut.''')
    @argument('mode', type=str, nargs ='?', default = 'h',
              help='''The name of the visual mode to display the automaton
              (h for horizontal and v for vertical).  Default: h.''')
    @line_cell_magic
    def automaton(self, line, cell=None):
        import d3Widget
        args = parse_argstring(self.automaton, line)
        if cell is None:
            if(args.format == 'gui'):
                d3Widget.VcsnD3DataFrame(self, args.var).show()
            else:
                AutomatonTextWidget(self, args.var, args.format, args.mode)
        else:
            a =  vcsn.automaton(cell.encode('utf-8'), args.format)
            self.shell.user_ns[args.var] = a
            display(a)

ip = get_ipython()
ip.register_magics(EditAutomaton)

def interact_h(_interact_f, *args, **kwargs):
    '''Similar to IPython's interact function, but with widgets
    packed horizontally.'''
    f = _interact_f
    w = interactive(f, *args, **kwargs)
    f.widget = w
    # Weirdly enough, be sure to display before changing the class.
    display(w)
    w.remove_class('vbox')
    w.add_class('hbox')
    return f


class table(list):
    ''' Overridden list class which takes a 2-dimensional list of
        the form [[1,2,3],[4,5,6]], and renders an HTML Table in
        IPython Notebook. '''

    def to_html(self, s):
        try:
            return s._repr_latex_()
        except AttributeError as e:
            pass
        try:
            return s._repr_html_()
        except AttributeError as e:
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
