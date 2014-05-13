## ----------- ##
## automaton.  ##
## ----------- ##

import re
from vcsn_cxx import automaton, label, weight
from vcsn import is_equal, info_to_dict, dot_to_svg, left_mult, right_mult

def one_epsilon(s):
    "Convert s to use the genuine epsilon character."
    s = re.sub(r'\\\\e', '&epsilon;', s)
    return s

automaton.__eq__ = lambda self, other: str(self) == str(other)
automaton.__add__ = automaton.sum
automaton.__and__ = lambda l, r: conjunction(l, r)
automaton.__invert__ = automaton.complement
automaton.__mul__ = right_mult
automaton.__mod__ = automaton.difference
automaton.__or__ = automaton.union
automaton.__pow__ = automaton.power
automaton.__repr__ = lambda self: self.info()['type']
automaton.__rmul__ = left_mult
automaton.__str__ = lambda self: self.format('dot')
automaton.__sub__ = automaton.difference
automaton._repr_svg_ = lambda self: dot_to_svg(one_epsilon(self.format('dot')))

class conjunction(object):
    """A proxy class that delays calls to the & operator in order
    to turn a & b & c into a variadic evaluation of
    automaton.product_(a, b, c)."""
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
            self.auts = automaton.product_(self.auts)
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

def automaton_eval(self, w):
    c = self.context()
    if not isinstance(w, label):
        w = c.word(str(w))
    return self.eval_(w)
automaton.eval = automaton_eval

def automaton_load(file, format = "dot"):
    return automaton(open(file, "r").read(), format)
automaton.load = staticmethod(automaton_load)

def automaton_fst(aut, cmd):
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

automaton.fstdeterminize = lambda self: automaton_fst(self, "fstdeterminize")
automaton.fstminimize = lambda self: automaton_fst(self, "fstminimize")

automaton.info = lambda self: info_to_dict(self.format('info'))

def automaton_is_synchronized_by(self, w):
    c = self.context()
    if not isinstance(w, label):
        w = c.word(str(w))
    return self.is_synchronized_by_(w)
automaton.is_synchronized_by = automaton_is_synchronized_by

automaton.lan_to_lal = \
  lambda self: automaton(re.sub(r'"lan<(lal_char\(.*?\))>', r'"\1',
                         self.format('dot')), 'dot')

# Somewhat cheating: in Python, proper returns a LAL, not a LAN.
# proper_real is the genuine binding to dyn::proper.
automaton.proper = lambda self, prune = True: self.proper_real(prune).lan_to_lal()
