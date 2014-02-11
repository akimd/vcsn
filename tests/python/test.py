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
    print('not ok ', count, *msg)
    print(here() + ": fail:", *msg)
    print()

def PASS(*msg):
    global count
    count += 1
    print('ok ', count, *msg)
    print()

def CHECK_EQ(expected, effective):
    "Check that effective value is equal to expected."
    if expected == effective:
        PASS()
    else:
        FAIL(str(expected) + " != " + str(effective))
        if expected[:-1] != '\n':
            expected += '\n'
        if effective[:-1] != '\n':
            effective += '\n'
        sys.stdout.writelines(diff(str(expected).splitlines(1),
                                   str(effective).splitlines(1),
                                   fromfile='expected', tofile='effective'))

def PLAN():
    print('1..'+str(count))
