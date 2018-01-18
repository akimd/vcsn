#! /usr/bin/env python

import vcsn
from test import *

## ------------------------------------ ##
## Taken from TAFK, command universal.  ##
## ------------------------------------ ##
exp = vcsn.automaton(r'''
digraph
{
  vcsn_context = "lal(ab), b"
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
  }
  I0 -> 0
  0 -> 0 [label = "a, b"]
  0 -> 1 [label = "a"]
  1 -> 0 [label = "a, b"]
  1 -> 1 [label = "a, b"]
  1 -> 2 [label = "b"]
  2 -> F2
  2 -> 0 [label = "a, b"]
  2 -> 1 [label = "a, b"]
  2 -> 2 [label = "a, b"]
}
''')
a1 = load('lal_char_b/a1.gv')
CHECK_EQ(exp, a1.universal())
CHECK_EQ(exp, exp.universal())


## ----------------------------------------------------------- ##
## Taken from vaucanson-1.4.1/src/demos/algorithms/universal.  ##
## ----------------------------------------------------------- ##

# With p, q, r -> 0, 1, 2
a = vcsn.automaton(r'''
digraph
{
  vcsn_context = "lal(abc), b"

  I -> 0
  0 -> 1 [label = a]
  0 -> 1 [label = c]
  0 -> 2 [label = b]
  1 -> 0 [label = b]
  1 -> 0 [label = c]
  1 -> 2 [label = a]
  2 -> 2 [label = b]
  2 -> 0 [label = a]
  2 -> 0 [label = c]
  2 -> F
}
''')
# This output was checked against Vaucanson 1's result using
# are-equivalent.
exp = vcsn.automaton(r'''
digraph
{
  vcsn_context = "lal(abc), b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    I1
    I2
    I3
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
  I1 -> 1
  I2 -> 2
  I3 -> 3
  0 -> 1 [label = "a, c"]
  0 -> 2 [label = "[a-c]"]
  0 -> 3 [label = "b"]
  0 -> 4 [label = "a, c"]
  0 -> 5 [label = "[a-c]"]
  0 -> 6 [label = "b"]
  1 -> 1 [label = "c"]
  1 -> 2 [label = "[a-c]"]
  1 -> 3 [label = "b"]
  1 -> 5 [label = "a"]
  2 -> 1 [label = "c"]
  2 -> 2 [label = "[a-c]"]
  2 -> 3 [label = "b"]
  3 -> 1 [label = "a, c"]
  3 -> 2 [label = "[a-c]"]
  3 -> 3 [label = "b"]
  3 -> 5 [label = "b"]
  3 -> 6 [label = "b"]
  4 -> 0 [label = "b, c"]
  4 -> 1 [label = "b, c"]
  4 -> 2 [label = "[a-c]"]
  4 -> 3 [label = "[a-c]"]
  4 -> 5 [label = "a"]
  4 -> 6 [label = "a"]
  5 -> 0 [label = "c"]
  5 -> 1 [label = "c"]
  5 -> 2 [label = "[a-c]"]
  5 -> 3 [label = "[a-c]"]
  6 -> F6
  6 -> 0 [label = "a, c"]
  6 -> 1 [label = "a, c"]
  6 -> 2 [label = "[a-c]"]
  6 -> 3 [label = "[a-c]"]
  6 -> 5 [label = "b"]
  6 -> 6 [label = "b"]
}
''')
CHECK_EQ(exp, a.universal())
# Idempotence.
CHECK_EQ(exp, exp.universal())
