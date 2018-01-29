#! /usr/bin/env python

import vcsn
from test import *

def check(i, exp=None):
  if not exp:
    exp = i
  i = vcsn.automaton(i)
  CHECK_EQ(exp, i.insplit())
  # Idempotence.
  CHECK_ISOMORPHIC(vcsn.automaton(exp), i.insplit().insplit())


check(r'''digraph
{
  vcsn_context = "[abc]? → ℚ"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F2
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "0, !\\e", shape = box]
    1 [label = "1, !\\e", shape = box]
    2 [label = "2, !\\e", shape = box]
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> 2 [label = "c"]
  2 -> F2
}''')


check(r'''
digraph
{
  vcsn_context = "[abc]? → ℚ"

  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "\\e"]
  1 -> 2 [label = "c"]
  2 -> F2
}''',r'''digraph
{
  vcsn_context = "[abc]? → ℚ"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F2
    F3
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "0, !\\e", shape = box]
    1 [label = "1, !\\e", shape = box]
    2 [label = "2, \\e", shape = box]
    3 [label = "2, !\\e", shape = box]
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "\\e"]
  1 -> 3 [label = "c"]
  2 -> F2
  3 -> F3
}''')

check(r'''
digraph
{
  vcsn_context = "[abc]? → ℚ"

  I0 -> 0
  0 -> 1 [label = "\\e"]
  0 -> 2 [label = "\\e"]
  1 -> 2 [label = "c"]
  2 -> F2
}''', r'''digraph
{
  vcsn_context = "[abc]? → ℚ"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F2
    F3
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "0, !\\e", shape = box]
    1 [label = "1, \\e", shape = box]
    2 [label = "2, \\e", shape = box]
    3 [label = "2, !\\e", shape = box]
  }
  I0 -> 0
  0 -> 1 [label = "\\e"]
  0 -> 2 [label = "\\e"]
  1 -> 3 [label = "c"]
  2 -> F2
  3 -> F3
}''')
