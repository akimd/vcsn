#! /usr/bin/env python

# check RAT-EXP RESULT
# --------------------
# Check that the splitting of RAT-EXP is RESULT.

import inspect, os, sys
import vcsn

ctx=vcsn.context('lal_char(abc)_ratexpset<lal_char(wxyz)_z>')
count = 0

def check(re, res):
    global count
    count += 1
    finfo = inspect.getframeinfo(inspect.currentframe().f_back)
    here = finfo.filename + ":" + str(finfo.lineno)
    r = ctx.ratexp(re)
    s = r.split()
    if str(s) == res:
        print "ok", count
    else:
        msg = r + " != " + res
        print here + ":", msg
        print 'not ok ', count, msg


check('<x>\z', '\z')
check('<x>\e', '<x>\e')
check('<x>a', '<x>a')
check('<xy>a<z>b', '<xy>a<z>b')
check('<x>a+<y>b', '<x>a + <y>b')
check('<x>a+<y>b+<z>a', '<x+z>a + <y>b')

check('(<w>a+<x>b)(<y>a+<z>b)', '<w>a(<y>a+<z>b) + <x>b(<y>a+<z>b)')
check('(<w>a+<x>b)&(<y>a+<z>b)', '<w>a&(<y>a+<z>b) + <x>b&(<y>a+<z>b)')

## --------------------- ##
## Documented examples.  ##
## --------------------- ##

# On The Number Of Broken Derived Terms Of A Rational Expression.
# Example 4.
F2='a*+b*'
E2="(" + F2 + ")(a(" + F2 + "))"
check(E2, "a*a(" + F2 + ") + b*a(" + F2 + ")")
check(F2, "a* + b*")


print '1..'+str(count)
