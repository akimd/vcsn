#! /usr/bin/env python

import vcsn
from test import *

def check(i, exp):
  i = vcsn.automaton(i)
  CHECK_EQ(exp, i.insplit())
  # Idempotence.
  CHECK_EQ(exp, i.insplit().insplit())


check('''
digraph
{
  vcsn_context = "lal_char(abc)_b"

  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> 2 [label = "c"]
  2 -> F2
}''','''digraph
{
  vcsn_context = "lal_char(abc)_b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F2
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
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
  vcsn_context = "lan_char(abc)_b"

  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "\\e"]
  1 -> 2 [label = "c"]
  2 -> F2
}''',r'''digraph
{
  vcsn_context = "lan<lal_char(abc)>_b"
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
    0
    1
    2
    3
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
  vcsn_context = "lan_char(abc)_b"

  I0 -> 0
  0 -> 1 [label = "\\e"]
  0 -> 2 [label = "\\e"]
  1 -> 2 [label = "c"]
  2 -> F2
}''', r'''digraph
{
  vcsn_context = "lan<lal_char(abc)>_b"
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
    0
    1
    2
    3
  }
  I0 -> 0
  0 -> 1 [label = "\\e"]
  0 -> 2 [label = "\\e"]
  1 -> 3 [label = "c"]
  2 -> F2
  3 -> F3
}''')
