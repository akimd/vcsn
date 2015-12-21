## ------------ ##
## expression.  ##
## ------------ ##

from vcsn_cxx import label, expression
from vcsn.tools import _is_equal, _info_to_dict, _left_mult, _right_mult

_expression_multiply_orig = expression.multiply


def _expression_multiply(self, exp):
    if isinstance(exp, tuple):
        return _expression_multiply_orig(self, *exp)
    else:
        return _expression_multiply_orig(self, exp)
expression.multiply = _expression_multiply

expression.__add__ = expression.sum
expression.__and__ = expression.conjunction
expression.__floordiv__ = expression.ldiv
expression.__invert__ = expression.complement
expression.__mod__ = expression.difference
expression.__mul__ = _right_mult
expression.__or__ = lambda l, r: expression._tuple([l, r])
expression.__pow__ = _expression_multiply
expression.__repr__ = lambda self: self.format('text')
expression.__rmul__ = _left_mult
expression.__str__ = lambda self: self.format('text')
expression.__truediv__ = expression.rdiv
expression._repr_latex_ = lambda self: '$' + self.format('latex') + '$'

expression.__lt__ = expression.less_than
expression.__le__ = lambda l, r: not r < l
expression.__ge__ = lambda l, r: not l < r
expression.__gt__ = lambda l, r: r < l
expression.__eq__ = lambda l, r: not l < r and not r < l
expression.__ne__ = lambda l, r: l < r or r < l


def _expression_derivation(self, w, *args):
    "Derive wrt. w, but convert it into a label if needed."
    c = self.context()
    if not isinstance(w, label):
        w = c.word(str(w))
    return self._derivation(w, *args)
expression.derivation = _expression_derivation


def _expression_info(self, key=None, detailed=False):
    res = _info_to_dict(self.format('info'))
    return res[key] if key else res
expression.info = _expression_info

expression.shortest = lambda self, *a, **kw: self.automaton().shortest(*a, **kw)
expression.star = lambda self: self.multiply(-1)
