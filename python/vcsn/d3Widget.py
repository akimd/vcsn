import os

# pylint: disable=ungrouped-imports
from IPython.display import display, Javascript
try:
    import ipywidgets as widgets
except ImportError:
    from IPython.html import widgets # pylint: disable=no-name-in-module
try:
    from notebook import nbextensions
except ImportError:
    from IPython.html import nbextensions # pylint: disable=no-name-in-module
try:
    import traitlets
except ImportError:
    from IPython.utils import traitlets # pylint: disable=no-name-in-module

import vcsn
import vcsn.ipython
from vcsn.dot import daut_to_transitions


class VcsnD3DataFrame(object):

    def __init__(self, ip, name):
        # Here we call ipython ip to avoid conflict with the ipython file
        self.ip = ip
        self.name = name
        if self.name in self.ip.shell.user_ns:
            aut = self.ip.shell.user_ns[self.name].strip()
            states, transitions, _ = self._d3_of_aut(aut)
            self.context = aut.context()
        # Here Add the conversion from vcsn to d3 datas
        else:
            states = [{'id': 0}]
            transitions = [{'source': '0', 'label': ''},
                           {'target': '0', 'label': ''}]
            self.context = vcsn.context('lal(a-z), b')

        aut = AutomatonD3Widget(states=states, transitions=transitions)#, context=self.context)
        self.error = widgets.HTML(value='')

        self._widget_ctx = vcsn.ipython.ContextText(self, self.context)
        self._widget_ctx.text.observe(self._on_change, 'value')

        self._widget = aut
        self._widget.observe(self._on_change, ['states','transitions'])

    def _aut_of_d3(self):
        '''Conversion from d3 to an automaton, via "daut".'''
        self.error.value = ''
        ctx = 'context = {:s}\n'.format(self.context)
        trans = self._widget.transitions
        aut = ''
        for t in trans:
            src = t['source'] if 'source' in t.keys() else '$'
            dst = t['target'] if 'target' in t.keys() else '$'
            aut += "{} -> {} {}\n".format(src, dst, t['label'])
        res = ctx + aut
        try:
            return vcsn.automaton(res, 'daut')
        except RuntimeError as e:
            self.error.value = vcsn.ipython.formatError(e)

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
        context = aut.context().format('sname')
        return (states, transitions, context)

    def _on_change(self, *_):
        # d3 ==> python (called every time the user changes a value on
        # the gui).  Here the conversion from d3 to vcsn.
        self.context = self._widget_ctx.text.value
        self.ip.shell.user_ns[self.name] = self._aut_of_d3()

    def show(self):
        ctx = widgets.HBox(children=self._widget_ctx.widget.tolist())
        box = widgets.VBox(children=[ctx, self.error, self._widget])
        display(box)


class AutomatonD3Widget(widgets.DOMWidget):
    _view_name = traitlets.Unicode('AutomatonView').tag(sync=True)
    _view_module = traitlets.Unicode('AutomatonD3Widget').tag(sync=True)
    states = traitlets.List(sync=True)
    transitions = traitlets.List(sync=True)
    context = traitlets.Unicode(sync=True)

# Load the Javascript file as a jupyter notebook extension.
# Overwrite to be sure to have the newest Javascript.
nbextensions.install_nbextension(
    os.path.abspath(vcsn.datadir + '/js/AutomatonD3Widget.js'),
    overwrite=True, user=True)
script = """IPython.notebook.config.update({
                "load_extensions": {"AutomatonD3Widget":true}
            })"""
display(Javascript(script))
