#! /usr/bin/env python

import vcsn
from test import *

def check(re, num, *exp):
  r = ctx.expression(re)
  p = r.standard().lightest(num = num)
  exp = " + ".join(exp)
  CHECK_EQ(exp, p)

ctx = vcsn.context('lal_char, nmin')
check('\z', 3, '\z')
check('\e', 3, '<0>\e')
check('a+b', 2, '<0>a + <0>b')
check('ababab', 10, '<0>ababab')
check('(<1>a+<1>b)*', 7, '<0>\e + <1>a + <1>b + <2>aa + <2>ab + <2>ba + <2>bb')
check('<4>a+(<1>a<1>b)+<1>c+<2>d', 1, '<1>c')

ctx = vcsn.context('lal_char(abcd), z')
check('[a-d]?{5}', 5, '\e + <5>a + <5>b + <5>c + <5>d')
check('<4>a+(ab)+c+<2>d', 2, 'c + ab')
check('a+<-1>a', 2, '\z')

ctx = vcsn.context('law_char, nmin')
check('<1>aaaa+<2>b', 1, '<1>aaaa')
