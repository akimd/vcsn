#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context('lal_char(abc)_ratexpset<lal_char(wxyz)_z>')

# check RAT-EXP RESULT
# --------------------
# Check that the splitting of RAT-EXP is RESULT.
def check(re, exp):
    r = ctx.ratexp(re)
    s = r.split()
    CHECK_EQ(exp, str(s))
    # Split polynomials is idempotent.
    CHECK_EQ(s, s.split())

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
F2 = 'a*+b*'
E2 = "({F2})(a({F2}))".format(F2=F2)
check(E2, "a*a({F2}) + b*a({F2})".format(F2=F2))
check(F2, "a* + b*")
