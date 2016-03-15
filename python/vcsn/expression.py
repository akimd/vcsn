## ------------ ##
## expression.  ##
## ------------ ##

from vcsn_cxx import label, expression
from vcsn.dot import _dot_pretty, _dot_to_svg
from vcsn.tools import (_extend, _is_equal, _info_to_dict,
                        _left_mult, _right_mult)

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


    def dot(self, mode = "pretty"):
        return _dot_pretty(self.format('dot'))


    def svg(self):
        return _dot_to_svg(self.dot())


    def SVG(self):
        from IPython.display import SVG
        return SVG(self.svg(
        ))

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


    def __format__(self, spec):
        """Format the expression according to `spec`.

        Parameters
        ----------
        spec : str, optional
            a list of letters that specify how the expression
            should be formatted.

        Supported specifiers
        --------------------

        - 'd': use Graphviz's Dot syntax
        - 'i': print info
        - 't': use text ASCII syntax (default)
        - 'u': use text UTF-8 syntax
        - 'x': use LaTeX syntax

        - ':spec': pass the remaining specification to the
                   formating function for strings.

        """

        syntax = 'text'
        syntaxes = {'i': 'info',
                    't': 'text',
                    'u': 'utf8',
                    'x': 'latex',
        }

        while spec:
            c, spec = spec[0], spec[1:]
            if c in syntaxes:
                syntax = syntaxes[c]
            elif c == ':':
                break
            else:
                raise ValueError("unknown format specification: " + c + spec)

        s = self.format(syntax)
        return s.__format__(spec)


    def info(self, key=None, detailed=False):
        res = _info_to_dict(self.format('info'))
        return res[key] if key else res


    shortest = lambda self, *a, **kw: self.automaton().shortest(*a, **kw)
    star = lambda self: self.multiply(-1)
