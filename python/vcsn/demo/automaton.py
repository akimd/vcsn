# pylint: disable=protected-access
try:
    import ipywidgets as widgets
except ImportError:
    from IPython.html import widgets # pylint: disable=no-name-in-module
from IPython.display import display

from .utils import _interact_h

def _the_callback(widget):
    ''' This callback is returned by _create_callback. Rational: This is the
    only way to do multi-line lambda in python as much as I now it. '''
    if not widget._algorithm_option.value in widget._automata:
        # pylint: disable=line-too-long
        widget._automata[widget._algorithm_option.value] = widget._expression.automaton(widget._algorithm_option.value)

    display(widget._automata[widget._algorithm_option.value])

def _create_callback(widget):
    ''' The returned lambda is called each time an option is selected in the
    drop down menu.'''
    return lambda name, value, new: _interact_h(lambda: _the_callback(widget))

class Automaton(widgets.DOMWidget):
    ''' Create a widget composed of a menu list of the following
    algorithms that are applyed to a given expression'''

    def __init__(self, expression):
        self._expression = expression
        self._automata = {'derived_term': self._expression.automaton()}
        algos = ('derived_term', 'standard', 'thompson', 'zpc', 'zpc_compact')
        self._algorithm_option = widgets.Dropdown(options=algos)
        self._algorithm_option.on_trait_change(_create_callback(self), 'value')

    def show(self):
        display(self._algorithm_option)
        _interact_h(lambda:
                display(self._automata[self._algorithm_option.value]))
