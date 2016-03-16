## ----------- ##
## expansion.  ##
## ----------- ##

from vcsn_cxx import expansion
from vcsn.tools import _extend, _format, _is_equal, _left_mult, _right_mult

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
