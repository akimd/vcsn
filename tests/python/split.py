#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context('lal_char(abc), expressionset<lal_char(wxyz), z>')

# check INPUT [RESULT = INPUT]
# ----------------------------
# Check that the splitting of INPUT is RESULT.
def check(re, exp = None):
    if exp is None:
        exp = re
    r = ctx.expression(re)
    s = r.split()
    CHECK_EQ(exp, s)
    # Split polynomials is idempotent.
    CHECK_EQ(s, s.split())

# fail INPUT
# ----------
def fail(re):
    re = ctx.expression(re)
    XFAIL(lambda: re.split())

fail('a*{c}')
fail('a*{\}b*')
fail('a:b')
fail('a*{T}')

check('\z')
check('<x>\e')
check('<x>a')
# FIXME: we are completely wrong on the following example, because we
# commute the product of the rweight to the level of the polynomial,
# so we produce <y>(<x>a)*!!!  As a consequence, the broken
# derived-term automaton is wrong...
#
# This needs to be fixed, but it is not entirely trivial: we need
# properties about the possibility to commute products between weights
# and labels.
#
# check('(<x>a)*<y>')
check('<xy>a<z>b')
check('<x>a+<y>b', '<x>a + <y>b')
check('<x>a+<y>b+<z>a', '<x+z>a + <y>b')

check('(<w>a+<x>b)(<y>a+<z>b)',  '<w>a(<y>a+<z>b) + <x>b(<y>a+<z>b)')
check('(<w>a+<x>b)&(<y>a+<z>b)', '<w>a&(<y>a+<z>b) + <x>b&(<y>a+<z>b)')
# The code is really different when there are more than two operands
# for conjunction.
check('(<w>a+<x>b)&(<y>a+<z>b)&(<y>a+<z>b)',
      '<w>a&(<y>a+<z>b)&(<y>a+<z>b) + <x>b&(<y>a+<z>b)&(<y>a+<z>b)')

## --------------------- ##
## Documented examples.  ##
## --------------------- ##

# On The Number Of Broken Derived Terms Of A Rational Expression.
# Example 4.
F2 = 'a*+b*'
E2 = "({F2})(a({F2}))".format(F2=F2)
check(E2, "a*a({F2}) + b*a({F2})".format(F2=F2))
check(F2, "a* + b*")
