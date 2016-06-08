#! /usr/bin/env python

import vcsn
from test import *

algos = ['a-star', 'bellman-ford', 'dijkstra']

def check(re, weight, exp):
  wexp = '<{w}>{e}'.format(w=weight, e=exp) if exp != r'\z' else r'\z'
  aut = ctx.expression(re).automaton()
  ref = ctx.expression(wexp).standard().trim().strip()
  for algo in algos:
      res = aut.lightest_automaton(1, algo)
      CHECK_ISOMORPHIC(ref, res)
      CHECK_EQ(weight, res.weight_series())

ctx = vcsn.context('lal_char, nmin')
check(r'\z', 'oo', r'\z')
check(r'\e', '0', r'\e')
check('<1>a+<2>b', '1', 'a')
check('<1>b+<1>a', '1', 'a')
check('aaa+<2>b', '0', 'aaa')
check('a(<1>b)*', '0', 'a')
check('[ab]', '0', 'a')

ctx = vcsn.context('lal_char, q')
for algo in algos:
    XFAIL(lambda: ctx.expression(r'\z').standard().lightest_automaton(1, algo))
