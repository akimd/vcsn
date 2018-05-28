## ------------ ##
## polynomial.  ##
## ------------ ##

from vcsn_cxx import polynomial
from vcsn.tools import _extend, _format, _lweight, _rweight

@_extend(polynomial)
class polynomial:
    __add__ = polynomial.add
    __and__ = polynomial.conjunction
    __floordiv__ = polynomial.ldivide
    __matmul__ = polynomial.compose
    __mul__ = _rweight
    __or__ = lambda l, r: polynomial._tuple([l, r])
    __repr__ = lambda self: self.format()
    __rmul__ = _lweight
    _repr_latex_ = lambda self: '$' + self.format('latex') + '$'

    __lt__ = lambda l, r: l.compare(r) <  0
    __le__ = lambda l, r: l.compare(r) <= 0
    __ge__ = lambda l, r: l.compare(r) >= 0
    __gt__ = lambda l, r: l.compare(r) >  0
    __eq__ = lambda l, r: l.compare(r) == 0
    __ne__ = lambda l, r: l.compare(r) != 0

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


    def one(self):
        r'''The polynomial for `\e`, if we can represent it.'''
        try:
            return self.context().polynomial(r'\e')
        except RuntimeError as e:
            if r'cannot represent \e' in str(e):
                raise RuntimeError(str(self.context())
                                   + ": cannot represent polynomial one")
            else:
                raise RuntimeError(e)

    def __pow__(self, k):
        if k == 0:
            res = self.one()
        else:
            res = self
            for _ in range(1, k):
                res *= self
        return res
