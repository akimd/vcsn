# pylint: disable=protected-access
try:
    import ipywidgets as widgets
except ImportError:
    from IPython.html import widgets # pylint: disable=no-name-in-module
from IPython.display import display

class Automaton:
    ''' Create a widget composed of a menu list of the following
    algorithms that are applyed to a given expression'''

    def __init__(self, expression):
        self.exp = expression
        algos = expression.automaton.algos

        self.option = widgets.Dropdown(options=algos)
        self.option.observe(self.update, 'value')

        self.aut = widgets.HTML(value=self.exp.automaton().SVG())

        display(self.option)
        display(self.aut)

    def update(self, *_):
        self.aut.value = self.exp.automaton(algo=self.option.value).SVG()
