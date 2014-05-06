#! /usr/bin/env python

import vcsn
from test import *

## check INPUT EXP
## ---------------
def check(input, exp):
  if isinstance(input, str):
    input = vcsn.automaton(input)
  CHECK_EQ(False, input.is_out_sorted())
  if isinstance(exp, str):
    exp = vcsn.automaton(exp)
  CHECK_EQ(True, exp.is_out_sorted())
  CHECK_EQ(exp, input.sort())

check(r'''digraph
{
  vcsn_context = "lal_char(a-e)_b"
  I -> 0
  2 -> F
  0 -> 1 [label = b]
  0 -> 2 [label = a]
  1 -> 2 [label = b]
  1 -> 0 [label = a]
  2 -> 0 [label = b]
  2 -> 1 [label = a]
}'''
, r'''digraph
{
  vcsn_context = "lal_char(abcde)_b"
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
    2
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> F1
  1 -> 2 [label = "a"]
  1 -> 0 [label = "b"]
  2 -> 0 [label = "a"]
  2 -> 1 [label = "b"]
}
''')
