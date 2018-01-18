#! /usr/bin/env python

import vcsn
from test import *

# check complete algorithm
# ------------------------
def check(i, o):
    if isinstance(i, str):
        i = vcsn.automaton(i)
    CHECK(not i.is_complete())
    o = vcsn.automaton(o)
    CHECK(o.is_complete())
    CHECK_EQ(o, i.complete())
    # Idempotence.
    CHECK_EQ(o, o.complete())


check('''
digraph
{
  vcsn_context = "lal(abcd), b"
  I -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> 2 [label = "c"]
  2 -> F
}
''', '''
digraph
{
  vcsn_context = "lal(abcd), b"
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
  0 -> 3 [label = "c, d", color = DimGray]
  1 -> 2 [label = "c"]
  1 -> 3 [label = "a, b, d", color = DimGray]
  2 -> F2
  2 -> 3 [label = "a, b, c, d", color = DimGray]
  3 -> 3 [label = "a, b, c, d", color = DimGray]
}
''')

# An automaton with an open context.
check(vcsn.b.expression('a').standard(), '''
digraph
{
  vcsn_context = "letterset<char_letters(a)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F1
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2 [color = DimGray]
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  1 -> F1
  1 -> 2 [label = "a", color = DimGray]
  2 -> 2 [label = "a", color = DimGray]
}
''')

# An automaton without initial state.
check('''
digraph
{
  vcsn_context = "lal(a), b"

  0 -> 0 [label = "a"]
  0 -> F0
}
''', '''
digraph
{
  vcsn_context = "lal(a), b"
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
