#! /usr/bin/env python

import vcsn
from test import *

z = vcsn.context('lal_char(abcd), z')

# check(INPUT, EXP)
# -----------------
def check(input, exp):
    eff = z.ratexp(input).expand()
    exp = z.ratexp(exp)
    if eff == exp:
        PASS()
    else:
        FAIL(str(exp) + " != " + str(eff))

check('\z', '\z')
check('\e', '\e')
check('a',    'a')
check('<2>a',  '<2>a')
check('a+a',  '<2>a')
check('a+a',  '<2>a')
check('b+a+b+a',  '<2>a+<2>b')

check('(a*)&(b+a+b+a)',  '<2>(a*&a)+<2>(a*&b)')

check('<3>(b+a+b+a)<5>',  '<30>a+<30>b')

check('(a+b)?{3}', \
      '\e+<3>a+<3>b+<3>(aa)+<3>(ab)+<3>(ba)+<3>(bb)' \
      + '+aaa+aab+aba+abb+baa+bab+bba+bbb')

# TAF-Kit doc
check('(a+b+\e)((a+ba)(ca+cc))*',\
      '(aca+acc+baca+bacc)*+a(aca+acc+baca+bacc)*+b(aca+acc+baca+bacc)*')

check('a(b(c+a)*+c(b)*)+ac(\e+b)(b*)', '<2>(acb*)+ab(a+c)*+acbb*')
