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
