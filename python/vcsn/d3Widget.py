# pylint: disable=ungrouped-imports
from IPython.display import display, display_javascript
import ipywidgets as widgets
import traitlets

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
        def get_state(obj):
            '''Convert whatever the given state is into a daut state.'''
            # Things that are not constant:
            #  - PRE and POST are decimals or '$'
            #  - states are strings or dicts
            if isinstance(obj, dict):
                state = str(obj['id'])
            elif isinstance(obj, str):
                state = obj
            try:
                # State ID is a valid integer number
                int(state)
                return state
            except ValueError:
                return '$'

        for t in trans:
            src = get_state(t['source'])
            dst = get_state(t['target'])
            aut += "{} -> {} {}\n".format(src, dst, t['label'])
        res = ctx + aut
        try:
            self.error.value = str(trans)
            return vcsn.automaton(res, 'daut')
        except RuntimeError as e:
            ts = ['{} -> {}'.format(t['source']['id'], t['target']['id'])
                  for t in trans
                  if isinstance(t['source'], dict) and isinstance(t['target'], dict)]
            self.error.value = vcsn.ipython.formatError(str(e)
                                                        + '\n#####\n'
                                                        + res
                                                        + '\n#####\n'
                                                        + '\n'.join(ts))

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

# Load the Javascript front-end
with open(vcsn.datadir + '/js/AutomatonD3Widget.js') as f:
    display_javascript(f.read())
