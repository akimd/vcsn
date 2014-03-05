#! /usr/bin/env python

import vcsn
from test import *

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
