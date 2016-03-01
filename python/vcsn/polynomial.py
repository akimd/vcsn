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

        syntax = 'text'
        syntaxes = {'l': 'list',
                    't': 'text',
                    'u': 'utf8',
                    'x': 'latex',
        }

        while spec:
            c, spec = spec[0], spec[1:]
            if c in syntaxes:
                syntax = syntaxes[c]
            elif c == ':':
                break
            else:
                raise ValueError("unknown format specification: " + c + spec)

        s = self.format(syntax)
        return s.__format__(spec)


    def __pow__(p, k):
        # FIXME: cannot generate the unit polynomial with the current API.
        assert 0 < k
        res = p
        for i in range(1, k):
            res *= p
        return res
