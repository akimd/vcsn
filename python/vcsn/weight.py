## -------- ##
## weight.  ##
## -------- ##

from vcsn_cxx import weight
from vcsn import automaton, _is_equal, ratexp

def _weight_mul(self, rhs):
    if isinstance(rhs, automaton) or isinstance(rhs, ratexp):
        return rhs.left_mult(self)
    else:
        return self.multiply(rhs)

weight.__eq__ = _is_equal
weight.__add__ = weight.sum
weight.__mul__ = _weight_mul
weight.__repr__ = lambda self: self.format('text')
weight._repr_latex_ = lambda self: '$' + self.format('latex') + '$'
