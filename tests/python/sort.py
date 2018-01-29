#! /usr/bin/env python

import vcsn
from test import *

## check INPUT EXP
## ---------------
def check(input, exp):
  if isinstance(input, str):
    input = vcsn.automaton(input)
  CHECK(not input.is_out_sorted())
  aut = input.sort()
  CHECK_EQ(exp, aut)
  CHECK(aut.is_out_sorted())

a = vcsn.automaton(r'''digraph
{
  vcsn_context = "[abcde]? → 𝔹"
  I -> 0
  2 -> F
  0 -> 1 -> 2 -> 0 [label = b]
  0 -> 2 -> 1 -> 0 [label = a]
}''')

check(a, '''digraph
{
  vcsn_context = "[abcde]? → 𝔹"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F1
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "0", shape = box]
    1 [label = "1", shape = box]
    2 [label = "2", shape = box]
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> F1
  1 -> 0 [label = "b"]
  1 -> 2 [label = "a"]
  2 -> 0 [label = "a"]
  2 -> 1 [label = "b"]
}''')

check(a.transpose(), '''digraph
{
  vcsn_context = "[abcde]? → 𝔹"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F1
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "1", shape = box]
    1 [label = "0", shape = box]
    2 [label = "2", shape = box]
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> F1
  1 -> 0 [label = "b"]
  1 -> 2 [label = "a"]
  2 -> 0 [label = "a"]
  2 -> 1 [label = "b"]
}''')
