try:
    import ipywidgets as widgets
except ImportError:
    from IPython.html import widgets # pylint: disable=no-name-in-module
from IPython.display import display

# Same function as in ipython module. It needs to be define here to break
# circular dependencie between vcsn.ipython and vcsn.demo.eliminate_state
def _interact_h(_interact_f, *args, **kwargs):
    '''Similar to IPython's interact function, but with widgets
    packed horizontally.'''
    f = _interact_f
    w = widgets.interactive(f, *args, **kwargs)
    f.widget = w
    # Weirdly enough, be sure to display before changing the class.
    display(w)
    return f
