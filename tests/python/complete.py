#! /usr/bin/env python

import vcsn
from test import *

# check complete algorithm
# ------------------------
def check(i, o):
  i = vcsn.automaton(i)
  o = vcsn.automaton(o)
  CHECK_EQ(o, i.complete())
  # Idempotence.
  CHECK_EQ(o, o.complete())

check('''
digraph
{
  vcsn_context = "lal_char(abc)_b"

  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> 2 [label = "c"]
  2 -> F2
}
''', '''
digraph
{
  vcsn_context = "lal_char(abc)_b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F2
  }
  {
    node [shape = circle]
    0
    1
    2
    3 [color = DimGray]
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  0 -> 3 [label = "c", color = DimGray]
  1 -> 2 [label = "c"]
  1 -> 3 [label = "a, b", color = DimGray]
  2 -> F2
  2 -> 3 [label = "a, b, c", color = DimGray]
  3 -> 3 [label = "a, b, c", color = DimGray]
}
''')

check('''
digraph
{
  vcsn_context = "lal_char(a)_b"

  I0 -> 0
  0 -> 1 [label = "a"]
  1 -> 1 [label = "a"]
  1 -> F1
}
''', '''
digraph
{
  vcsn_context = "lal_char(a)_b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F1
  }
  {
    node [shape = circle]
    0
    1
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  1 -> F1
  1 -> 1 [label = "a"]
}
''')


check('''
digraph
{
  vcsn_context = "lal_char(a)_b"

  0 -> 0 [label = "a"]
  0 -> F0
}
''', '''
digraph
{
  vcsn_context = "lal_char(a)_b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I1
    F0
  }
  {
    node [shape = circle]
    0 [color = DimGray]
    1 [color = DimGray]
  }
  I1 -> 1 [color = DimGray]
  0 -> F0 [color = DimGray]
  0 -> 0 [label = "a", color = DimGray]
  1 -> 1 [label = "a", color = DimGray]
}
''')
