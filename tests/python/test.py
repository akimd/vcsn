from __future__ import print_function

from difflib import unified_diff as diff
import inspect, os, sys

def here():
    finfo = inspect.getframeinfo(inspect.currentframe().f_back.f_back)
    return finfo.filename + ":" + str(finfo.lineno)

count = 0

srcdir = os.environ["srcdir"]

# The directory associated to the current test.
medir = sys.argv[0].replace(".py", ".dir")

def FAIL(*msg):
    global count
    count += 1
    print('not ok ', count, here() + ":", *msg)

def PASS(*msg):
    global count
    count += 1
    print('ok ', count, *msg)

def CHECK_EQ(expected, effective):
    "Check that effective value is equal to expected."
    if expected == effective:
        PASS()
    else:
        FAIL(str(expected) + " != " + str(effective))
        sys.stdout.writelines(diff(str(expected).splitlines(1),
                                   str(effective).splitlines(1),
                                   fromfile='expected', tofile='effective'))

def PLAN():
    print('1..'+str(count))
