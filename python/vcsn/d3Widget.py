from __future__ import print_function

from IPython.display import display, Javascript
from IPython.html import widgets
from IPython.utils import traitlets

import vcsn
from vcsn.dot import daut_to_transitions

class VcsnD3DataFrame(object):
    def __init__(self, ipython, name):
        self.ipython = ipython
        self.name = name
        if self.name in self.ipython.shell.user_ns:
            aut = self.ipython.shell.user_ns[self.name].strip()
            states, transitions = self._d3_of_aut(aut)
        # Here Add the conversion from vcsn to d3 datas
        else:
            states = [{'id': 0}]
            transitions = [{'source': '0', 'label': ''},
                           {'target': '0', 'label': ''}]
        aut = AutomatonD3Widget(states=states, transitions=transitions)
        self._widget = aut
        self._widget.on_trait_change(self._on_transitions_changed, 'transitions')

    def _aut_of_d3(self, e, trans):
        '''Conversion from d3 to an automaton, via "daut".'''
        aut = ''
        for t in trans:
            src = float(t['source']['id'])
            dst = float(t['target']['id'])
            aut += "{src} -> {dst} {label}\n".format(
                src = int(src) if src == int(src) else '$',
                dst = int(dst) if dst == int(dst) else '$',
                label = t['label']
            )
        return vcsn.automaton(aut, 'daut')

    def _d3_of_aut(self, aut):
        '''Convert an automaton into a list of states and a list
        of transitions.'''
        ts = daut_to_transitions(aut.format('daut'))
        transitions = []
        for t in ts:
            if t[0] == '$':
                transitions.append({'target': t[1], 'label': t[2]})
            elif t[1] == '$':
                transitions.append({'source': t[0], 'label': t[2]})
            else:
                transitions.append({'source': t[0], 'target': t[1], 'label': t[2]})
        states = [{'id': s}
                  for s in set().union(*[set([t[0], t[1]]) for t in ts])
                  if s != '$']
        return (states, transitions)

    def _on_transitions_changed(self, e, trans):
        # d3 ==> python (called every time the user changes a value on
        # the gui).  Here the conversion from d3 to vcsn.
        try:
            self.ipython.shell.user_ns[self.name] = self._aut_of_d3(e, trans)
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
