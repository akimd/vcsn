#! /usr/bin/env python

import vcsn
from test import *

z = vcsn.context('lal_char(abc)_z')

# check EXPECTED RAT1 RAT2
# ------------------------
# Check that RAT1.standard().is_isomorphic(RAT2.standard()) == EXPECTED
# and that swapping RAT1 and RAT2 yield the same result.
def check(exp, r1, r2):
    eff = z.ratexp(r1).standard().is_isomorphic(z.ratexp(r2).standard())
    if r1 != r2:
        eff2 = z.ratexp(r2).standard().is_isomorphic(z.ratexp(r1).standard())
        if eff != eff2:
            FAIL('isomorphism check is not commutative on ' + r1 + ' and ' + r2)
    CHECK_EQ(exp, eff)

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

# Non-deterministic automata.
a = '(a+a+a+a)'
b1 = '(a+a+b+a)'
b2 = '(b+a+a+a)'
b3 = '(a+a+a+b)'
check(True, a, a)
check(False, a, b1)
check(False, a, b2)
check(False, a, b3)
check(True, b1, b1)
check(True, b2, b2)
check(True, b3, b3)
check(True, b1, b2)
check(True, b1, b3)
check(True, b2, b3)
