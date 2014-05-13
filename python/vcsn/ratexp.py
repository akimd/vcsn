## -------- ##
## ratexp.  ##
## -------- ##

from vcsn_cxx import label, ratexp
from vcsn import _is_equal, _info_to_dict, _left_mult, _right_mult

def _ratexp_power(self, exp):
    if isinstance(exp, tuple):
        return self.chain(*exp)
    else:
        return self.chain(exp)

ratexp.__add__ = ratexp.sum
ratexp.__and__ = ratexp.conjunction
ratexp.__eq__ = _is_equal
ratexp.__mod__ = ratexp.difference
ratexp.__mul__ = _right_mult
ratexp.__pow__ = _ratexp_power
ratexp.__repr__ = lambda self: self.format('text')
ratexp.__rmul__ = _left_mult
ratexp.__str__ = lambda self: self.format('text')
ratexp._repr_latex_ = lambda self: '$' + self.format('latex') + '$'

def _ratexp_derivation(self, w, *args):
    "Derive wrt. w, but convert it into a label if needed."
    c = self.context()
    if not isinstance(w, label):
        w = c.word(str(w))
    return self._derivation(w, *args)
ratexp.derivation = _ratexp_derivation

ratexp.enumerate = lambda self, num = 1: self.standard().enumerate(num)

ratexp.info = lambda self: _info_to_dict(self.format('info'))

ratexp.shortest = lambda self, num = 1: self.standard().shortest(num)
