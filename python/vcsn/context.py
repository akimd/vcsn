## --------- ##
## context.  ##
## --------- ##

from vcsn_cxx import context
from vcsn import _is_equal, label, polynomial, expression, weight

context.__eq__ = _is_equal
context.__repr__ = lambda self: self.format('text')
context._repr_latex_ = lambda self: '$' + self.format('latex') + '$'

context.expression = lambda self, v: expression(self, v)
context.label =      lambda self, v: label(self, v)
context.polynomial = lambda self, v: polynomial(self, v)
context.weight =     lambda self, v: weight(self, v)
