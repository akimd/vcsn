#! /usr/bin/env python

import re
import vcsn
from test import *

ctx = vcsn.context('lal_char(abcd), z')
def expr(e, ids='associative'):
    return ctx.expression(e, ids)

# check(INPUT, EXP)
# -----------------
def check(input, exp=None):
    if exp is None:
        exp = input
    ref = expr(exp)
    eff = expr(input).expand()
    CHECK_EQ(ref, eff)
    # Make sure the expression has the right identities.
    CHECK_EQ(eff.identities(), 'associative')

check(r'\z')
check(r'\e')
check('a')
check('<2>a')
check('a+a', '<2>a')
check('b+a+b+a', '<2>a+<2>b')

check('(a*) &  (b+a+b+a)c', '(a*) &  (<2>(ac)+<2>(bc))')
check('(a*) :  (b+a+b+a)c', '(a*) :  (<2>(ac)+<2>(bc))')
check('(a*) &: (b+a+b+a)c', '(a*) &: (<2>(ac)+<2>(bc))')

check('<3>(b+a+b+a)<5>', '<30>a+<30>b')

check('(a+b)?{3}', \
      r'\e+<3>a+<3>b+<3>(aa)+<3>(ab)+<3>(ba)+<3>(bb)' \
      r'+aaa+aab+aba+abb+baa+bab+bba+bbb')

# Tools doc
check(r'(a+b+\e)((a+ba)(ca+cc))*',\
      '(aca+acc+baca+bacc)*+a(aca+acc+baca+bacc)*+b(aca+acc+baca+bacc)*')

check(r'a(b(c+a)*+c(b)*)+ac(\e+b)(b*)', '<2>(acb*)+ab(a+c)*+acbb*')

check('([ab]a){c}', '(aa+ba){c}')
check('([ab]a){T}', '(aa+ba){T}')
check(r'([ab]a){\}(a[ab])', r'(aa+ba){\}(aa+ab)')

ctx = vcsn.context('lat<lan, lan>, q')
check('(a|b)(x|x+y|y)', '(a|b)(x|x) + (a|b)(y|y)')
check('(a|b*)(x|x*+y|y*)', '(a|b*)(x|x*)+(a|b*)(y|y*)')
check('(a|a + b|b)(c|c) @ (a|a)(b|b + c|c)',
      '((a|a)(c|c)+(b|b)(c|c)) @ ((a|a)(b|b)+(a|a)(c|c))')
