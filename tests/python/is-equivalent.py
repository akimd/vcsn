#! /usr/bin/env python

import vcsn
from test import *

b = vcsn.context('lal_char(ab)_b')

# check EXPECTED RAT1 RAT2
# ------------------------
# Check that are-equivalent(RAT1, RAT2) == EXPECTED.
def check(exp, r1, r2):
    eff = b.ratexp(r1).is_equivalent(b.ratexp(r2))
    if eff == exp:
        PASS()
    else:
        FAIL(exp + " != " + eff)


check(True, '\z', '\z')
check(True, '\e', '\e')
check(True, 'a', 'a')
check(True, 'ab', 'a.b')
check(True, 'a*', '\e+(a.(a*))')
check(True, 'a+b', 'b+a')
check(True, 'a*+a*+(\e+aa*)', 'a*')

check(False, '\z', '\e')
check(False, 'a', 'b')
check(False, 'ab', 'ba')
check(False, 'aa*', 'a*')

PLAN()
