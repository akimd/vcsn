## ------- ##
## label.  ##
## ------- ##

from vcsn_cxx import label
from vcsn.tools import _extend, _format, _is_equal

@_extend(label)
class label:
    __eq__ = _is_equal
    __mul__ = label.multiply
    __repr__ = lambda self: self.format('text')
    _repr_latex_ = lambda self: '$' + self.format('latex') + '$'


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
