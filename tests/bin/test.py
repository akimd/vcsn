from difflib import unified_diff as diff
import atexit
import inspect
import os
import re
import sys

import vcsn

try:
    repatterntype = re.Pattern
except AttributeError:
    # Before Python 3.7
    repatterntype = re._pattern_type

ntest = 0
npass = 0
nfail = 0

def num_fail():
    return nfail

def num_pass():
    return npass

def num_test():
    return ntest


# For build-checks, use our abs_srcdir from tests/bin/vcsn. For
# install checks, since the latter is not run (it runs
# /usr/local/bin/vcsn), use Automake's srcdir.
srcdir = os.environ['abs_srcdir'] if 'abs_srcdir' in os.environ \
    else os.environ['srcdir']

# The directory associated to the current test.
medir = sys.argv[0].replace(".py", ".dir")


def set_medir(dir):
    global medir
    medir = dir


def mefile(fn, ext=None):
    '''The path to the test's file `fn.ext`,
    where `ext` is possibly empty.'''
    return medir + '/' + fn + ('.' + ext if ext else '')

def metext(fn, ext=None):
    '''The content of the test's file `fn.ext`,
    where `ext` is possibly empty.'''
    return open(mefile(fn, ext)).read().strip()

def meaut(fn, ext=None):
    '''The automaton stored in the test's file `fn.ext`,
    where `ext` is possibly empty.'''
    return vcsn.automaton(filename=mefile(fn, ext))


# http://stackoverflow.com/questions/377017/test-if-executable-exists-in-python
def which(program):
    'Return the file name for program if it exists, None otherwise.'

    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, _ = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            path = path.strip('"')
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file

    return None


def rst_file(name, content):
    print(name + "::")
    print()
    for l in content.splitlines():
        print("\t" + l)
    print()


def format(d):
    '''Pretty-print `d` into a diffable string.'''
    if isinstance(d, dict):
        return "\n".join(["{}: {}".format(k, format(d[k]))
                          for k in sorted(d.keys())])
    else:
        return str(d)


def rst_diff(expected, effective):
    "Report the difference bw `expected` and `effective`."
    exp = format(expected)
    eff = format(effective)
    if exp[:-1] != '\n':
        exp += '\n'
    if eff[:-1] != '\n':
        eff += '\n'
    rst_file("Diff on output",
             ''.join(diff(exp.splitlines(1),
                          eff.splitlines(1),
                          fromfile='expected', tofile='effective')))


def load(fname):
    "Load the library automaton file `fname`."
    return vcsn.automaton(filename=vcsn.datadir + "/" + fname)


def here():
    # Find the top-level call.
    frame = inspect.currentframe()
    while frame.f_back:
        frame = frame.f_back
    finfo = inspect.getframeinfo(frame)
    return finfo.filename + ":" + str(finfo.lineno)


def automaton(a):
    if not isinstance(a, vcsn.automaton):
        a = vcsn.automaton(a)
    return a


def FAIL(*msg, **kwargs):
    global ntest, nfail # pylint: disable=global-statement
    ntest += 1
    nfail += 1
    # Don't display multi-line failure messages, only the first line
    # will be reported anyway by the TAP driver.
    m = ' '.join(msg)
    if m.count("\n") == 0:
        print('not ok', ntest, m)
    else:
        print('not ok', ntest)
    loc = kwargs['loc'] if 'loc' in kwargs and kwargs['loc'] else here()
    print(loc + ": fail:", *msg)
    print()
    sys.stdout.flush()


def PASS(*msg, **kwargs):
    global ntest, npass # pylint: disable=global-statement
    ntest += 1
    npass += 1
    print('ok ', ntest, *msg)
    loc = kwargs['loc'] if 'loc' in kwargs and kwargs['loc'] else here()
    print(loc + ": pass:", *msg)
    print()
    sys.stdout.flush()


def SKIP(*msg):
    PASS('# SKIP', *msg)


def XFAIL(fun, exp=None):
    '''Run `fun`: it should fail.  If `exp` is given, check that the
    exception includes it.'''
    try:
        fun()
    except RuntimeError as e:
        # Error messages about files that we read will have $srcdir as
        # prefix.  Remove it.  E.g., from
        #   while reading automaton: ../../tests/python/efsm.dir/bad_final_weight.efsm
        # to
        #   while reading automaton: efsm.dir/bad_final_weight.efsm
        eff = str(e).replace(medir + '/', '')
        if (exp is None
            or isinstance(exp, repatterntype) and re.match(exp, eff)
            or isinstance(exp, str) and exp in eff):
            PASS()
        else:
            FAIL('Unexpected error message')
            rst_file("Expected error", exp)
            rst_file("Effective error", eff)
            rst_diff(exp, str(e))
    else:
        FAIL('did not raise an exception', str(fun))


