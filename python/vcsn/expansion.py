## ----------- ##
## expansion.  ##
## ----------- ##

from vcsn_cxx import expansion
from vcsn.tools import _is_equal, _left_mult, _right_mult

expansion.__add__ = expansion.sum
expansion.__eq__ = _is_equal
expansion.__invert__ = expansion.complement
expansion.__mul__ = _right_mult
expansion.__or__ = lambda l, r: expansion._tuple([l, r])
expansion.__repr__ = lambda self: self.format('text')
expansion.__rmul__ = _left_mult
expansion._repr_latex_ = lambda self: '$' + self.format('latex') + '$'
