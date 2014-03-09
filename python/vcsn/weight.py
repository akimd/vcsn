## -------- ##
## weight.  ##
## -------- ##

from vcsn_cxx import weight
from vcsn import automaton, is_equal, ratexp

def weight_mul(self, rhs):
    if isinstance(rhs, automaton) or isinstance(rhs, ratexp):
        return rhs.left_mult(self)
    else:
        return self.mul(rhs)

weight.__eq__ = is_equal
weight.__mul__ = weight_mul
weight.__repr__ = lambda self: self.format('text')
weight._repr_latex_ = lambda self: '$' + self.format('latex') + '$'
