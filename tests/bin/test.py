from difflib import unified_diff as diff
import inspect
import os
import re
import sys

import vcsn

count = 0
npass = 0
nfail = 0

# For build-checks, use our abs_srcdir from tests/bin/vcsn. For
# install checks, since the latter is not run (it runs
# /usr/local/bin/vcsn), use Automake's srcdir.
srcdir = os.environ['abs_srcdir'] if 'abs_srcdir' in os.environ \
         else os.environ['srcdir']

# The directory associated to the current test.
medir = sys.argv[0].replace(".py", ".dir")

# http://stackoverflow.com/questions/377017/test-if-executable-exists-in-python
def which(program):
    'Return the file name for program if it exists, None otherwise.'
    import os
    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)
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
    import vcsn
    return vcsn.automaton(filename = vcsn.datadir + "/" + fname)

def here():
    # Find the top-level call.
    frame = inspect.currentframe();
    while frame.f_back:
        frame = frame.f_back
    finfo = inspect.getframeinfo(frame)
    return finfo.filename + ":" + str(finfo.lineno)

def FAIL(*msg, **kwargs):
    global count, nfail
    count += 1
    nfail += 1
    # Don't display multi-line failure messages, only the first line
    # will be reported anyway by the TAP driver.
    m = ' '.join(msg)
    if m.count("\n") == 0:
        print('not ok ', count, m)
    else:
        print('not ok ', count)
    loc = kwargs['loc'] if 'loc' in kwargs and kwargs['loc'] is not None else here()
    print(loc + ": fail:", *msg)
    print()
    sys.stdout.flush()

def PASS(*msg, **kwargs):
    global count, npass
    count += 1
    npass += 1
    print('ok ', count, *msg)
    loc = kwargs['loc'] if 'loc' in kwargs and kwargs['loc'] is not None else here()
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
        if exp is None or exp in str(e):
            PASS()
        else:
            FAIL('does not include the expected error message')
            rst_file("Expected error", exp)
            rst_file("Effective error", str(e))
    else:
        FAIL('did not raise an exception', str(fun))

def CHECK(effective, msg='', loc=None):
    "Check that `effective` is `True`."
    if effective:
        PASS(loc=loc)
    else:
        FAIL("assertion failed", msg, loc=loc)

def CHECK_EQ(expected, effective, loc = None):
    "Check that `effective` is equal to `expected`."
    if isinstance(expected, str) and not isinstance(effective, str):
        effective = str(effective)
    if expected == effective:
        PASS(loc=loc)
    else:
        exp = format(expected)
        eff = format(effective)
        FAIL("Unexpected result", loc=loc)
        rst_file("Expected output", exp)
        rst_file("Effective output", eff)
        rst_diff(exp, eff)

def CHECK_NE(expected, effective, loc = None):
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
    # Eliminate nullablesets if there are that remain.  If there are
    # \e that remains, the following conversion _will_ reject it.
    to = re.sub(r'nullableset<(lal_char\(.*?\)|letterset<char_letters\(.*?\)>)>',
                r'\1',
                a.context().format('sname'))
    return a.automaton(vcsn.context(to))

def can_test_equivalence(a):
    ctx = a.context().format('sname')
    expressionset_ws = re.match('.*expressionset<.*>', ctx) is None
    return not(ctx.endswith('min')) and expressionset_ws

def CHECK_EQUIV(a1, a2):
    '''Check that `a1` and `a2` are equivalent.  Also works for
    expressions.'''
    num = 10
    # Cannot compute equivalence on Zmin, approximate with shortest.
    try:
        if can_test_equivalence(a1) and can_test_equivalence(a2):
            res = a1.is_equivalent(a2)
        else:
            res = a1.proper().shortest(num) == a2.proper().shortest(num)
    except RuntimeError as e:
        FAIL("cannot check equivalence: " + str(e))
        res = False

    if res:
        PASS()
    else:
        FAIL("not equivalent")
        rst_file("Left", format(a1))
        rst_file("Right", format(a2))
        try:
            s1 = a1.proper().shortest(num).format('list')
            s2 = a2.proper().shortest(num).format('list')
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
    print('1..'+str(count))
    print('PASS:', npass)
    print('FAIL:', nfail)

import atexit
atexit.register(PLAN)
