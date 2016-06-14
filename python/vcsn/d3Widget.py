import html

# pylint: disable=ungrouped-imports
from IPython.display import display#, Javascript (unused, mentionned in fixme)
try:
    import ipywidgets as widgets
except ImportError:
    from IPython.html import widgets # pylint: disable=no-name-in-module
try:
    import traitlets
except ImportError:
    from IPython.utils import traitlets # pylint: disable=no-name-in-module
from vcsn.dot import daut_to_transitions

import vcsn
import vcsn.ipython


class VcsnD3DataFrame(object):

    def __init__(self, ip, name):
        # Here we call ipython ip to avoid conflict with the ipython file
        self.ip = ip
        self.name = name
        if self.name in self.ip.shell.user_ns:
            aut = self.ip.shell.user_ns[self.name].strip()
            states, transitions, context = self._d3_of_aut(aut)
            self.context = vcsn.context(context)
        # Here Add the conversion from vcsn to d3 datas
        else:
            states = [{'id': 0}]
            transitions = [{'source': '0', 'label': ''},
                           {'target': '0', 'label': ''}]
            self.context = vcsn.context('letterset<char_letters(b)>, b')

        aut = AutomatonD3(states=states, transitions=transitions)#, context=self.context)
        self.error = widgets.HTML(value='')

        self._widget_ctx = vcsn.ipython.ContextText(self, self.context)
        self._widget_ctx.text.on_trait_change(lambda: self._on_change())

        self._widget = aut
        self._widget.on_trait_change(lambda: self._on_change())

    def _aut_of_d3(self):
        '''Conversion from d3 to an automaton, via "daut".'''
        self.error.value = ''
        ctx = 'context = {:s}\n'.format(self.context)
        trans = self._widget.transitions
        aut = ''
        for t in trans:
            src = float(t['source']['id'])
            dst = float(t['target']['id'])
            aut += "{src} -> {dst} {label}\n".format(
                src=int(src) if src == int(src) else '$',
                dst=int(dst) if dst == int(dst) else '$',
                label=t['label']
            )
        res = ctx + aut
        try:
            return vcsn.automaton(res, 'daut')
        except RuntimeError as e:
            self.error.value = html.escape(str(e))

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

    def _on_change(self):
        # d3 ==> python (called every time the user changes a value on
        # the gui).  Here the conversion from d3 to vcsn.
        self.context = self._widget_ctx.text.value.encode('utf-8')
        try:
            self.ip.shell.user_ns[self.name] = self._aut_of_d3()
        except TypeError:
            pass

    def show(self):
        wc1 = widgets.ContainerWidget()
        wc1.children = [self._widget_ctx.text]
        wc2 = widgets.ContainerWidget()
        self._widget_ctx.latex.set_css({
            'padding-left': '10px',
            'padding-top': '15px', })
        wc2.children = [wc1, self._widget_ctx.latex]
        wc3 = widgets.ContainerWidget()
        self.error.set_css({
            'padding-left': '20px',
            'padding-top': '15px',
            'color': 'maroon', })
        wc3.children = [wc2, self.error]
        display(wc3)
        wc2.remove_class('vbox')
        wc2.add_class('hbox')
        wc3.remove_class('vbox')
        wc3.add_class('hbox')
        display(self._widget)


class AutomatonD3Widget(widgets.DOMWidget):
    # Here the Javascript Code
    # Using D3, Jquery and Backbone
    # We load and display it from the js file we install in nb_extension
    # install_nbextension(os.path.abspath(vcsn.datadir + '/js/AutomatonD3Widget.js'), verbose=0)

    # FIXME: script = """IPython.load_extensions("AutomatonD3Widget")"""
    # FIXME: display(Javascript(script));

    _view_name = traitlets.Unicode('AutomatonView', sync=True)
    states = traitlets.List(sync=True)
    transitions = traitlets.List(sync=True)
    context = traitlets.Unicode(sync=True)
