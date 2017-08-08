#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context('lal_char(abc), q')
expr = ctx.expression

# check(OUTPUT, EXP)
# ------------------
# Check that EXP's validity is OUTPUT ("true" or "false").
# Also check is-valid on its Thompson.
#
# Use a context with expression weights to check the order of products.
def check(exp, re):
    r = expr(re)
    CHECK_EQ(exp, r.is_valid())
    # Check that we are consistent with the validity of the Thompson
    # of RAT_EXP.
    a = r.thompson()
    CHECK_EQ(exp, a.is_valid())

check(True,  'a*')
check(False, '(<42>b*)*')
check(True,  '<64>a+<4>b+(<6/10>b*)*')
check(False, '(<6/10>a*+<4/10>b*)*')
check(True,  '(<6/10>a*+<3/10>b*)*<42>')
check(True,  '(<6/10>a*.<4/10>b*)*')

# Arguably, we should be able to answer here, since there are no star
# that depends on knowing these constant-terms.  The day we pass
# instead of failing, just add a star...
e = expr(r'a{\}a')
XFAIL(lambda: e.is_valid(),
      r'''is_valid: operator ldivide not supported: a{\}a
  while computing constant-term of: a{\}a''')

e = vcsn.context('lat<lal, lal>, q').expression(r'ab|\e @ \e|cd')
XFAIL(lambda: e.is_valid(),
      r'''is_valid: operator compose not supported: ab|\e@\e|cd
  while computing constant-term of: ab|\e@\e|cd''')
