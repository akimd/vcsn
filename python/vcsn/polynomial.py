## ------------ ##
## polynomial.  ##
## ------------ ##

from vcsn_cxx import polynomial
from vcsn.tools import _extend, _format, _is_equal, _left_mult, _right_mult

@_extend(polynomial)
class polynomial:
    __add__ = polynomial.sum
    __and__ = polynomial.conjunction
    __eq__ = _is_equal
    __matmul__ = polynomial.compose
    __mul__ = _right_mult
    __or__ = lambda l, r: polynomial._tuple([l, r])
    __repr__ = lambda self: self.format('text')
    __rmul__ = _left_mult
    _repr_latex_ = lambda self: '$' + self.format('latex') + '$'


    def __format__(self, spec):
        """Format the polynomial according to `spec`.

        Parameters
        ----------
        spec : str, optional
            a list of letters that specify how the polynomial
            should be formatted.

        Supported specifiers
        --------------------

        - 't': use text ASCII syntax (default)
        - 'u': use text UTF-8 syntax
        - 'l': use list syntax
        - 'x': use LaTeX syntax

        - ':spec': pass the remaining specification to the
                   formating function for strings.

        """

        syntaxes = {'l': 'list',
                    't': 'text',
                    'u': 'utf8',
                    'x': 'latex',
        }
        return _format(self, spec, 'text', syntaxes)


    def __pow__(self, k):
        if k == 0:
            raise RuntimeError("cannot compute power 0 of a polynomial")
        res = self
        for _ in range(1, k):
            res *= self
        return res
