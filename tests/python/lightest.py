#! /usr/bin/env python

import vcsn
from test import *

algos = ['auto', 'a-star', 'bellman-ford', 'breadth-first', 'dijkstra', 'yen']
k_algos = ['auto', 'breadth-first', 'yen', 'eppstein']
def check(re, num, exp, tests = []):
  a = ctx.expression(re, 'none').standard()
  for algo in tests if tests else algos if num == 1 else k_algos:
      if algo != 'eppstein' or weightset_is(ctx, 'nmin'):
          print(algo + ':' + re)
          p = a.lightest(num=num, algo=algo)
          CHECK_EQ(exp, p)

ctx = vcsn.context('lal_char, nmin')
check(r'\z', 3, r'\z', k_algos)
check(r'\e', 3, r'<0>\e', k_algos)
check('a+b', 2, '<0>a + <0>b', k_algos)
check('ababab', 10, '<0>ababab', k_algos)
check('(<1>a+<1>b)*', 7, r'<0>\e + <1>a + <1>b + <2>aa + <2>ab + <2>ba + <2>bb', ['auto', 'eppstein'])
check('<4>a+(<1>a<1>b)+<1>c+<2>d', 1, '<1>c', k_algos)

ctx = vcsn.context('lal_char(abcd), z')
check('[a-d]?{5}', 5, r'\e + aa + ab + ac + ad', ['auto'])
check('<4>a+(ab)+c+<2>d', 2, 'c + ab')
check('a+<-1>a', 2, r'\z')
check('a+(<2>a<-10>a)', 1, '<-20>aa', ['auto'])

ctx = vcsn.context('lal_char(abcd), q')
check('a+(<2>a<1/10>a)', 1, '<1/5>aa', ['auto'])

ctx = vcsn.context('law_char, nmin')
check('<1>aaaa+<2>b', 1, '<1>aaaa')

ctx = vcsn.context('lal_char, rmin')
check(r'\z', 3, r'\z')
check(r'\e', 3, r'<0>\e')
check('a+b', 2, '<0>a + <0>b')
check('ababab', 10, '<0>ababab')
check('(<1>a+<1>b)*', 7, r'<0>\e + <1>a + <1>b + <2>aa + <2>ab + <2>ba + <2>bb', ['auto'])
check('<4>a+(<1>a<1>b)+<1>c+<2>d', 1, '<1>c')

zero = ctx.expression(r'\z').standard()
for algo in algos:
    if algo not in k_algos:
        XFAIL(lambda: zero.lightest(2, algo),
              "lightest: invalid algorithm: " + algo)
