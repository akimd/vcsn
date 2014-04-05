## -------- ##
## ratexp.  ##
## -------- ##

from vcsn_cxx import ratexp
from vcsn import is_equal, info_to_dict, automaton_mul

ratexp.__add__ = ratexp.sum
ratexp.__and__ = ratexp.conjunction
ratexp.__eq__ = is_equal
ratexp.__mod__ = ratexp.difference
ratexp.__mul__ = automaton_mul
ratexp.__repr__ = lambda self: self.format('text')
ratexp.__str__ = lambda self: self.format('text')
ratexp._repr_latex_ = lambda self: '$' + self.format('latex') + '$'

ratexp.info = lambda self: info_to_dict(self.format('info'))
