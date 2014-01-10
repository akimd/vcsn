#! /usr/bin/env python

import vcsn
from test import *

z = vcsn.context('lal_char(abc)_z')

# check EXPECTED RAT1 RAT2
# ------------------------
# Check that are-isomorphic(RAT1.standard(), RAT2.standard()) == EXPECTED.
def check(exp, r1, r2):
    eff = z.ratexp(r1).standard().is_isomorphic(z.ratexp(r2).standard())
    if r1 != r2:
        eff2 = z.ratexp(r2).standard().is_isomorphic(z.ratexp(r1).standard())
        if eff != eff2:
            FAIL('isomorphism check is not commutative on ' + r1 + ' and ' + r2)
    if eff == exp:
        PASS()
    else:
        FAIL(r1 + ' should check as ' + ('' if exp else 'NOT')
             + ' isomorphic to ' + r2)

check(True, '\z', '\z')
check(True, '\e', '\e')
check(False, '\z', '\e')

a = '(ab*c)*'
b = '(c*ba)*'
check(False, a, '\e')
check(False, b, '\e')
check(False, a, '\z')
check(False, b, '\z')

check(True, a, a)
check(True, b, b)
check(False, a, b)
check(True, a+'+'+b, a+'+'+b)
check(True, a+'+'+b, b+'+'+a)

PLAN()
