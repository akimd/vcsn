#! /usr/bin/env python

import vcsn
from test import *

CHECK_EQ(vcsn.automaton(r'''
digraph
{
  vcsn_context = "lal_char(ab)_b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
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
  0 -> 0 [label = "a, b"]
  0 -> 1 [label = "a"]
  1 -> 2 [label = "a, b"]
  2 -> 3 [label = "a, b"]
  3 -> F3
}
'''), vcsn.context('lal_char(ab)_b').de_bruijn(2))

CHECK_EQ(vcsn.automaton(r'''
digraph
{
  vcsn_context = "lal_char(xyz)_b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F4
  }
  {
    node [shape = circle]
    0
    1
    2
    3
    4
  }
  I0 -> 0
  0 -> 0 [label = "[x-z]"]
  0 -> 1 [label = "x"]
  1 -> 2 [label = "[x-z]"]
  2 -> 3 [label = "[x-z]"]
  3 -> 4 [label = "[x-z]"]
  4 -> F4
}
'''), vcsn.context('lal_char(xyz)_b').de_bruijn(3))
