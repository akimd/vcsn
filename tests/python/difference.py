#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context('lal_char(abcd)_b')

# check LHS RHS EXPECTED
# ----------------------
# Check that to-expression(difference(dterm(LHS), dterm(RHS))) == EXPECTED.
def check(lhs, rhs, exp):
    l = ctx.ratexp(lhs).derived_term()
    r = ctx.ratexp(rhs).derived_term()
    CHECK_EQ(exp, str((l-r).ratexp()))

check('(a+b)*', 'b*',            'b*a(a+b)*')
check('(a+b)*', '(a+b)*b(a+b)*', 'a*')
check('(a+b)*', '(a+b)*',        '\z')

check('a(ba)*', '(ab)*a', '\z')

check('(?@lal_char(ab)_z)(<2>a+<3>b)*', '(a+b)*a(a+b)*', '(<3>b)*')
