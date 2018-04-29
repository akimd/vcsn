#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context('[abcd] -> b')

# check LHS RHS EXPECTED
# ----------------------
# Check that to-expression(difference(dterm(LHS), dterm(RHS))) == EXPECTED.


def check(lhs, rhs, exp):
    l = ctx.expression(lhs).derived_term()
    r = ctx.expression(rhs).derived_term()
    CHECK_EQ(exp, str((l - r).expression()))

check('(a+b)*', 'b*',            'b*a(a+b)*')
check('(a+b)*', '(a+b)*b(a+b)*', 'a*')
check('(a+b)*', '(a+b)*',        r'∅')

check('a(ba)*', '(ab)*a', r'∅')

check('(?@[ab] -> z)(<2>a+<3>b)*', '(a+b)*a(a+b)*', '(⟨3⟩b)*')
