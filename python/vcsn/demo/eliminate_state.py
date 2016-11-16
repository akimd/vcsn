# pylint: disable=protected-access
import ipywidgets as widgets
from IPython.display import display

def eliminate_states(automaton):
    ''' Create the list of automata while applying the eliminate_state algorithm.'''
    count = automaton.state_number()
    automata = {}
    automata[0] = automaton.SVG()
    for i in range(count):
        automaton = automaton.eliminate_state()
        automata[i + 1] = automaton.SVG()
    return automata, count

class EliminateState:
    ''' Create a widget composed of an IntSlider and a svg to showcase each
    steps of the algorithm automaton.eliminate_state'''
    def __init__(self, automaton):
        self.automata, count = eliminate_states(automaton)

        self.slider = widgets.IntSlider(value=0, min=0, max=count, step=1)
        self.slider.observe(self.update, 'value')
        slider_label = widgets.Label(value='Algorithm step(s):', padding='5px 0 0 0')
        self.aut = widgets.HTML(value=self.automata[0])

        box = widgets.HBox(children=[slider_label, self.slider])

        display(box)
        display(self.aut)

    def update(self, *_):
        self.aut.value = self.automata[self.slider.value]
