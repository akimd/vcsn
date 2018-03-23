#! /usr/bin/env python

import vcsn
from test import *

# Use a context with expression weights to check the order of
# products.
ctx = vcsn.context("[abc] -> seriesset<[xyz] -> q>")

# check WEIGHT EXP
# ----------------
# Check that the constant-term of EXP is WEIGHT, and check that this
# is indeed the evaluation of the empty word on derived-term(EXP).
def check(weight, exp, algo='expansion'):
    w = ctx.weight(weight)
    re = ctx.expression(exp)
    CHECK_EQ(w, re.constant_term())
    CHECK_EQ(w, re.automaton(algo).evaluate(''))

# zero.
check(r'\z', r'<x>\z')

# one.
check('x', r'<x>\e')

# label.
check(r'\z', 'a')

# add.
check('x+y', '<x>a*+<y>b*')
check('x(y+y)z', '<x>(<y>a*+<y>b*)<z>')

# left and right weight.
check('xy*z', r'<x>(<y>\e)*<z>')

# mul.
check('xy', '<x>a*<y>b*')

# conjunction.
check(r'\z', '<x>a&<y>b')
check('xy', '<x>a*&<y>b*')

# shuffle.
check(r'\z', '<x>a:<y>b')
check('xy', '<x>a*:<y>b*')

# infiltrate.
check(r'\z', '<x>a&:<y>b')
check('xy', '<x>a*&:<y>b*')

# star.
check(r'\e', '(<x>a)*')
check('x*', r'(<x>\e)*')
XFAIL(lambda: vcsn.Q.expression('a**').constant_term(),
      r'''Q: value is not starrable: 1
  while computing constant-term of: a**''')

# complement.
check(r'\e', '(<x>a){c}')
check('x*', r'(<x>\e)*')

# transposition.
check('zyx', r'(<xyz>\e){T}')

# ldivide
e = ctx.expression(r'<x>a{\}<x>a')
XFAIL(lambda: e.constant_term(),
      r'''constant_term: operator ldivide not supported: <x>a{\}<x>a
  while computing constant-term of: <x>a{\}<x>a''')

# tuple.
ctx = vcsn.context('lat<lal<char>, lal<char>>, q')
check('0',  ' (<2>a)|(<3>x)',  'derivation')
check('0', r' (<2>a)|(<3>\e)', 'derivation')
check('0', r'(<2>\e)|(<3>x)',  'derivation')
check('6', r'(<2>\e)|(<3>\e)', 'derivation')

# compose
e = ctx.expression(r'\e|a @ a|\e', 'none')
XFAIL(lambda: e.constant_term(),
      r'''operator compose not supported: \e|a@a|\e
  while computing constant-term of: \e|a@a|\e''')
