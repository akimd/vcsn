#! /usr/bin/env python

import vcsn
from test import *

lal = vcsn.context('lal_char(abc), r')
lan = vcsn.context('lan_char(abc), r')

# check(OUTPUT, RAT_EXP)
# ----------------------
# Check that RAT-EXP's validity is OUTPUT ("true" or "false").
# Also check is-valid on its Thompson.
#
# Use a context with ratexp weights to check the order of products.
def check (exp, re):
    r = lal.ratexp(re)
    CHECK_EQ(exp, r.is_valid())
    # Check that we are consistent with the validity of the Thompson
    # of RAT_EXP.
    a = lan.ratexp(re).thompson()
    CHECK_EQ(exp, a.is_valid())

check(True, 'a*')
check(False, '(<42>b*)*')
check(True, '<64>a+<4>b+(<0.6>b*)*')
check(False, '(<0.6>a*+<0.4>b*)*')
check(True, '(<0.6>a*+<0.3>b*)*<42>')
check(True, '(<0.6>a*.<0.4>b*)*')
