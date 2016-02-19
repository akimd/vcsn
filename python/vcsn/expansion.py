## ----------- ##
## expansion.  ##
## ----------- ##

from vcsn_cxx import expansion
from vcsn.tools import _extend, _is_equal, _left_mult, _right_mult

@_extend(expansion)
class expansion:
    __add__ = expansion.sum
    __eq__ = _is_equal
    __invert__ = expansion.complement
    __mul__ = _right_mult
    __or__ = lambda l, r: expansion._tuple([l, r])
    __repr__ = lambda self: self.format('text')
    __rmul__ = _left_mult
    _repr_latex_ = lambda self: '$' + self.format('latex') + '$'
