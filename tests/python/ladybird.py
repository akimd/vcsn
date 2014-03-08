#! /usr/bin/env python

import vcsn
from test import *

b = vcsn.context('lal_char(abc)_b')
z = vcsn.context('lal_char(abc)_z')

exp = vcsn.automaton(r'''
digraph
{
  vcsn_context = "lal_char(abc)_b"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F0
  }
  {
    node [shape = circle]
    0
    1
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "a"]
  1 -> 0 [label = "a, c"]
  1 -> 1 [label = "b, c"]
}''')

CHECK_EQ(exp, b.ladybird(2))
CHECK_EQ(vcsn.automaton(str(exp).replace('_b', '_z')), z.ladybird(2))

exp = vcsn.automaton(r'''
digraph
{
  vcsn_context = "lal_char(abc)_zmin"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F0
  }
  {
    node [shape = circle]
    0
    1
  }
  I0 -> 0 [label = "<0>"]
  0 -> F0 [label = "<0>"]
  0 -> 1 [label = "<0>a"]
  1 -> 0 [label = "<0>a, <0>c"]
  1 -> 1 [label = "<0>b, <0>c"]
}
''')
CHECK_EQ(exp,
         vcsn.context('lal_char(abc)_zmin').ladybird(2))
