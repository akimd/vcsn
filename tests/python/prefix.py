#! /usr/bin/env python

import vcsn
from test import *

# check prefix algorithm
# ----------------------
def check(i, o):
  i = vcsn.automaton(i)
  o = vcsn.automaton(o)
  CHECK_EQ(o, i.prefix())
  # prefix of prefix automaton is the same
  CHECK_EQ(o, i.prefix().prefix())

check('''
digraph
{
  vcsn_context = "lal_char(ab)_b"

  I0 -> 0
  0 -> 1 [label = "a"]
  1 -> 2 [label = "b"]
  2 -> 2 [label = "b"]
  2 -> F2
}
''', '''
digraph
{
  vcsn_context = "lal_char(ab)_b"
  rankdir = LR
  {
    node [shape = point, width = 0]
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
  0 -> F0
  0 -> 1 [label = "a"]
  1 -> F1
  1 -> 2 [label = "b"]
  2 -> 2 [label = "b"]
  2 -> F2
}
''')

check('''
digraph
{
  vcsn_context = "lal_char(abc)_b"

  I0 -> 0
  0 -> 1 [label = "a"]
  1 -> 1 [label = "b"]
  1 -> 2 [label = "c"]
  2 -> F2
  2 -> 3 [label = "a"]
  3 -> 4 [label = "b"]
  5 -> 1 [label = "b"]
  6 -> 5 [label = "c"]
}
''', '''
digraph
{
  vcsn_context = "lal_char(abc)_b";
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F0
    F1
    F2
    F5
    F6
  }
  {
    node [shape = circle]
    0
    1
    2
    3
    4
    5
    6
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "a"]
  1 -> F1
  1 -> 1 [label = "b"]
  1 -> 2 [label = "c"]
  2 -> F2
  2 -> 3 [label = "a"]
  3 -> 4 [label = "b"]
  5 -> F5
  5 -> 1 [label = "b"]
  6 -> F6
  6 -> 5 [label = "c"]
}
''')

check('''
digraph
{
  vcsn_context = "lal_char(a)_b"

  I0 -> 0
  0 -> F0
  0 -> 0 [label = "a"]
}
''','''
digraph
{
  vcsn_context = "lal_char(a)_b"

  rankdir = LR
  {
    node [shape = point, widht = 0]
    I0
    F0
  }

  {
    node [shape = cycle]
    0
  }

  I0 -> 0
  0 -> F0
  0 -> 0 [label = "a"]
}
''')

check('''
digraph
{
  vcsn_context = "lal_char(abc)_b"

  I0 -> 0
  I1 -> 1
  0 -> 2 [label = "a"]
  1 -> 3 [label = "b"]
  2 -> 2 [label = "b"]
  2 -> 4 [label = "c"]
  3 -> 3 [label = "b"]
  4 -> 3 [label = "a"]
  4 -> 5 [label = "b"]
  5 -> F5
}
''', '''
digraph
{
  vcsn_context = "lal_char(abc)_b"

  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    I1
    F2
    F4
    F5
  }

  {
    node [shape = cycle]
    0
    1
    2
    3
    4
    5
  }

  I0 -> 0
  I1 -> 1
  0 -> F0
  0 -> 2 [label = "a"]
  1 -> 3 [label = "b"]
  2 -> F2
  2 -> 2 [label = "b"]
  2 -> 4 [label = "c"]
  3 -> 3 [label = "b"]
  4 -> F4
  4 -> 3 [label = "a"]
  4 -> 5 [label = "b"]
  5 -> F5
}
''')
