## ------------ ##
## expression.  ##
## ------------ ##

from vcsn_cxx import label, expression
from vcsn import _is_equal, _info_to_dict, _left_mult, _right_mult

def _expression_power(self, exp):
    if isinstance(exp, tuple):
        return self.chain(*exp)
    else:
        return self.chain(exp)

expression.__add__ = expression.sum
expression.__and__ = expression.conjunction
expression.__eq__ = _is_equal
expression.__mod__ = expression.difference
expression.__mul__ = _right_mult
expression.__pow__ = _expression_power
expression.__repr__ = lambda self: self.format('text')
expression.__rmul__ = _left_mult
expression.__str__ = lambda self: self.format('text')
expression._repr_latex_ = lambda self: '$' + self.format('latex') + '$'

def _expression_derivation(self, w, *args):
    "Derive wrt. w, but convert it into a label if needed."
    c = self.context()
    if not isinstance(w, label):
        w = c.word(str(w))
    return self._derivation(w, *args)
expression.derivation = _expression_derivation

def _expression_info(self, key = None, detailed = False):
    res = _info_to_dict(self.format('info'))
    return res[key] if key else res
expression.info = _expression_info

expression.shortest = lambda self, *a, **kw: self.derived_term().strip().shortest(*a, **kw)
expression.star = lambda self: self.chain(-1)
