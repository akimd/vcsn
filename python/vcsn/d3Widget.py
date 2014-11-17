from __future__ import print_function

import os, io
import re
import vcsn

from IPython.core.magic import (Magics, magics_class, line_cell_magic, line_magic)
from IPython.core.magic_arguments import (argument, magic_arguments, parse_argstring)
from IPython.display import display, HTML, Latex, SVG
from IPython.html import widgets
from IPython.html.widgets import interactive
from IPython.utils.warn import info, error
from IPython.utils import traitlets
from IPython.display import Javascript

class VcsnD3DataFrame(object):
    def __init__(self, ipython, name):
        self.ipython = ipython
        self.name = name
        if self.name in self.ipython.shell.user_ns:
            aut = self.ipython.shell.user_ns[self.name].strip()
        # Here Add the conversion from vcsn to d3 datas
        else:
            states = [{'id': 0}]
            transitions = [{'source': '0', 'label': ''},
                           {'target': '0', 'label': ''}]
        aut = AutomatonD3Widget(states=states, transitions=transitions)
        self._widget = aut
        self._widget.on_trait_change(self._on_transitions_changed, 'transitions')

    def _on_transitions_changed(self, e, trans):
        # d3 ==> python ( called every time the user
        # changes a value on the gui
        # Here the conversion from d3 to vcsn
        try:
            res = ''
            for t in trans:
                src = float(t['source']['id'])
                dst = float(t['target']['id'])
                res += "{src} -> {dst} {label}\n".format(
                    src = int(src) if src == int(src) else '$',
                    dst = int(dst) if dst == int(dst) else '$',
                    label = t['label']
                )
            self.ipython.shell.user_ns[self.name] = vcsn.automaton(res, 'daut')
        except TypeError:
            pass

    def show(self):
        display(self._widget)

class AutomatonD3Widget(widgets.DOMWidget):

        # Here the Javascript Code
        # Using D3, Jquery and Backbone
        # We load and display it from the js file
        f = open(vcsn.datadir + '/js/AutomatonD3Widget.js', 'r')
        js = f.read()
        display(Javascript(js))

        _view_name = traitlets.Unicode('AutomatonView', sync=True)
        states = traitlets.List(sync=True)
        transitions = traitlets.List(sync=True)
