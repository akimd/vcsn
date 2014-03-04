## -------- ##
## weight.  ##
## -------- ##

from vcsn_cxx import weight
from vcsn import is_equal

weight.__eq__ = is_equal
weight.__repr__ = lambda self: self.format('text')
weight._repr_latex_ = lambda self: '$' + self.format('latex') + '$'
