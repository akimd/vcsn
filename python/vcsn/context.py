## --------- ##
## context.  ##
## --------- ##

from vcsn_cxx import context
from vcsn import label, polynomial, expression, weight
from vcsn.tools import _extend, _is_equal

@_extend(context)
class context:
    __eq__ = _is_equal
    __plus__ = context.join
    __repr__ = lambda self: self.format('text')
    _repr_latex_ = lambda self: '$' + self.format('latex') + '$'

    expression = lambda *a, **kw: expression(*a, **kw)
    label =      lambda *a, **kw: label(*a, **kw)
    polynomial = lambda *a, **kw: polynomial(*a, **kw)
    weight =     lambda *a, **kw: weight(*a, **kw)
