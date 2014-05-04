## -------- ##
## ratexp.  ##
## -------- ##

from vcsn_cxx import label, ratexp
from vcsn import is_equal, info_to_dict, left_mult, right_mult

def ratexp_power(self, exp):
    if isinstance(exp, tuple):
        return self.chain(*exp)
    else:
        return self.chain(exp)

ratexp.__add__ = ratexp.sum
ratexp.__and__ = ratexp.conjunction
ratexp.__eq__ = is_equal
ratexp.__mod__ = ratexp.difference
ratexp.__mul__ = right_mult
ratexp.__pow__ = ratexp_power
ratexp.__repr__ = lambda self: self.format('text')
ratexp.__rmul__ = left_mult
ratexp.__str__ = lambda self: self.format('text')
ratexp._repr_latex_ = lambda self: '$' + self.format('latex') + '$'

def ratexp_derivation(self, w, *args):
    "Derive wrt. w, but convert it into a label if needed."
    c = self.context()
    if not isinstance(w, label):
        w = c.word(str(w))
    return self.derivation_(w, *args)
ratexp.derivation = ratexp_derivation

ratexp.enumerate = lambda self, num = 1: self.standard().enumerate(num)

ratexp.info = lambda self: info_to_dict(self.format('info'))

ratexp.shortest = lambda self, num = 1: self.standard().shortest(num)
