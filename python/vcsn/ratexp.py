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

def derivation(self, w, *args):
    "Derive wrt. w, but convert it into a label if needed."
    c = self.context()
    if not isinstance(w, label):
        w = c.word(str(w))
    return self._derivation(w, *args)
ratexp.derivation = derivation

ratexp.enumerate = lambda self, num = 1: self.standard().enumerate(num)

ratexp.info = lambda self, detailed = False: _info_to_dict(self.format('info'))

ratexp.shortest = lambda self, num = 1: self.standard().shortest(num)
ratexp.star = lambda self: self.chain(-1)

def _as_ratexp(self, context = None):
    "Convert to ratexp, possibly in a different context"
    if context is None:
        return self._as_ratexp()
    else:
        return self._as_ratexp_in(context)
ratexp.as_ratexp = _as_ratexp

def _as_series(self, context = None):
    "Convert to series, possibly in a different context"
    if context is None:
        return self._as_series()
    else:
        return self._as_series_in(context)
ratexp.as_series = _as_series
