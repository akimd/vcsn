#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context("lal_char(abc)_ratexpset<lal_char(xyz)_z>")

# check WEIGHT RAT-EXP
# --------------------
# Check that the constant-term of RAT-EXP is WEIGHT, and check that
# this is indeed the evaluation of the empty word on derived-term(RAT-EXP).
#
# Use a context with ratexp weights to check the order of products.
def check(weight, exp):
    w = ctx.weight(weight)
    re = ctx.ratexp(exp)
    CHECK_EQ(w, re.constant_term())
    CHECK_EQ(w, re.derived_term().eval(''))

check('\z', 'a')
check('\e', 'a*')
check('x', '<x>a*')

# Test zero.
check('\z', '<x>\z')

# Test one.
check('x', '<x>\e')

# Test add.
check('x+y', '<x>a*+<y>b*')
check('x(y+y)z', '<x>(<y>a*+<y>b*)<z>')

# Test mul.
check('xy', '<x>a*<y>b*')

# Test conjunction.
check('\z', '<x>a&<y>b')
check('xy', '<x>a*&<y>b*')

# Test star.
check('\e', '(<x>a)*')
check('x*', '(<x>\e)*')

# Test left and right weight.
check('xy*z', '<x>(<y>\e)*<z>')
