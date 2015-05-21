## ------- ##
## label.  ##
## ------- ##

from vcsn_cxx import label
from vcsn import _is_equal

label.__eq__ = _is_equal
label.__mul__ = label.multiply
label.__repr__ = lambda self: self.format('text')
label._repr_latex_ = lambda self: '$' + self.format('latex') + '$'
