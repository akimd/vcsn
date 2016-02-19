## ------------ ##
## polynomial.  ##
## ------------ ##

from vcsn_cxx import polynomial
from vcsn.tools import _extend, _is_equal, _right_mult

@_extend(polynomial)
class polynomial:
    __add__ = polynomial.sum
    __and__ = polynomial.conjunction
    __eq__ = _is_equal
    __mul__ = _right_mult
    __repr__ = lambda self: self.format('text')
    _repr_latex_ = lambda self: '$' + self.format('latex') + '$'

    def __pow__(p, k):
        # FIXME: cannot generate the unit polynomial with the current API.
        assert 0 < k
        res = p
        for i in range(1, k):
            res *= p
        return res
