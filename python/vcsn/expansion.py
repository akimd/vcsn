## ----------- ##
## expansion.  ##
## ----------- ##

from vcsn_cxx import expansion
from vcsn import is_equal

expansion.__eq__ = is_equal
expansion.__repr__ = lambda self: self.format('text')
expansion._repr_latex_ = lambda self: '$' + self.format('latex') + '$'
