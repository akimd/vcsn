#! /usr/bin/env python

import vcsn
from test import *

# Check both syntaxes: `aut.multiply(n)` and `aut ** n`.
def check(aut, n, exp):
    CHECK_EQ(exp, aut ** n)
    CHECK_EQ(exp, aut.multiply(n))

a = vcsn.B.expression('a').standard()
check(a, 0, vcsn.B.expression(r'\e').standard())
check(a, 5, '''digraph
{
  vcsn_context = "letterset<char_letters(a)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F5
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
    3
    4
    5
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  1 -> 2 [label = "a"]
  2 -> 3 [label = "a"]
  3 -> 4 [label = "a"]
  4 -> 5 [label = "a"]
  5 -> F5
}''')


a = vcsn.automaton('''
digraph
{
  vcsn_context = "letterset<char_letters(ab)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I
    F1
    F2
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
  }
  I -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  2 -> F2
  1 -> F1
}
''')

check(a, 3, '''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F5
    F6
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
    3
    4
    5
    6
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> 3 [label = "a"]
  1 -> 4 [label = "b"]
  2 -> 3 [label = "a"]
  2 -> 4 [label = "b"]
  3 -> 5 [label = "a"]
  3 -> 6 [label = "b"]
  4 -> 5 [label = "a"]
  4 -> 6 [label = "b"]
  5 -> F5
  6 -> F6
}''')


## ------------------------------------------------- ##
## Repeated multiply on expressions vs on automata.  ##
## ------------------------------------------------- ##

ctx = vcsn.context('lal_char(ab), z')
r = ctx.expression('a')
a = r.standard()
def check(*args):
    "Check that standard and multiply commute."
    CHECK_ISOMORPHIC(a ** args, (r ** args).standard())

check(0)
check(0, 1)
check(1)
check(3)
check(0, 3)
check(3, -1)
check(-1)
XFAIL(lambda: a ** (2, 1))
