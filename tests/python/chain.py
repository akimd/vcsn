#! /usr/bin/env python

import vcsn
from test import *

a = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(a)_b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I
    F1
  }
  {
    node [shape = circle]
    0
    1
  }
  I -> 0
  0 -> 1 [label = "a"]
  1 -> F1
}
''')

CHECK_EQ(a.chain(0), vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(a)_b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F0
  }
  {
    node [shape = circle]
    0
  }
  I0 -> 0
  0 -> F0
}
'''))

CHECK_EQ(a.chain(5), vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(a)_b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F5
  }
  {
    node [shape = circle]
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
}
'''))


a = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab)_b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I
    F1
    F2
  }
  {
    node [shape = circle]
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

CHECK_EQ(a.chain(3), vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab)_b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F5
    F6
  }
  {
    node [shape = circle]
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
}'''))


## --------------------------------- ##
## chain on ratexps vs on automata.  ##
## --------------------------------- ##

ctx = vcsn.context('lal_char(ab)_z')
def check(r, *args):
    "Check that standard and chain commute."
    r = ctx.ratexp(r)
    a = r.standard()
    CHECK_ISOMORPHIC(a.chain(*args), (r ** args).standard())

check('a', 0, 0)
check('a', 0, 1)
check('a', 1, 1)
check('a', 3, 3)
check('a', 0, 3)
check('a', 3, -1)
check('a', -1, -1)
