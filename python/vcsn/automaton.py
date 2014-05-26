## ----------- ##
## automaton.  ##
## ----------- ##

from IPython.display import display, SVG
import re
from vcsn_cxx import automaton, label, weight
from vcsn import _dot_to_svg, _info_to_dict, _left_mult, _right_mult

def _one_epsilon(s):
    "Convert s to use the genuine epsilon character."
    return re.sub(r'\\\\e', '&epsilon;', s)

automaton.__eq__ = lambda self, other: str(self) == str(other)
automaton.__add__ = automaton.sum
automaton.__and__ = lambda l, r: _conjunction(l, r)
automaton.__invert__ = automaton.complement
automaton.__mul__ = _right_mult
automaton.__mod__ = automaton.difference
automaton.__or__ = automaton.union
automaton.__pow__ = automaton.power
automaton.__repr__ = lambda self: self.info()['type']
automaton.__rmul__ = _left_mult
automaton.__str__ = lambda self: self.format('dot')
automaton.__sub__ = automaton.difference
automaton._repr_svg_ = lambda self: _dot_to_svg(self.dot())

class _conjunction(object):
    """A proxy class that delays calls to the & operator in order
    to turn a & b & c into a variadic evaluation of
    automaton._product(a, b, c)."""
    def __init__(self, *args):
        self.auts = [args[0]]
        for arg in args[1:]:
            self.__and__(arg)
    def __and__(self, arg):
        if isinstance(arg, int):
            self.auts[-1] = self.auts[-1].power(arg)
        else:
            self.auts += (arg,)
        return self
    def value(self):
        if isinstance(self.auts, list):
            self.auts = automaton._product(self.auts)
        return self.auts
    def __nonzero__(self):
        return bool(self.value())
    def __str__(self):
        return str(self.value())
    def __repr__(self):
        return repr(self.value())
    def __getattr__(self, name):
        return getattr(self.value(), name)
    def __hasattr__(self, name):
        return hasattr(self.value(), name)

def _automaton_display(self, mode):
    """Display automaton `self` with a local menu to propose different
    formats.
    """
    if mode == "dot" or mode == "tooltip":
        dot = _one_epsilon(self.format("dot"))
        if mode == "tooltip":
            dot = re.sub(r'label = (".*?"), shape = box, style = rounded',
                         r'tooltip = \1',
                         dot)
        svg = _dot_to_svg(dot)
        display(SVG(svg))
    elif mode == "info":
        display(self.info())
    elif mode == "type":
        display(repr(self))
    else:
        raise(ValueError("invalid display format: " + mode))

# Requires IPython 2.0.
try:
    from IPython.html.widgets import interact
    automaton.display = \
        lambda self: interact(lambda mode: _automaton_display(self, mode),
                              mode = ['dot', 'info', 'tooltip', 'type'])
except ImportError:
    pass

automaton.dot = lambda self: _one_epsilon(self.format('dot'))

def _automaton_eval(self, w):
    """Evaluation of word `w` on `self`, with possible conversion from
    plain string to genuine label object.
    """
    c = self.context()
    if not isinstance(w, label):
        w = c.word(str(w))
    return self._eval(w)
automaton.eval = _automaton_eval

def _automaton_load(file, format = "dot"):
    return automaton(open(file, "r").read(), format)
automaton.load = staticmethod(_automaton_load)

def _automaton_fst(aut, cmd):
    import subprocess
    p1 = subprocess.Popen(['efstcompile'],
                          stdin=subprocess.PIPE,
                          stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE)
    p2 = subprocess.Popen([cmd],
                          stdin=p1.stdout,
                          stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE)
    p1.stdout.close()  # Allow p1 to receive a SIGPIPE if p2 exits.
    p3 = subprocess.Popen(['efstdecompile'],
                          stdin=p2.stdout,
                          stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE)
    p2.stdout.close()  # Allow p2 to receive a SIGPIPE if p3 exits.
    p1.stdin.write(aut.format('efsm'))
    res = p3.communicate()[0]
    return automaton(res, "efsm")

automaton.fstdeterminize = lambda self: _automaton_fst(self, "fstdeterminize")
automaton.fstminimize = lambda self: _automaton_fst(self, "fstminimize")

automaton.info = lambda self: _info_to_dict(self.format('info'))

def _automaton_is_synchronized_by(self, w):
    c = self.context()
    if not isinstance(w, label):
        w = c.word(str(w))
    return self._is_synchronized_by(w)
automaton.is_synchronized_by = _automaton_is_synchronized_by

def _lan_to_lal(a):
    """Convert an automaton from supporting spontaneous transitions
    to not supporting them by modifying its context specification.
    """
    dot = a.format('dot')
    dot = re.sub(r'"lan<(lal_char\(.*?\))>', r'"\1', dot)
    dot = re.sub(r'"lat<lan<(lal_char\(.*?\))>, *lan<(lal_char\(.*?\))>',
                 r'"lat<\1, \2', dot)
    return automaton(dot, 'dot')
automaton.lan_to_lal = lambda self: _lan_to_lal(self)

# Somewhat cheating: in Python, proper returns a LAL, not a LAN.
# _proper is the genuine binding to dyn::proper.
automaton.proper = lambda self, prune = True: self._proper(prune).lan_to_lal()
