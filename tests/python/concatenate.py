#! /usr/bin/env python

import vcsn
from test import *

ab = vcsn.context('lal_char(ab)_b').ratexp('(a+b)*')
bc = vcsn.context('lal_char(bc)_b').ratexp('(b+c)*')
result = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abc)_b"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F0
    F1
    F2
    F3
    F4
  }
  {
    node [shape = circle]
    0
    1
    2
    3
    4
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  0 -> 3 [label = "b"]
  0 -> 4 [label = "c"]
  1 -> F1
  1 -> 1 [label = "a"]
  1 -> 2 [label = "b"]
  1 -> 3 [label = "b"]
  1 -> 4 [label = "c"]
  2 -> F2
  2 -> 1 [label = "a"]
  2 -> 2 [label = "b"]
  2 -> 3 [label = "b"]
  2 -> 4 [label = "c"]
  3 -> F3
  3 -> 3 [label = "b"]
  3 -> 4 [label = "c"]
  4 -> F4
  4 -> 3 [label = "b"]
  4 -> 4 [label = "c"]
}
''')
CHECK_EQ(ab.standard().concatenate(bc.standard()), result)

# Using AB, BC on concatenate in this form,
#
#  ab.concatenate(bc).standard().sort()
#
# gives a context of "lal_char(ab)_b", though the correct result
# should be "lal_char(abc)_b".  For now, we expect this to fail until
# we fix this issue.  This is a fragile test.
CHECK_NEQ(ab.concatenate(bc).standard().sort(), result)

a = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab)_z"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I
    F
  }
  {
    node [shape = circle]
    0
    1
    2
  }
  I -> 0
  0 -> 1 [label = "a, b"]
  1 -> 2 [label = "b"]
  2 -> 2 [label = "a, b"]
  2 -> F [label = "<2>"]
}
''').standard()

CHECK_EQ(a.concatenate(a), vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab)_z"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F4
  }
  {
    node [shape = circle]
    0
    1
    2
    3
    4
  }
  I0 -> 0
  0 -> 1 [label = "a, b"]
  1 -> 2 [label = "b"]
  2 -> 2 [label = "a, b"]
  2 -> 3 [label = "<2>a, <2>b"]
  3 -> 4 [label = "b"]
  4 -> F4 [label = "<2>"]
  4 -> 4 [label = "a, b"]
}
'''))


# Check union of contexts.
a = vcsn.context('lal_char(a)_ratexpset<lal_char(x)_b>').ratexp('<x>a*').standard()
b = vcsn.context('lal_char(b)_ratexpset<lal_char(y)_b>').ratexp('<y>b*').standard()
CHECK_EQ(a.concatenate(b).standard(), vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab)_ratexpset<lal_char(xy)_b>"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F0
    F1
    F2
  }
  {
    node [shape = circle]
    0
    1
    2
  }
  I0 -> 0
  0 -> F0 [label = "<xy>"]
  0 -> 1 [label = "<x>a"]
  0 -> 2 [label = "<xy>b"]
  1 -> F1 [label = "<y>"]
  1 -> 1 [label = "a"]
  1 -> 2 [label = "<y>b"]
  2 -> F2
  2 -> 2 [label = "b"]
}
'''))

