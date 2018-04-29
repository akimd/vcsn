## ----------- ##
## expansion.  ##
## ----------- ##

from vcsn_cxx import expansion
from vcsn.tools import _extend, _format, _is_equal, _lweight, _rweight

@_extend(expansion)
class expansion:
    __add__ = expansion.add
    __and__ = expansion.conjunction
    __eq__ = _is_equal
    __floordiv__ = expansion.ldivide
    __invert__ = expansion.complement
    __matmul__ = expansion.compose
    __mul__ = _rweight
    __or__ = lambda l, r: expansion._tuple([l, r])
    __repr__ = lambda self: self.format('utf8')
    __rmul__ = _lweight
    _repr_latex_ = lambda self: '$' + self.format('latex') + '$'


    def __format__(self, spec):
        """Format the expansion according to `spec`.

        Parameters
        ----------
        spec : str, optional
            a list of letters that specify how the expansion
            should be formatted.

        Supported specifiers
        --------------------

        - 't': use text ASCII syntax (default)
        - 'u': use text UTF-8 syntax
        - 'x': use LaTeX syntax

        - ':spec': pass the remaining specification to the
                   formating function for strings.

        """

        syntaxes = {'t': 'text',
                    'u': 'utf8',
                    'x': 'latex',
        }
        return _format(self, spec, 'text', syntaxes)
