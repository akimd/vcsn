## -------- ##
## weight.  ##
## -------- ##

from vcsn_cxx import weight
from vcsn import automaton, _is_equal, expression

def _weight_pow(self, exp):
    if isinstance(exp, tuple):
        return self.multiply(*exp)
    else:
        return self.multiply(exp)

def _weight_mul(self, rhs):
    '''Translate `weight * expression` to `expression.left_mult(weight)`.'''
    if isinstance(rhs, automaton) or isinstance(rhs, expression):
        return rhs.left_mult(self)
    elif isinstance(rhs, int):
        raise RuntimeError("cannot multiply a weight by and int: need two weights")
    else:
        return self.multiply(rhs)

weight.__eq__ = _is_equal
weight.__add__ = weight.sum
weight.__mul__ = _weight_mul
weight.__pow__ = _weight_pow
weight.__repr__ = lambda self: self.format('text')
weight._repr_latex_ = lambda self: '$' + self.format('latex') + '$'
