#! /usr/bin/env python

import vcsn
from test import *

a = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(a)_b"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
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
}
'''))

CHECK_EQ(a.chain(5), vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(a)_b"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
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
    node [style = invis, shape = none, label = "", width = 0, height = 0]
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
    node [style = invis, shape = none, label = "", width = 0, height = 0]
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

def check_isomorphic(a1, a2):
    # Does not work with non-deterministic automata.
    # CHECK_EQ(True, a.chain(min, max).is_isomorphic(r.chain(min, max).standard()))
    CHECK_EQ(a1.info(), a2.info())
    CHECK_EQ(a1.shortest(4), a2.shortest(4))

ctx = vcsn.context('lal_char(ab)_z')
def check(r, min, max):
    r = ctx.ratexp(r)
    a = r.standard()
    check_isomorphic(a.chain(min, max), r.chain(min, max).standard())

check('a', 0, 0)
check('a', 0, 1)
check('a', 1, 1)
check('a', 3, 3)
check('a', 0, 3)
check('a', 3, -1)
check('a', -1, -1)
