## ------------ ##
## expression.  ##
## ------------ ##

from vcsn_cxx import label, expression
from vcsn.tools import _extend, _is_equal, _info_to_dict, _left_mult, _right_mult

@_extend(expression)
class expression:
    _multiply_orig = expression.multiply
    def multiply(self, exp):
        if isinstance(exp, tuple):
            return self._multiply_orig(*exp)
        else:
            return self._multiply_orig(exp)

    __add__ = expression.sum
    __and__ = expression.conjunction
    __floordiv__ = expression.ldiv
    __invert__ = expression.complement
    __mod__ = expression.difference
    __mul__ = _right_mult
    __or__ = lambda l, r: expression._tuple([l, r])
    __pow__ = multiply
    __repr__ = lambda self: self.format('text')
    __rmul__ = _left_mult
    __str__ = lambda self: self.format('text')
    __truediv__ = expression.rdiv
    _repr_latex_ = lambda self: '$' + self.format('latex') + '$'

    __lt__ = expression.less_than
    __le__ = lambda l, r: not r < l
    __ge__ = lambda l, r: not l < r
    __gt__ = lambda l, r: r < l
    __eq__ = lambda l, r: not l < r and not r < l
    __ne__ = lambda l, r: l < r or r < l


    def derivation(self, w, *args):
        "Derive wrt. w, but convert it into a label if needed."
        c = self.context()
        if not isinstance(w, label):
            w = c.word(str(w))
        return self._derivation(w, *args)


    _derived_term_orig = expression.derived_term
    def derived_term(self, algo="expansion",
                     lazy=False, deterministic=False, breaking=False):
        if lazy:
            algo = 'lazy,' + algo
        for b, tag in [(breaking, "breaking"),
                       (deterministic, "deterministic")]:
            if b:
                algo += "," + tag
        return self._derived_term_orig(algo)

    def info(self, key=None, detailed=False):
        res = _info_to_dict(self.format('info'))
        return res[key] if key else res

    shortest = lambda self, *a, **kw: self.automaton().shortest(*a, **kw)
    star = lambda self: self.multiply(-1)
