# pylint: disable=ungrouped-imports
from IPython.display import display
try:
    import ipywidgets as widgets
except ImportError:
    from IPython.html import widgets # pylint: disable=no-name-in-module

from .utils import _interact_h

def _slider_eliminate_state(automaton):
    ''' Create the list of automata while applying the eliminate_state algorithm.'''
    count = automaton.state_number()
    automata = {}
    automata[0] = automaton
    for i in range(count):
        automaton = automaton.eliminate_state()
        automata[i + 1] = automaton
    return automata, count

def _create_callback(widget):
    ''' Create a callback which update an interactive svg for an EliminateState
        widget object. The result is required as argument of
        IPython.html.widget.on_trait_change(callback, ...)'''
    return lambda name, value, new: _interact_h(lambda:
            display(widget.automata[new]))

class EliminateState(widgets.DOMWidget):
    ''' Create a widget composed of an IntSlider and a svg to showcase each
    steps of the algorithm automaton.eliminate_state'''
    def __init__(self, automaton):
        self.automata, count = _slider_eliminate_state(automaton)
        self.value = 0
        self._slide_bar = widgets.IntSlider(description='Algorithm step(s)'
                                        , min=0
                                        , max=count
                                        , step=1
                                        , value=0)

        self._slide_bar.on_trait_change(_create_callback(self), 'value')

    def show(self):
        display(self._slide_bar)
        _interact_h(lambda: display(self.automata[0]))
