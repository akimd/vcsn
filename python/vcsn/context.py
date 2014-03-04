## --------- ##
## context.  ##
## --------- ##

from vcsn_cxx import context, ratexp
from vcsn import is_equal

context.__eq__ = is_equal
context.__repr__ = lambda self: self.format('text')
context._repr_latex_ = lambda self: '$' + self.format('latex') + '$'

def make_polynomial(ctx, p):
    return polynomial(ctx, p)
context.polynomial = make_polynomial

def make_ratexp(ctx, re):
    return ratexp(ctx, re)
context.ratexp = make_ratexp
