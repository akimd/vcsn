#! /usr/bin/env python

import vcsn
from test import *

z = vcsn.context('lal(abc), z')

# check EXPECTED RAT1 RAT2
# ------------------------
# Check that RAT1.standard().is_isomorphic(RAT2.standard()) == EXPECTED
# and that swapping RAT1 and RAT2 yield the same result.
def check(exp, r1, r2):
    eff = z.expression(r1).standard().is_isomorphic(z.expression(r2).standard())
    if not z.expression(r1).standard().is_isomorphic(z.expression(r1).standard()):
        FAIL('isomorphism check is not reflexive on ' + r1)
    if not z.expression(r2).standard().is_isomorphic(z.expression(r2).standard()):
        FAIL('isomorphism check is not reflexive on ' + r2)
    if r1 != r2:
        eff2 = z.expression(r2).standard().is_isomorphic(z.expression(r1).standard())
        if eff != eff2:
            FAIL('isomorphism check is not commutative on ' + r1 + ' and ' + r2)
    CHECK_EQ(exp, eff)

check(False, r'\z', r'\e')

a = '(ab*c)*'
b = '(c*ba)*'
check(False, a, r'\e')
check(False, b, r'\e')
check(False, a, r'\z')
check(False, b, r'\z')

check(False, a, b)
check(True, a+'+'+b, b+'+'+a)

# Non-deterministic automata.
a = '(a+a+a+a)'
b1 = '(a+a+b+a)'
b2 = '(b+a+a+a)'
b3 = '(a+a+a+b)'
check(False, a, b1)
check(False, a, b2)
check(False, a, b3)
check(True, b1, b2)
check(True, b1, b3)
check(True, b2, b3)
