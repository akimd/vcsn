#! /usr/bin/env python

import vcsn
from test import *

aut1 = vcsn.automaton(r'''
digraph
{
  vcsn_context = "lal(abcd), zmin"

  I0 -> 0
  0 -> 1 [label = "<1>a"]
  0 -> 2 [label = "<2>a"]
  1 -> 1 [label = "<3>b"]
  1 -> 3 [label = "<5>c"]
  2 -> 2 [label = "<3>b"]
  2 -> 3 [label = "<6>d"]
  3 -> F3 [label = "<0>"]
}
''')
CHECK(aut1.has_twins_property())


aut2 = vcsn.automaton(r'''
digraph
{
  vcsn_context = "lal(abcd), zmin"

  I0 -> 0
  0 -> 1 [label = "<1>a"]
  0 -> 2 [label = "<2>a"]
  1 -> 1 [label = "<3>b"]
  1 -> 3 [label = "<5>c"]
  2 -> 2 [label = "<4>b"]
  2 -> 3 [label = "<6>d"]
  3 -> F3 [label = "<0>"]
}
''')
CHECK(not aut2.has_twins_property())


aut3 = vcsn.automaton('''digraph
{
  vcsn_context = "lal(abcd), zmin"
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
  7 -> 2 [label = "<9>d"]
}''')
CHECK(aut3.has_twins_property())


aut4 = vcsn.automaton('''digraph
{
  vcsn_context = "lal(abcd), zmin"
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
CHECK(not aut4.has_twins_property())


aut5 = vcsn.automaton('''digraph
{
  vcsn_context = "lal(abcd), q"
  I0 -> 0
  0 -> 1 [label = "<2/3>a"]
  0 -> 2 [label = "<3/5>a"]
  1 -> 3 [label = "<4>b"]
  1 -> 4 [label = "<2>c"]
  2 -> 4 [label = "<7>d"]
  2 -> 5 [label = "<2>b"]
  3 -> 1 [label = "<6>d"]
  4 -> F4
  5 -> 2 [label = "<12>d"]
}''')
CHECK(aut5.has_twins_property())


aut6 = vcsn.automaton('''digraph
{
  vcsn_context = "lal(abcd), q"
  I0 -> 0
  0 -> 1 [label = "<2/3>a"]
  0 -> 2 [label = "<3/5>a"]
  1 -> 3 [label = "<4>b"]
  1 -> 4 [label = "<2>c"]
  2 -> 4 [label = "<7>d"]
  2 -> 5 [label = "<2>b"]
  3 -> 1 [label = "<6>d"]
  4 -> F4
  5 -> 2 [label = "<17>d"]
}''')
CHECK(not aut6.has_twins_property())


aut7 = vcsn.automaton('''digraph {
  vcsn_context = "lal(abc), z"
  I0 -> 0
  0 -> 1 [label ="<3>a"]
  0 -> 2 [label = "<5>a"]
  1 -> 3 [label = "<3>b"]
  1 -> F3
  2 -> 4 [label = "<2>b"]
  2 -> F3
  3 -> 1 [label = "<2>a"]
  4 -> 2 [label = "<3>a"]
}''')
CHECK(aut7.has_twins_property())


aut8 = vcsn.automaton('''digraph {
  vcsn_context = "lal(abc), z"
  I0 -> 0
  0 -> 1 [label ="<3>a"]
  0 -> 2 [label = "<5>a"]
  1 -> 3 [label = "<3>b"]
  1 -> F3
  2 -> 4 [label = "<2>b"]
  2 -> F3
  3 -> 1 [label = "<2>a"]
  4 -> 2 [label = "<5>a"]
}''')
CHECK(not aut8.has_twins_property())
