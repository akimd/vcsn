## ------------ ##
## polynomial.  ##
## ------------ ##

from vcsn_cxx import polynomial
from vcsn import _is_equal, _right_mult

polynomial.__eq__ = _is_equal
polynomial.__mul__ = _right_mult
polynomial.__repr__ = lambda self: self.format('text')
polynomial._repr_latex_ = lambda self: '$' + self.format('latex') + '$'
