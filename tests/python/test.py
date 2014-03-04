from __future__ import print_function

from difflib import unified_diff as diff
import inspect, os, sys

count = 0

# For build-checks, use our abs_srcdir from tests/bin/vcsn. For
# install checks, since the latter is not run (it runs
# /usr/local/bin/vcsn), use Automake's srcdir.
srcdir = os.environ['abs_srcdir'] if 'abs_srcdir' in os.environ \
         else os.environ['srcdir']

# The directory associated to the current test.
medir = sys.argv[0].replace(".py", ".dir")

def load(fname):
    "Load the library automaton file fname."
    import vcsn
    return vcsn.automaton.load(vcsn.datadir + "/" + fname)

def here():
    # Find the top-level call.
    frame = inspect.currentframe();
    while frame.f_back:
        frame = frame.f_back
    finfo = inspect.getframeinfo(frame)
    return finfo.filename + ":" + str(finfo.lineno)

def FAIL(*msg):
    global count
    count += 1
    # Don't display multi-line failure messages, only the first line
    # will be reported anyway by the TAP driver.
    m = ' '.join(msg)
    if m.count("\n") == 0:
        print('not ok ', count, m)
    else:
        print('not ok ', count)
    print(here() + ": fail:", *msg)
    print()

def PASS(*msg):
    global count
    count += 1
    print('ok ', count, *msg)
    print()

def XFAIL(fun):
    try:
        fun()
    except RuntimeError:
        PASS()
    else:
        FAIL('did not raise an exception', fun)

def CHECK_EQ(expected, effective):
    "Check that effective value is equal to expected."
    if expected == effective:
        PASS()
    else:
        exp = str(expected)
        eff = str(effective)
        FAIL(exp + " != " + eff)
        if exp[:-1] != '\n':
            exp += '\n'
        if eff[:-1] != '\n':
            eff += '\n'
        sys.stdout.writelines(diff(exp.splitlines(1),
                                   eff.splitlines(1),
                                   fromfile='expected', tofile='effective'))

def CHECK_NEQ(expected, effective):
    "Check that effective value is not equal to expected."
    if expected == effective:
        exp = str(expected)
        eff = str(effective)
        FAIL(exp + " != " + eff)
        if exp[:-1] != '\n':
            exp += '\n'
        if eff[:-1] != '\n':
            eff += '\n'
        sys.stdout.writelines(diff(exp.splitlines(1),
                                   eff.splitlines(1),
                                   fromfile='expected', tofile='effective'))
    else:
        PASS()

def PLAN():
    "TAP requires that we announce the plan: the number of tests."
    print('1..'+str(count))

import atexit
atexit.register(PLAN)
