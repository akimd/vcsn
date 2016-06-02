#! /usr/bin/env python

import vcsn
from test import *

# Use a context with expression weights to check the order of
# products.
ctx = vcsn.context("lal_char(abc), seriesset<lal_char(xyz), q>")

# check WEIGHT EXP
# ----------------
# Check that the constant-term of EXP is WEIGHT, and check that this
# is indeed the evaluation of the empty word on derived-term(EXP).
def check(weight, exp, algo='expansion'):
    w = ctx.weight(weight)
    re = ctx.expression(exp)
    CHECK_EQ(w, re.constant_term())
    CHECK_EQ(w, re.automaton(algo).eval(''))

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

# transposition.
check('zyx', '(<xyz>\e){T}')

# ldiv
e = ctx.expression('<x>a{\}<x>a')
XFAIL(lambda: e.constant_term(),
      "constant_term: ldiv is not supported")

# tuple.
#
# We have to request for construction based on the derivation, not the
# expansion, becase expansion wants the label one, i.e., it requires
# the labelsets to be lan, not lal.
ctx = vcsn.context('lat<lal<char>, lal<char>>, q')
check('0', ' (<2>a)|(<3>x)',  'derivation')
check('0', ' (<2>a)|(<3>\e)', 'derivation')
check('0', '(<2>\e)|(<3>x)',  'derivation')
check('6', '(<2>\e)|(<3>\e)', 'derivation')

# compose
e = ctx.expression('\e|a @ a|\e')
XFAIL(lambda: e.constant_term(),
      "constant_term: compose is not supported")
