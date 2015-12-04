## ------------ ##
## polynomial.  ##
## ------------ ##

from vcsn_cxx import polynomial
from vcsn.tools import _is_equal, _right_mult

polynomial.__add__ = polynomial.sum
polynomial.__and__ = polynomial.conjunction
polynomial.__eq__ = _is_equal
polynomial.__mul__ = _right_mult
polynomial.__repr__ = lambda self: self.format('text')
polynomial._repr_latex_ = lambda self: '$' + self.format('latex') + '$'


def _polynomial_pow(p, k):
    # FIXME: cannot generate the unit polynomial with the current API.
    assert 0 < k
    res = p
    for i in range(1, k):
        res *= p
    return res
polynomial.__pow__ = _polynomial_pow
