#! /usr/bin/env python

import vcsn
from test import *

g = vcsn.automaton('''digraph
{
  vcsn_context = "lal_char(ab)_b"
  rankdir = LR
  {
    node [shape = circle]
    0 [color = DimGray]
    1 [color = DimGray]
    2 [color = DimGray]
    3 [color = DimGray]
  }
  0 -> 1 [label="a"]
  1 -> 1 [label="a"]
  2 -> 2 [label="a"]
  3 -> 3 [label="a"]

  0 -> 1 [label="b"]
  1 -> 2 [label="b"]
  2 -> 3 [label="b"]
  3 -> 0 [label="b"]
}
''')

## ---------------- ##
## automaton.pair.  ##
## ---------------- ##

CHECK_EQ(vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab)_b"
  rankdir = LR
  {
    node [shape = circle]
    0 [color = DimGray]
    1 [color = DimGray]
    2 [color = DimGray]
    3 [color = DimGray]
    4 [color = DimGray]
    5 [color = DimGray]
    6 [color = DimGray]
  }
  0 -> 0 [label = "a, b"]
  1 -> 0 [label = "a"]
  1 -> 4 [label = "b"]
  2 -> 4 [label = "a"]
  2 -> 5 [label = "b"]
  3 -> 1 [label = "b"]
  3 -> 5 [label = "a"]
  4 -> 4 [label = "a"]
  4 -> 6 [label = "b"]
  5 -> 2 [label = "b"]
  5 -> 5 [label = "a"]
  6 -> 3 [label = "b"]
  6 -> 6 [label = "a"]
}
'''),
         g.pair())

## ------------------------------ ##
## automaton.is_synchronized_by.  ##
## ------------------------------ ##

not_synchronizing = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab)_b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I2
    F2
  }
  {
    node [shape = circle]
    0 [color = DimGray]
    1
    2
  }
  I2 -> 2
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> 1 [label = "b"]
  1 -> 2 [label = "a"]
  2 -> F2
  2 -> 1 [label = "a"]
  2 -> 2 [label = "b"]
}
''')

CHECK_EQ(not_synchronizing.is_synchronizing(), False)
CHECK_EQ(g.is_synchronizing(), True)
CHECK_EQ(g.is_synchronized_by('abbababbba'), True)
CHECK_EQ(g.is_synchronized_by('abbbabbba'), True)
CHECK_EQ(g.is_synchronized_by(g.synchronizing_word('greedy')), True)
CHECK_EQ(g.is_synchronized_by(g.synchronizing_word('synchrop')), True)
CHECK_EQ(g.is_synchronized_by(g.synchronizing_word('synchropl')), True)

b = vcsn.context('lal_char(ab)_b')

for i in [3, 5, 7]:
    db = b.de_bruijn(i).determinize()
    CHECK_EQ(db.is_synchronized_by(db.synchronizing_word()), True)

not_deterministic = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab)_b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I2
    F2
  }
  {
    node [shape = circle]
    0 [color = DimGray]
    1
    2
  }
  I2 -> 2
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  0 -> 1 [label = "b"]
  1 -> 2 [label = "a"]
  2 -> F2
  2 -> 1 [label = "a"]
  2 -> 2 [label = "b"]
}''')

try:
    not_deterministic.synchronizing_word()
except RuntimeError:
    PASS()
else:
    FAIL('did not raise an exception: automaton is not deterministic')

try:
    not_synchronizing.synchronizing_word()
except RuntimeError:
    PASS()
else:
    FAIL('should raise an exception: automaton is not synchronizing')
