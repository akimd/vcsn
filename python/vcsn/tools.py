import tempfile
import timeit
from vcsn_cxx import weight


def _extend(*classes):
    """
    Decorator that extends all the given classes with the contents
    of the class currently being defined.
    """
    def wrap(this):
        for cls in classes:
            for (name, val) in this.__dict__.items():
                if name not in ('__dict__', '__weakref__') \
                   and not (name == '__doc__' and val is None):
                    setattr(cls, name, val)
        return classes[0]
    return wrap


def _info_to_dict(info):
    '''Convert a "key: value" list of lines into a dictionary.
    Convert Booleans into bool, and likewise for integers.
    '''
    res = {}
    # Since Python 3.7, dictionaries are guaranteed to preserve the
    # order.  So let's use a nice one.
    #
    # We used to rely on IPython displaying dictionaries in order in
    # our test-suite (doc/notebooks), but with Python 3.7, IPython no
    # longer sorts the dictionaries.  Sorting here although makes it
    # easier for our tests.
    for l in sorted(info.splitlines()):
        k, v = l.split(':', 1)
        v = v.strip()

        # Beware that we may display "N/A" for Boolean (e.g., "is
        # ambiguous" for non LAL), and for integers (e.g., "number of
        # deterministic states" for non LAL).
        # Don't convert "0" and "1", which are used for false and
        # true, as integer: special case categories that start
        # with "is ".
        if k.startswith('is '):
            if v in ['false', '0']:
                v = False
            elif v in ['true', '1']:
                v = True
        # Otherwise, if it passes the conversion into a number,
        # make it a number.
        else:
            try:
                v = int(v)
            except ValueError:
                pass
        res[k] = v
    return res


def _format(self, spec, syntax, syntaxes):
    """Format `self` weight according to `spec`.

    Parameters
    ----------
    spec : str, optional
        a list of letters that specify how the label
        should be formatted.

    """

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


# FIXME: Get rid of this.
def _is_equal(lhs, rhs):
    "A stupid string-based comparison.  Must be eliminated once we DRT."
    return isinstance(rhs, lhs.__class__) and str(lhs) == str(rhs)


def _lweight(self, lhs):
    '''Support "weight * self".  Serves for automata, expansions,
    expressions and polynomials.'''
    return self.lweight(self.context().weight(str(lhs)))


def _rweight(self, rhs):
    '''Support both "aut * aut" and "aut * weight".  Also serves for
    expressions and polynomials.'''
    if isinstance(rhs, type(self)):
        return self.multiply(rhs)
    elif isinstance(rhs, weight):
        return self.rweight(rhs)
    else:
        return self.rweight(self.context().weight(str(rhs)))


def _timeit(stmt="pass", setup="pass", repeat=3):
    """
    http://stackoverflow.com/q/19062202/190597 (endolith)
    Imitate default behavior when timeit is run as a script.

    Runs enough loops so that total execution time is greater than 0.2 sec,
    and then repeats that 3 times and keeps the lowest value.

    Returns the number of loops and the time for each loop in microseconds.
    """
    t = timeit.Timer(stmt, setup)

    # determine number so that 0.2 <= total time < 2.0
    for i in range(1, 10):
        number = 10**i
        x = t.timeit(number) # seconds
        if 0.2 <= x:
            break
    r = t.repeat(repeat, number)
    best = min(r)
    usec = best * 1e6 / number
    return number, usec


def _tmp_file(suffix, **kwargs):
    '''A NamedTemporaryFile suitable for Vcsn.'''
    return tempfile.NamedTemporaryFile(prefix='vcsn-',
                                       suffix=('.' + suffix),
                                       **kwargs)

def _tuple(args):
    'Allow `vcsn.tuple(a, b, c)` rather than `a._tuple([b, c])`.'
    # pylint: disable=protected-access
    return args[0]._tuple(list(args))
