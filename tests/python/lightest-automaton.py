#! /usr/bin/env python

import vcsn
from test import *

algos = ['bellman-ford', 'dijkstra']

def check(re, exp):
  r = ctx.expression(re)
  p = r.standard()
  for algo in algos:
      res = p.lightest_automaton(algo)
      CHECK_EQ(exp, res.lightest())

ctx = vcsn.context('lal_char, nmin')
check('\z', '\z')
check('\e', '<0>\e')
check('a+<2>b', '<0>a')
check('aaa+<2>b', '<0>aaa')

ctx = vcsn.context('lal_char, q')
for algo in algos:
    XFAIL(lambda: ctx.expression('\z').standard().lightest_automaton(algo))
