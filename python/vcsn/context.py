## --------- ##
## context.  ##
## --------- ##

from vcsn_cxx import context
from vcsn import label, polynomial, expression, weight
from vcsn.tools import _extend, _format, _is_equal

@_extend(context)
class context:
    __eq__ = _is_equal
    __plus__ = context.join
    __repr__ = lambda self: self.format('text')
    _repr_latex_ = lambda self: '$' + self.format('latex') + '$'


    def __format__(self, spec):
        """Format the context according to `spec`.

        Parameters
        ----------
        spec : str, optional
            a list of letters that specify how the context
            should be formatted.

        Supported specifiers
        --------------------

        - 's': use the `sname` format, used to generate code
        - 't': use text ASCII syntax (default)
        - 'u': use text UTF-8 syntax
        - 'x': use LaTeX syntax

        - ':spec': pass the remaining specification to the
                   formating function for strings.

        """

        syntaxes = {'s': 'sname',
                    't': 'text',
                    'u': 'utf8',
                    'x': 'latex',
        }
        return _format(self, spec, 'text', syntaxes)


    expression = lambda *a, **kw: expression(*a, **kw)
    label =      lambda *a, **kw: label(*a, **kw)
    polynomial = lambda *a, **kw: polynomial(*a, **kw)
    weight =     lambda *a, **kw: weight(*a, **kw)
