from __future__ import print_function

import re

from IPython.core.magic import (Magics, magics_class, line_magic)
from IPython.display import display, SVG
from IPython.html import widgets

import vcsn
from vcsn.dot import _dot_to_svg, _dot_pretty, to_dot, from_dot
from IPython.utils.warn import info, error

# The class MUST call this class decorator at creation time
class EditAutomatonWidget:
    def __init__(self, ipython, name):
        self.ipython = ipython
        self.name = name
        if self.name in self.ipython.shell.user_ns:
            # Strip the automaton, as we cannot preserve the state names
            # anyway.
            aut = self.ipython.shell.user_ns[self.name].strip()
        else:
            aut = vcsn.context('lal_char(abc)_b').ratexp(r'\e').standard()
            self.ipython.shell.user_ns[self.name] = aut
        daut = from_dot(str(aut))

        self.text = widgets.TextareaWidget(value = daut)
        height = self.text.value.count('\n')
        self.text.set_css({'lines': '500'})
        self.text.on_trait_change(lambda: self.update())

        self.error = widgets.HTMLWidget(value = '')
        self.svg = widgets.HTMLWidget(value = aut._repr_svg_())

        cw = widgets.ContainerWidget()
        cw.remove_class('vbox')
        cw.add_class('hbox')
        cw.children = [self.svg, self.error, self.text]
        display(cw)

    def update(self):
       try:
            self.error.value = ''
            dot = self.text.value.encode('utf-8')
            dot = to_dot(dot)
            self.ipython.shell.user_ns[self.name] = vcsn.automaton(dot)
            self.svg.value = _dot_to_svg(_dot_pretty(dot))
       except RuntimeError, e:
            self.error.value = str(e)

@magics_class
class EditAutomaton(Magics):

    @line_magic
    def automaton(ipython, line):
        name = line.split()[0]
        EditAutomatonWidget(ipython, name)

ip = get_ipython()
ip.register_magics(EditAutomaton)
