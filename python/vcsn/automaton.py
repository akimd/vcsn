## ----------- ##
## automaton.  ##
## ----------- ##

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
automaton._repr_svg_ = lambda self: self.as_svg()

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
            if len(self.auts) == 1:
                self.auts = self.auts[0]
            else:
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

def _automaton_as_svg(self, format = "dot", engine = "dot"):
    if format == "dot":
        return _dot_to_svg(self.dot(), engine)
    elif format == "dot2tex":
        dot2tex = self.format("dot2tex")
        import tempfile
        tmp = tempfile.NamedTemporaryFile().name
        import subprocess
        p1 = subprocess.Popen(['dot2tex', '--prog', engine],
                              stdin=subprocess.PIPE,
                              stdout=open(tmp + ".tex", "w"),
                              stderr=subprocess.PIPE)
        p1.stdin.write(dot2tex.encode('utf-8'))
        res = p1.communicate()
        subprocess.check_call(["texi2pdf", "--batch", "--clean", "--quiet",
                               "--output", tmp + ".pdf", tmp + ".tex"])
        # sudo easy_install wand
        from wand.image import Image as WImage
        return WImage(filename=tmp + '.pdf', resolution = 600)
    else:
        raise(ValueError("invalid format: ", format))

automaton.as_svg = lambda self, fmt = "dot", engine = "dot": _automaton_as_svg(self, fmt, engine)

def _automaton_display(self, mode, engine = "dot"):
    """Display automaton `self` in `mode` with Graphviz `engine`."""
    from IPython.display import display, SVG
    if mode == "dot" or mode == "tooltip":
        dot = _one_epsilon(self.format("dot"))
        if mode == "tooltip":
            dot = re.sub(r'label = (".*?"), shape = box, style = rounded',
                         r'tooltip = \1',
                         dot)
        svg = _dot_to_svg(dot, engine)
        display(SVG(svg))
    elif mode == "dot2tex":
        display(self.as_svg(mode, engine))
    elif mode == "info":
        display(self.info(False))
    elif mode == "info,detailed":
        display(self.info(True))
    elif mode == "type":
        display(repr(self))
    else:
        raise(ValueError("invalid display format: " + mode))

# Requires IPython 2.0.
def _automaton_interact(self):
    """Display automaton `self` with a local menu to the select
    the display mode.  Pay attention to not displaying large
    automata by default.
    """
    from IPython.html.widgets import interact
    if 20 < self.state_number():
        modes = ['info', 'dot']
    else:
        modes = ['dot', 'info']
    modes += ['info,detailed', 'tooltip', 'type', 'dot2tex']
    engines = ['dot', 'neato', 'twopi', 'circo', 'fdp', 'sfdp', 'patchwork']
    interact(lambda mode, engine: _automaton_display(self, mode, engine),
             mode = modes, engine = engines)

automaton.display = _automaton_interact

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

automaton.info = lambda self, detailed = False: \
  _info_to_dict(self.format('info,detailed' if detailed else 'info'))

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

automaton.shuffle = lambda *auts: automaton._shuffle(list(auts))

automaton.state_number = lambda self: self.info(False)['number of states']
