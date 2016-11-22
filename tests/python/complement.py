#! /usr/bin/env python

import vcsn
from test import *

# Precondition: lal.
XFAIL(lambda: vcsn.automaton('''
digraph
{
  vcsn_context = "law_char(ab), b"
  I -> 0
  0 -> 0 [label="a, b"]
  0 -> F
}
''').complement())

# Precondition: Boolean.
# XFAIL(lambda: vcsn.automaton('''
# digraph
# {
#   vcsn_context = "lal_char(ab), z"
#   I -> 0
#   0 -> 0 [label="a, b"]
#   0 -> F
# }
# ''').complement())

# Precondition: deterministic.
XFAIL(lambda: vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab), b"
  I -> 0
  0 -> 1 [label="a"]
  0 -> 2 [label="a,b"]
  2 -> 2 [label="a,b"]
  2 -> F
}
''').complement())

# Precondition: complete.
XFAIL(lambda: vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab), b"
  I -> 0
  0 -> 0 [label="a"]
  0 -> F
}
''').complement())

# Complement for real.
a = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab), b"
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
    2 [color = DimGray]
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b", color = DimGray]
  1 -> F1
  1 -> 2 [label = "a, b", color = DimGray]
  2 -> 2 [label = "a, b", color = DimGray]
}
''')

o = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab), b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F0
    F2
  }
  {
    node [shape = circle]
    0
    1
    2
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> 2 [label = "a, b"]
  2 -> F2
  2 -> 2 [label = "a, b"]
}
''')

comp = a.complement()
CHECK_EQUIV(o, comp)
# Check cached value.
CHECK_EQ(True, comp.info('is deterministic'))

# Involution.
invol = a.complement().complement()
CHECK_EQUIV(a, invol)
CHECK_EQ(True, invol.info('is deterministic'))
