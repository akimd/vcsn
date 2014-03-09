## ----------- ##
## automaton.  ##
## ----------- ##

import re
from vcsn_cxx import automaton, weight
from vcsn import is_equal, info_to_dict, dot_to_svg

def automaton_mul(self, rhs):
    if isinstance(rhs, weight):
        return self.right_mult(rhs)
    else:
        return self.concatenate(rhs)

automaton.__eq__ = is_equal
automaton.__add__ = automaton.sum
automaton.__and__ = automaton.product
automaton.__invert__ = automaton.complement
automaton.__mul__ = automaton_mul
automaton.__mod__ = automaton.difference
automaton.__or__ = automaton.union
automaton.__pow__ = automaton.power
automaton.__repr__ = lambda self: self.info()['type']
automaton.__str__ = lambda self: self.format('dot')
automaton.__sub__ = automaton.difference
automaton._repr_svg_ = lambda self: dot_to_svg(self.format('dot'))

def automaton_load(file, format = "dot"):
    return automaton(open(file, "r").read(), format)
automaton.load = staticmethod(automaton_load)

def automaton_fst(aut, cmd):
    open("/tmp/in.efsm", "w").write(aut.format("efsm"))
    from subprocess import check_call, check_output
    check_call(['efstcompile',   '/tmp/in.efsm', '/tmp/in.fst'])
    check_call([cmd,             '/tmp/in.fst',  '/tmp/out.fst'])
    res = check_output(['efstdecompile', '/tmp/out.fst'])
    return automaton(res, "efsm")

automaton.fstdeterminize = lambda self: automaton_fst(self, "fstdeterminize")
automaton.fstminimize = lambda self: automaton_fst(self, "fstminimize")

automaton.info = lambda self: info_to_dict(self.format('info'))

automaton.lan_to_lal = \
  lambda self: automaton(re.sub(r'"lan<(lal_char\(.*?\))>', r'"\1', self.format('dot')), 'dot')

# Somewhat cheating: in Python, proper returns a LAL, not a LAN.
# proper_real is the genuine binding to dyn::proper.
automaton.proper = lambda self, prune = True: self.proper_real(prune).lan_to_lal()
