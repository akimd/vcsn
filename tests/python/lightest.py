#! /usr/bin/env python

import vcsn
from test import *

# The algos that can compute the (1) lightest path.
algos = ['auto', 'a-star', 'bellman-ford', 'breadth-first', 'dijkstra', 'yen']

# The algos that can compute the k lightest paths.
k_algos = ['auto', 'breadth-first', 'yen', 'eppstein']

def check_aut(aut, re, num, exp, tests = []):
  for algo in tests if tests else algos if num == 1 else k_algos:
      if algo != 'eppstein' or weightset_of(ctx) in ['nmin', 'rmin', 'zmin']:
          print(algo + ':' + re)
          p = aut.lightest(num=num, algo=algo)
          CHECK_EQ(exp, p)

def check(re, num, exp, tests = []):
  check_aut(ctx.expression(re, 'none').standard(), re, num, exp, tests)

ctx = vcsn.context('[...] -> nmin')
check(r'\z', 3, r'\z', k_algos)
check(r'\e', 3, r'<0>\e', k_algos)
check('a+b', 2, '<0>a + <0>b', k_algos)
check('ababab', 10, '<0>ababab', k_algos)
check('(<1>a+<1>b)*', 7, r'<0>\e + <1>a + <1>b + <2>aa + <2>ab + <2>ba + <2>bb', ['auto', 'eppstein'])
check('<4>a+(<1>a<1>b)+<1>c+<2>d', 1, '<1>c', k_algos)

aut = vcsn.automaton('''
context = [...]? → ℕmin
$ -> 0
0 -> 1 <6>a
0 -> 2 <1>a
2 -> 3 <1>b
3 -> 3 <2>b
3 -> 4 <1>c
4 -> 1 <1>d
0 -> 5 <2>a
5 -> 1 <3>b
1 -> $
''')

check_aut(aut, "notebook example", 5,
          '<6>a + <5>ab + <4>abcd + <6>abbcd + <8>abbbcd', ['auto', 'eppstein'])

ctx = vcsn.context('[abcd] -> z')
check('[a-d]?{5}', 5, r'\e + aa + ab + ac + ad', ['auto'])
check('<4>a+(ab)+c+<2>d', 2, 'c + ab')
check('a+<-1>a', 2, r'\z')
check('a+(<2>a<-10>a)', 1, '<-20>aa', ['auto'])

ctx = vcsn.context('[abcd] -> q')
check('a+(<2>a<1/10>a)', 1, '<1/5>aa', ['auto'])

ctx = vcsn.context('[...]* -> Nmin')
check('<1>aaaa+<2>b', 1, '<1>aaaa')

ctx = vcsn.context('[...] -> rmin')
check(r'\z', 3, r'\z')
check(r'\e', 3, r'<0>\e')
check('a+b', 2, '<0>a + <0>b')
check('ababab', 10, '<0>ababab')
check('(<1>a+<1>b)*', 7, r'<0>\e + <1>a + <1>b + <2>aa + <2>ab + <2>ba + <2>bb', ['auto', 'eppstein'])
check('<4>a+(<1>a<1>b)+<1>c+<2>d', 1, '<1>c')

zero = ctx.expression(r'\z').standard()
for algo in algos:
    if algo not in k_algos:
        XFAIL(lambda: zero.lightest(2, algo),
              "lightest: invalid algorithm: " + algo)
