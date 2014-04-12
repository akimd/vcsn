## ------- ##
## label.  ##
## ------- ##

from vcsn_cxx import label
from vcsn import is_equal

label.__eq__ = is_equal
label.__repr__ = lambda self: self.format('text')
label._repr_latex_ = lambda self: '$' + self.format('latex') + '$'
