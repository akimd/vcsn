#! /usr/bin/env python

import vcsn
from test import *

# check complete algorithm
# ------------------------
def check(i, o):
  i = vcsn.automaton(i)
  o = vcsn.automaton(o)
  CHECK_EQ(o.sort(), i.insplit().sort())
  # Idempotence.
  CHECK_EQ(o.sort(), o.insplit().sort())


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
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
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
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> 2 [label = "c"]
  2 -> F2
}
''')


check('''
digraph
{
  vcsn_context = "lan_char(abc)_b"

  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "\\\\e"]
  1 -> 2 [label = "c"]
  2 -> F2
}''','''digraph
{
  vcsn_context = "lan_char(abc)_b"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F2
    F3
  }
  {
    node [shape = circle]
    0
    1
    2
    3
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 3 [label = "\\\\e"]
  1 -> 2 [label = "c"]
  2 -> F2
  3 -> F3
}
''')

check('''
digraph
{
  vcsn_context = "lan_char(abc)_b"

  I0 -> 0
  0 -> 1 [label = "\\\\e"]
  0 -> 2 [label = "\\\\e"]
  1 -> 2 [label = "c"]
  2 -> F2
}''','''digraph
{
  vcsn_context = "lan_char(abc)_b"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F2
    F3
  }
  {
    node [shape = circle]
    0
    1
    2
    3
  }
  I0 -> 0
  0 -> 1 [label = "\\\\e"]
  0 -> 3 [label = "\\\\e"]
  1 -> 2 [label = "c"]
  2 -> F2
  3 -> F3
}
''')
