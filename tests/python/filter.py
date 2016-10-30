#! /usr/bin/env python

import vcsn
from test import *

def check(in_aut, ss, out_aut):
    faut = in_aut.filter(ss)
    CHECK_EQ(out_aut, faut)

# Check empty automaton.
in_aut = vcsn.automaton('''digraph {
  vcsn_context = "lal_char(abc), b"
}''')

out_aut = vcsn.automaton('''digraph
{
  vcsn_context = "lal_char(abc), b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
}''')

check(in_aut, [], out_aut)

check(in_aut, [0, 1], out_aut)

# Check automaton has a state.
in_aut = vcsn.automaton('''digraph {
  vcsn_context = "lal_char(abc), b"
  I0 -> 0
}''')

check(in_aut, [], out_aut)
check(in_aut, [1, 2], out_aut)

out_aut = vcsn.automaton('''digraph
{
  vcsn_context = "lal_char(abc), b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "0", shape = box, color = DimGray]
  }
  I0 -> 0 [color = DimGray]
}''')

check(in_aut, [0], out_aut)
check(in_aut, [0, 2], out_aut)

# Automaton with several states.
in_aut = vcsn.automaton('''digraph
{
  vcsn_context = "lal_char(abc), b"
  0 -> 1 [label = a]
  1 -> 0 [label = b]
  0 -> 4 [label = c]
  1 -> F1
  1 -> 2 [label = a]
  I0 -> 0
  3 -> 4 [label = c]
  4 -> 0 [label = b]
  4 -> 5 [label = a]
}''')

out_aut = vcsn.automaton('''digraph
{
  vcsn_context = "lal_char(abc), b"
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
    3 [label = "3", shape = box, color = DimGray]
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "c"]
  1 -> F1
  1 -> 0 [label = "b"]
  1 -> 3 [label = "a", color = DimGray]
  2 -> 0 [label = "b"]
}''')

check(in_aut, [0, 1, 2, 3], out_aut)
check(in_aut, [0, 1, 2, 3, 7], out_aut)

check(in_aut.filter([0, 1, 2, 3, 4, 5]), [0, 1, 2, 3], out_aut)
check(in_aut.filter([0, 1, 2, 3, 4]), [0, 1, 2, 3, 7], out_aut)

check(in_aut.transpose(), [0, 1, 2, 3, 7],
      in_aut.filter([0, 1, 2, 3]).transpose())

# Regression: evaluate did not work once.
CHECK_EQ('1', in_aut.filter([0, 1, 2, 3, 4, 5]).evaluate('cba'))
CHECK_EQ('1', in_aut.filter([0, 1, 2, 3, 4, 5]).evaluate('a'))

CHECK_EQ('0', in_aut.filter([0, 1]).evaluate('cba'))
CHECK_EQ('1', in_aut.filter([0, 1]).evaluate('a'))

CHECK_EQ('0', in_aut.filter([]).evaluate('cba'))
CHECK_EQ('0', in_aut.filter([]).evaluate('a'))
