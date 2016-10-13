## -------- ##
## weight.  ##
## -------- ##

from vcsn_cxx import weight
from vcsn.tools import _extend, _format, _is_equal

@_extend(weight)
class weight:
    __eq__ = _is_equal
    __add__ = weight.add
    __floordiv__ = weight.ldivide
    __pow__ = weight.multiply
    __repr__ = lambda self: self.format('text')
    __truediv__ = weight.rdivide
    _repr_latex_ = lambda self: '$' + self.format('latex') + '$'

    def __format__(self, spec):
        """Format the weight according to `spec`.

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


    def __mul__(self, rhs):
        '''Translate `weight * obj` to `obj.lweight(weight)` when applicable.'''
        if 'lweight' in dir(rhs):
            return rhs.lweight(self)
        elif isinstance(rhs, int):
            raise RuntimeError(
                "cannot multiply a weight by and int: need two weights")
        else:
            return self.multiply(rhs)


    def __pow__(self, exp):
        if isinstance(exp, tuple):
            return self.multiply(*exp)
        else:
            return self.multiply(exp)


    star = lambda self: self.multiply(-1)
