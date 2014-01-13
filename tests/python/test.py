from __future__ import print_function
import inspect

def here():
    finfo = inspect.getframeinfo(inspect.currentframe().f_back.f_back)
    return finfo.filename + ":" + str(finfo.lineno)

count = 0

def FAIL(*msg):
    global count
    count += 1
    print('not ok ', count, here() + ":", *msg)

def PASS(*msg):
    global count
    count += 1
    print('ok ', count, *msg)

def PLAN():
    print('1..'+str(count))
