#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context("lal_char(abc), seriesset<lal_char(xyz), q>")

# check WEIGHT RAT-EXP
# --------------------
# Check that the constant-term of RAT-EXP is WEIGHT, and check that
# this is indeed the evaluation of the empty word on derived-term(RAT-EXP).
#
# Use a context with expression weights to check the order of products.
def check(weight, exp):
    w = ctx.weight(weight)
    re = ctx.expression(exp)
    CHECK_EQ(w, re.constant_term())
    CHECK_EQ(w, re.automaton().eval(''))

# zero.
check('\z', '<x>\z')

# one.
check('x', '<x>\e')

# label.
check('\z', 'a')

# add.
check('x+y', '<x>a*+<y>b*')
check('x(y+y)z', '<x>(<y>a*+<y>b*)<z>')

# left and right weight.
check('xy*z', '<x>(<y>\e)*<z>')

# mul.
check('xy', '<x>a*<y>b*')

# conjunction.
check('\z', '<x>a&<y>b')
check('xy', '<x>a*&<y>b*')

# shuffle.
check('\z', '<x>a:<y>b')
check('xy', '<x>a*:<y>b*')

# infiltration.
check('\z', '<x>a&:<y>b')
check('xy', '<x>a*&:<y>b*')

# star.
check('\e', '(<x>a)*')
check('x*', '(<x>\e)*')

# complement.
check('\e', '(<x>a){c}')
check('x*', '(<x>\e)*')

