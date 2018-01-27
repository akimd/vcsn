#! /usr/bin/env python

import vcsn
from test import *

g = vcsn.automaton('''digraph
{
  vcsn_context = "lal(ab), b"
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
}''')

## ---------------- ##
## automaton.pair.  ##
## ---------------- ##

CHECK_EQ('''digraph
{
  vcsn_context = "[ab]? → 𝔹"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [color = DimGray]
    1 [label = "0, 1", shape = box, color = DimGray]
    2 [label = "0, 2", shape = box, color = DimGray]
    3 [label = "0, 3", shape = box, color = DimGray]
    4 [label = "1, 2", shape = box, color = DimGray]
    5 [label = "1, 3", shape = box, color = DimGray]
    6 [label = "2, 3", shape = box, color = DimGray]
  }
  0 -> 0 [label = "a, b", color = DimGray]
  1 -> 0 [label = "a", color = DimGray]
  1 -> 4 [label = "b", color = DimGray]
  2 -> 4 [label = "a", color = DimGray]
  2 -> 5 [label = "b", color = DimGray]
  3 -> 1 [label = "b", color = DimGray]
  3 -> 5 [label = "a", color = DimGray]
  4 -> 4 [label = "a", color = DimGray]
  4 -> 6 [label = "b", color = DimGray]
  5 -> 2 [label = "b", color = DimGray]
  5 -> 5 [label = "a", color = DimGray]
  6 -> 3 [label = "b", color = DimGray]
  6 -> 6 [label = "a", color = DimGray]
}''',
         str(g.pair()))

## ------------------------------ ##
## automaton.is_synchronized_by.  ##
## ------------------------------ ##

not_synchronizing = vcsn.automaton('''
digraph
{
  vcsn_context = "lal(ab), b"
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
}''')

CHECK_EQ(not_synchronizing.is_synchronizing(), False)
CHECK_EQ(g.is_synchronizing(), True)
CHECK_EQ(g.is_synchronized_by('abbababbba'), True)
CHECK_EQ(g.is_synchronized_by('abbbabbba'), True)
CHECK_EQ(g.is_synchronized_by(g.synchronizing_word('greedy')), True)
CHECK_EQ(g.is_synchronized_by(g.synchronizing_word('cycle')), True)
CHECK_EQ(g.is_synchronized_by(g.synchronizing_word('synchrop')), True)
CHECK_EQ(g.is_synchronized_by(g.synchronizing_word('synchropl')), True)
CHECK_EQ(g.is_synchronized_by(g.synchronizing_word('fastsynchro')), True)

b = vcsn.context('lal(ab), b')

for i in [3, 5, 7]:
    db = b.de_bruijn(i).determinize()
    CHECK_EQ(db.is_synchronized_by(db.synchronizing_word()), True)

not_deterministic = vcsn.automaton('''
digraph
{
  vcsn_context = "lal(ab), b"
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


synchro_regression = vcsn.automaton('''
digraph
{
  vcsn_context = "lal(ab), b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F3
  }
  {
    node [shape = circle]
    0 [color = DimGray]
    1 [color = DimGray]
    2 [color = DimGray]
    3 [color = DimGray]
    4 [color = DimGray]
    5 [color = DimGray]
  }
  I0 -> 0 [color = DimGray]
  0 -> 1 [label = "b", color = DimGray]
  0 -> 2 [label = "a", color = DimGray]
  1 -> 1 [label = "a", color = DimGray]
  1 -> 4 [label = "b", color = DimGray]
  2 -> 2 [label = "b", color = DimGray]
  2 -> 5 [label = "a", color = DimGray]
  3 -> F3 [color = DimGray]
  3 -> 4 [label = "a, b", color = DimGray]
  4 -> 0 [label = "a", color = DimGray]
  4 -> 2 [label = "b", color = DimGray]
  5 -> 1 [label = "b", color = DimGray]
  5 -> 2 [label = "a", color = DimGray]
}
''')

CHECK_EQ(str(synchro_regression.synchronizing_word('greedy')), 'bbb')
CHECK_EQ(str(synchro_regression.synchronizing_word('synchrop')), 'bbb')
CHECK_EQ(str(synchro_regression.synchronizing_word('synchropl')), 'bbb')
CHECK_EQ(str(synchro_regression.synchronizing_word('fastsynchro')), 'bbb')

# Check that the minimum length of cerny's automata match the cerny's
# conjecture.

for i in (10, 20, 30, 42):
    CHECK_EQ(len(str(b.cerny(i).synchronizing_word('cycle'))), (i - 1) ** 2)
