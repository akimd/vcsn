## ------- ##
## label.  ##
## ------- ##

from vcsn_cxx import label
from vcsn.tools import _extend, _format

@_extend(label)
class label:
    __floordiv__ = label.ldivide
    __matmul__ = label.compose
    __mul__ = label.multiply
    __pow__ = label.multiply
    __repr__ = lambda self: self.format()
    __truediv__ = label.rdivide
    _repr_latex_ = lambda self: '$' + self.format('latex') + '$'

    __lt__ = lambda l, r: l.compare(r) <  0
    __le__ = lambda l, r: l.compare(r) <= 0
    __ge__ = lambda l, r: l.compare(r) >= 0
    __gt__ = lambda l, r: l.compare(r) >  0
    __eq__ = lambda l, r: l.compare(r) == 0
    __ne__ = lambda l, r: l.compare(r) != 0

    def __format__(self, spec):
        """Format the label according to `spec`.

        Parameters
        ----------
        spec : str, optional
            a list of letters that specify how the label
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
