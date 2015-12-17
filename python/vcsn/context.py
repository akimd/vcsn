## --------- ##
## context.  ##
## --------- ##

from vcsn_cxx import context
from vcsn import label, polynomial, expression, weight
from vcsn.tools import _is_equal

context.__eq__ = _is_equal
context.__plus__ = context.join
context.__repr__ = lambda self: self.format('text')
context._repr_latex_ = lambda self: '$' + self.format('latex') + '$'

context.expression = lambda *a, **kw: expression(*a, **kw)
context.label =      lambda *a, **kw: label(*a, **kw)
context.polynomial = lambda *a, **kw: polynomial(*a, **kw)
context.weight =     lambda *a, **kw: weight(*a, **kw)
