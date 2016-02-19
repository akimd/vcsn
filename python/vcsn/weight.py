## -------- ##
## weight.  ##
## -------- ##

from vcsn_cxx import weight
from vcsn import automaton, expression
from vcsn.tools import _extend, _is_equal

@_extend(weight)
class weight:
    __eq__ = _is_equal
    __add__ = weight.sum
    __repr__ = lambda self: self.format('text')
    _repr_latex_ = lambda self: '$' + self.format('latex') + '$'

    def __pow__(self, exp):
        if isinstance(exp, tuple):
            return self.multiply(*exp)
        else:
            return self.multiply(exp)

    def __mul__(self, rhs):
        '''Translate `weight * expression` to `expression.left_mult(weight)`.'''
        if isinstance(rhs, automaton) or isinstance(rhs, expression):
            return rhs.left_mult(self)
        elif isinstance(rhs, int):
            raise RuntimeError(
                "cannot multiply a weight by and int: need two weights")
        else:
            return self.multiply(rhs)