def CHECK(effective, msg='', loc=None):
    "Check that `effective` is `True`."
    if effective:
        PASS(loc=loc)
    else:
        FAIL("assertion failed", msg, loc=loc)


def CHECK_EQ(expected, effective, loc=None):
    'Check that `effective` is equal to `expected`.'
    aut = isinstance(effective, vcsn.automaton)
    if isinstance(expected, str) and not isinstance(effective, str):
        effective = str(effective)
    if expected == effective:
        PASS(loc=loc)
    else:
        msg = 'Unexpected result'
        if aut:
            expaut = automaton(expected)
            effaut = automaton(effective)
            if not effaut.is_accessible():
                msg += (" (different, but cannot check whether isomorphic"
                        ", as is not accessible)")
            elif expaut.is_isomorphic(effaut):
                msg += ' (different but isomorphic)'
            else:
                msg += ' (different and not even isomorphic)'
        exp = format(expected)
        eff = format(effective)
        FAIL(msg, loc=loc)
        rst_file("Expected output", exp)
        rst_file("Effective output", eff)
        rst_diff(exp, eff)


def CHECK_NE(expected, effective, loc=None):
    "Check that `effective` is not equal to `expected`."
    if isinstance(expected, str) and not isinstance(effective, str):
        effective = str(effective)
    if expected != effective:
        PASS(loc=loc)
    else:
        exp = format(expected)
        eff = format(effective)
        FAIL("Unexpected equality", loc=loc)
        rst_file("First argument", exp)
        rst_file("Second argument", eff)
        rst_diff(exp, eff)


def normalize(a):
    '''Turn automaton `a` into something we can check equivalence with.'''
    a = a.strip().realtime()
    # Eliminate nullablesets if there are that remain.  This is safe:
    # if there are \e that remain, the following conversion _will_
    # fail.
    to = re.sub(r'nullableset<(lal_char\(.*?\)|letterset<char_letters\(.*?\)>)>',
                r'\1',
                a.context().format('sname'))
    return a.automaton(vcsn.context(to))


def can_test_equivalence(a):
    ctx = a.context().format('sname')
    # Cannot check on Zmin and the like.
    if ctx.endswith('min'):
        return False
    # Cannot check on expressionset as weightset.
    if 'expressionset<' in ctx:
        return False
    # Cannot check equivalence on labelset.
    if 'lat<' in ctx:
        return False
    return True

def shortest(e, num=20):
    if isinstance(e, vcsn.automaton):
        e = e.proper()
    return e.shortest(num)

def CHECK_EQUIV(a1, a2):
    '''Check that `a1` and `a2` are equivalent.  Works for
    two automata, or two expressions.'''
    # If we cannot check equivalence, check equality of the `num`
    # shortest monomials.
    num = 20
    # Cannot compute equivalence on Zmin, approximate with shortest.
    try:
        if can_test_equivalence(a1) and can_test_equivalence(a2):
            res = a1.is_equivalent(a2)
            via = '(via is_equivalent)'
        else:
            res = shortest(a1, num) == shortest(a2, num)
            via = '(via shortests)'
    except RuntimeError as e:
        FAIL("cannot check equivalence: " + str(e))
        res = False
        via = ''

    if res:
        PASS()
    else:
        FAIL("not equivalent", via)
        rst_file("Left", format(a1))
        rst_file("Right", format(a2))
        try:
            s1 = shortest(a1, num).format('list')
            s2 = shortest(a2, num).format('list')
            rst_file("Left shortest", s1)
            rst_file("Right shortest", s2)
            rst_diff(s1, s2)
        except RuntimeError as e:
            FAIL("cannot run shortest: " + str(e))


def CHECK_ISOMORPHIC(a1, a2):
    "Check that `a1` and `a2` are isomorphic."
    if a1.is_isomorphic(a2):
        PASS()
    else:
        a1 = format(a1)
        a2 = format(a2)
        FAIL("automata are not isomorphic")
        rst_file("Left automaton", a1)
        rst_file("Right automaton", a2)
        rst_diff(a1, a2)


def CHECK_IS_EPS_ACYCLIC(a):
    "Check if `a` is epsilon acyclic."
    if a.is_eps_acyclic():
        PASS()
    else:
        a = format(a)
        FAIL("automata is not epsilon acyclic")
        rst_file(a)


def PLAN():
    "TAP requires that we announce the plan: the number of tests."
    print('1..' + str(ntest))
    print('PASS:', npass)
    print('FAIL:', nfail)

def weightset_of(ctx):
    s = str(ctx).lower().split('-> ')
    return s[-1] if 0 < len(s) else None

atexit.register(PLAN)
