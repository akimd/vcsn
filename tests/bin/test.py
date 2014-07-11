from __future__ import print_function

from difflib import unified_diff as diff
import inspect
import os
import re
import sys

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
    """Pretty-print `d` into a diffable string."""
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
    return vcsn.automaton.load(vcsn.datadir + "/" + fname)

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

def PASS(*msg, **kwargs):
    global count, npass
    count += 1
    npass += 1
    print('ok ', count, *msg)
    loc = kwargs['loc'] if 'loc' in kwargs and kwargs['loc'] is not None else here()
    print(loc + ": pass:", *msg)
    print()

def SKIP(*msg):
    PASS('SKIP', *msg)

def XFAIL(fun):
    try:
        fun()
    except RuntimeError:
        PASS()
    else:
        FAIL('did not raise an exception', str(fun))

def CHECK_EQ(expected, effective, loc = None):
    "Check that `effective` is equal to `expected`."
    if isinstance(expected, str) and not isinstance(effective, str):
        effective = str(effective)
    if expected == effective:
        PASS(loc=loc)
    else:
        exp = format(expected)
        eff = format(effective)
        msg = exp + " != " + eff
        if msg.count("\n") == 0:
            FAIL(msg, loc=loc)
        else:
            FAIL(loc=loc)
            rst_file("Expected output", exp)
            rst_file("Effective output", eff)
        rst_diff(exp, eff)

def CHECK_EQUIV(a1, a2):
    """Check that `a1` and `a2` are equivalent."""
    num = 10
    a1 = a1.strip()
    a2 = a2.strip()
    if str(a1.context()).startswith('lan'):
        a1 = a1.proper()
    if str(a2.context()).startswith('lan'):
        a2 = a2.proper()

    # Cannot compare automata on Zmin.
    if str(a1.context()).endswith('zmin') or str(a2.context()).endswith('zmin'):
        res = a1.shortest(num) == a2.shortest(num)
    else:
        res = a1.is_equivalent(a2)

    if res:
        PASS()
    else:
        FAIL("automata are not equivalent")
        rst_file("Left automaton", a1)
        rst_file("Right automaton", a2)
        s1 = a1.shortest(num).format('list')
        s2 = a2.shortest(num).format('list')
        rst_file("Left automaton shortest", s1)
        rst_file("Right automaton shortest", s2)
        rst_diff(s1, s2)

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

def PLAN():
    "TAP requires that we announce the plan: the number of tests."
    print('1..'+str(count))
    print('PASS:', npass)
    print('FAIL:', nfail)

import atexit
atexit.register(PLAN)
