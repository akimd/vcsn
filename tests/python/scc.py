#! /usr/bin/env python

import vcsn
from test import *

a = vcsn.automaton('''digraph {
  vcsn_context = "lal_char(abc), b"
}''')
CHECK_EQ(0, a.num_sccs())

a = vcsn.automaton('''digraph {
  vcsn_context = "lal_char(abc), b"
  I0 -> 0
  0 -> F0
}''')
CHECK_EQ(1, a.num_sccs())

a = vcsn.automaton('''digraph {
  vcsn_context = "lal_char(abc), b"
  I0 -> 0
  0 -> 1 [label = "a"]
  1 -> F1
}''')
CHECK_EQ(2, a.num_sccs())

a = vcsn.automaton('''digraph {
  vcsn_context = "lal_char(abc), b"
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "a"]
  1 -> 2 [label = "b"]
  2 -> 0 [label = "b"]
}''')
CHECK_EQ(1, a.num_sccs())

a = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abc), b"
  I0 -> 0
  0 -> 1 [label = "c"]
  1 -> 2 [label = "a"]
  2 -> 3 [label = "b"]
  2 -> 4 [label = "b"]
  3 -> 5 [label = "c"]
  3 -> 6 [label = "c"]
  4 -> 7 [label = "c"]
  5 -> 2 [label = "a"]
  6 -> 0 [label = "b"]
  7 -> F7
  7 -> 2 [label = "a"]
}''')
CHECK_EQ(1, a.num_sccs())

a = vcsn.automaton('''digraph
{
  vcsn_context = "lal_char(abcd), zmin"
  I0 -> 0
  0 -> 1 [label = "<2>a"]
  0 -> 2 [label = "<3>a"]
  1 -> 3 [label = "<3>b"]
  1 -> 4 [label = "<2>c"]
  2 -> 4 [label = "<5>d"]
  2 -> 5 [label = "<3>b"]
  3 -> 6 [label = "<6>c"]
  4 -> F4
  5 -> 7 [label = "<6>c"]
  6 -> 1 [label = "<9>d"]
  7 -> 2 [label = "<13>d"]
}''')
CHECK_EQ(4, a.num_sccs())
