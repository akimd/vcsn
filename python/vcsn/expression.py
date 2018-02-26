## ------------ ##
## expression.  ##
## ------------ ##

from vcsn_cxx import label, expression
from vcsn.dot import _dot_pretty, _dot_to_svg
from vcsn.tools import (_extend, _format, _info_to_dict,
                        _lweight, _rweight)

@_extend(expression)
class expression:
    _multiply_orig = expression.multiply
    def multiply(self, exp):
        if isinstance(exp, tuple):
            return self._multiply_orig(*exp)
        else:
            return self._multiply_orig(exp)

    __add__ = expression.add
    __and__ = expression.conjunction
    __floordiv__ = expression.ldivide
    __invert__ = expression.complement
    __matmul__ = expression.compose
    __mod__ = expression.difference
    __mul__ = _rweight
    __or__ = lambda l, r: expression._tuple([l, r])
    __pow__ = multiply
    __repr__ = lambda self: self.format('text')
    __rmul__ = _lweight
    __str__ = lambda self: self.format('text')
    __truediv__ = expression.rdivide
    _repr_latex_ = lambda self: '$' + self.format('latex') + '$'

    __lt__ = lambda l, r: l.compare(r) <  0
    __le__ = lambda l, r: l.compare(r) <= 0
    __ge__ = lambda l, r: l.compare(r) >= 0
    __gt__ = lambda l, r: l.compare(r) >  0
    __eq__ = lambda l, r: l.compare(r) == 0
    __ne__ = lambda l, r: l.compare(r) != 0


    # Turn automaton into a native Python function
    # so that we can then give it attributes (PEP 232).
    def automaton(self, algo='auto'):
        return self._automaton(algo)

    def dot(self, physical=False):
        '''The Graphviz Dot text.

        Parameters
        ----------
        physical : bool, optional
            whether to display the physical representation, as a DAG,
            rather than the logical tree.

        '''

        return _dot_pretty(self.format('dot,physical' if physical else 'dot'))


    def svg(self, physical=False):
        '''The SVG text.

        Parameters
        ----------
        physical : bool, optional
            whether to display the physical representation, as a DAG,
            rather than the logical tree.

        '''

        return _dot_to_svg(self.dot(physical))


    def SVG(self, physical=False):
        '''The IPython SVG object.

        Parameters
        ----------
        physical : bool, optional
            whether to display the physical representation, as a DAG,
            rather than the logical tree.

        '''

        from IPython.display import SVG
        return SVG(self.svg(physical))


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

        - 'd': use Graphviz's Dot syntax to display logical tree
        - 'D': use Graphviz's Dot syntax to display implementation DAG
        - 'e': use ERE's syntax (i.e., like grep -E)
        - 'i': print info
        - 'r': use redgrep's syntax (i.e., ERE, with support for `&` and `!`)
        - 't': use text ASCII syntax (default)
        - 'u': use text UTF-8 syntax
        - 'x': use LaTeX syntax

        - ':spec': pass the remaining specification to the
                   formating function for strings.

        """

        syntaxes = {'d': 'dot',
                    'D': 'dot,physical',
                    'e': 'ere',
                    'i': 'info',
                    'r': 'redgrep',
                    't': 'text',
                    'u': 'utf8',
                    'x': 'latex',
        }
        return _format(self, spec, 'text', syntaxes)


    def info(self, key=None):
        res = _info_to_dict(self.format('info'))
        return res[key] if key else res


    def is_extended(self):
        '''Whether this expression uses extended operators.'''
        ext = ['complement', 'compose', 'conjunction', 'infiltrate', 'ldivide',
               'shuffle', 'transposition', 'tuple']
        i = self.info()
        for op in ext:
            if i[op]:
                return True
        return False


    shortest = lambda self, *a, **kw: self.automaton().shortest(*a, **kw)
    star = lambda self: self.multiply(-1)
    type = lambda self: self.info('type')
