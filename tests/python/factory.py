#! /usr/bin/env python

# Check the factories (of automata, ratexps, etc.)

import vcsn
from test import *

## ----------- ##
## divkbaseb.  ##
## ----------- ##

b = vcsn.context('lal_char(0-9)_b')

XFAIL(lambda: b.divkbaseb(0, 2))
XFAIL(lambda: b.divkbaseb(2, 0))
XFAIL(lambda: b.divkbaseb(2, 1))
XFAIL(lambda: b.divkbaseb(2, 11))

# FIXME: we don't parse polynomials yet.
CHECK_EQ(r'\e + 0 + 00 + 10 + 000 + 010 + 100 + 110 + 0000 + 0010',
         str(b.divkbaseb(2,2).shortest(10)))
CHECK_EQ(r'\e + 0 + 00 + 10 + 20 + 30 + 40 + 50 + 60 + 70',
         str(b.divkbaseb(10,10).shortest(10)))
CHECK_EQ(r'\e + 0 + 5 + 00 + 05 + 10 + 15 + 20 + 25 + 30',
         str(b.divkbaseb(5,10).shortest(10)))
CHECK_EQ(r'\e + 0 + 3 + 6 + 9 + 00 + 03 + 06 + 09 + 12',
         str(b.divkbaseb(3,10).shortest(10)))


## ------------- ##
## double_ring.  ##
## ------------- ##

ctx = vcsn.context('lal_char(abcd)_b')
CHECK_EQ(ctx.double_ring(0, []),
vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abcd)_b"
  rankdir = LR
}
'''))

CHECK_EQ(ctx.double_ring(1, [0]),
vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abcd)_b"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F0
  }
  {
    node [shape = circle]
    0
  }
  I0 -> 0
  0 -> F0
  0 -> 0 [label = "a, b"]
}
'''))

CHECK_EQ(ctx.double_ring(4, [2, 3]),
vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abcd)_b"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F2
    F3
  }
  {
    node [shape = circle]
    0
    1
    2
    3
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 3 [label = "b"]
  1 -> 0 [label = "b"]
  1 -> 2 [label = "a"]
  2 -> F2
  2 -> 1 [label = "b"]
  2 -> 3 [label = "a"]
  3 -> F3
  3 -> 0 [label = "a"]
  3 -> 2 [label = "b"]
}
'''))

## -------- ##
## random.  ##
## -------- ##

# Expect a clique.
c1 = vcsn.context('lal_char(a)_b').random(4, 1, 4, 4)
c2 = vcsn.automaton.load(medir + '/clique-a-4.gv')
CHECK_EQ(c1, c2)

# Expect the right number of states.
a = vcsn.context('lal_char(a)_b').random(100, .1, 20, 30)
CHECK_EQ(a.info()['type'], 'mutable_automaton<lal_char(a)_b>')
CHECK_EQ(a.info()['number of states'], 100)
CHECK_EQ(a.info()['number of initial states'], 20)
CHECK_EQ(a.info()['number of final states'], 30)


## --- ##
## u.  ##
## --- ##

CHECK_EQ(vcsn.automaton.load(medir + '/u-5.gv'),
         vcsn.context('lal_char(abc)_b').u(5))
