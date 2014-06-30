#! /usr/bin/env python

import vcsn
from test import *


def suffix_check(i, o):
  CHECK_EQ(o, i.suffix())
  CHECK_EQ(o, i.suffix().suffix())


def prefix_check(i, o):
  CHECK_EQ(o, i.prefix())
  CHECK_EQ(o, i.prefix().prefix())


def factor_check(i, o):
  CHECK_EQ(o, i.factor())
  CHECK_EQ(o, i.factor().factor())

def subsequence_check(i, o):
  CHECK_EQ(o, i.subsequence())
#  CHECK_EQ(o, i.subsequence().subsequence())

def to_nullable_automate(aut):
  return vcsn.automaton(aut.format('dot').replace("lal_char(", "lan_char("))



## --------------- ##
## Test automata.  ##
## --------------- ##

aut1 = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab)_z"

  I0 -> 0 [label = "<2>"]
  0 -> 1  [label = "<3>a"]
  1 -> 2  [label = "<5>b"]
  2 -> 2  [label = "<7>b"]
  2 -> F2 [label = "<11>"]
}
''')

aut2 = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abc)_z"

  I0 -> 0
  0 -> 1 [label = "<3>a"]
  1 -> 1 [label = "b"]
  1 -> 2 [label = "c"]
  2 -> F2
  2 -> 3 [label = "a", color = DimGray]
  3 -> 4 [label = "b", color = DimGray]
  5 -> 1 [label = "b", color = DimGray]
  6 -> 5 [label = "c", color = DimGray]
}
''')

aut3 = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(a)_z"

  I0 -> 0
  0 -> F0
  0 -> 0 [label = "a"]
}
''')

aut4 = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abc)_z"

  I0 -> 0
  I1 -> 1 [color = DimGray]
  0 -> 2 [label = "a"]
  1 -> 3 [label = "b", color = DimGray]
  2 -> 2 [label = "b"]
  2 -> 4 [label = "c"]
  3 -> 3 [label = "b", color = DimGray]
  4 -> 3 [label = "a", color = DimGray]
  4 -> 5 [label = "b"]
  5 -> F5
}
''')

# suffix checking
# ---------------
suffix_check(aut1, '''digraph
{
  vcsn_context = "lal_char(ab)_z"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    I1
    I2
    F2
  }
  {
    node [shape = circle]
    0
    1
    2
  }
  I0 -> 0
  I1 -> 1
  I2 -> 2
  0 -> 1 [label = "<3>a"]
  1 -> 2 [label = "<5>b"]
  2 -> F2 [label = "<11>"]
  2 -> 2 [label = "<7>b"]
}''')

suffix_check(aut2, '''digraph
{
  vcsn_context = "lal_char(abc)_z"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    I1
    I2
    I3
    I4
    F2
  }
  {
    node [shape = circle]
    0
    1
    2
    3 [color = DimGray]
    4 [color = DimGray]
    5 [color = DimGray]
    6 [color = DimGray]
  }
  I0 -> 0
  I1 -> 1
  I2 -> 2
  I3 -> 3 [color = DimGray]
  I4 -> 4 [color = DimGray]
  0 -> 1 [label = "<3>a"]
  1 -> 1 [label = "b"]
  1 -> 2 [label = "c"]
  2 -> F2
  2 -> 3 [label = "a", color = DimGray]
  3 -> 4 [label = "b", color = DimGray]
  5 -> 1 [label = "b", color = DimGray]
  6 -> 5 [label = "c", color = DimGray]
}''')

suffix_check(aut3, '''digraph
{
  vcsn_context = "lal_char(a)_z"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F0
  }
  {
    node [shape = circle]
    0
  }
  I0 -> 0
  0 -> F0
  0 -> 0 [label = "a"]
}''')

suffix_check(aut4, '''digraph
{
  vcsn_context = "lal_char(abc)_z"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    I1
    I2
    I3
    I4
    I5
    F5
  }
  {
    node [shape = circle]
    0
    1 [color = DimGray]
    2
    3 [color = DimGray]
    4
    5
  }
  I0 -> 0
  I1 -> 1 [color = DimGray]
  I2 -> 2
  I3 -> 3 [color = DimGray]
  I4 -> 4
  I5 -> 5
  0 -> 2 [label = "a"]
  1 -> 3 [label = "b", color = DimGray]
  2 -> 2 [label = "b"]
  2 -> 4 [label = "c"]
  3 -> 3 [label = "b", color = DimGray]
  4 -> 3 [label = "a", color = DimGray]
  4 -> 5 [label = "b"]
  5 -> F5
}''')


# prefix checking
# ---------------
prefix_check(aut1, '''digraph
{
  vcsn_context = "lal_char(ab)_z"
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
  I0 -> 0 [label = "<2>"]
  0 -> F0
  0 -> 1 [label = "<3>a"]
  1 -> F1
  1 -> 2 [label = "<5>b"]
  2 -> F2
  2 -> 2 [label = "<7>b"]
}''')

prefix_check(aut2, '''digraph
{
  vcsn_context = "lal_char(abc)_z"
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
    3 [color = DimGray]
    4 [color = DimGray]
    5 [color = DimGray]
    6 [color = DimGray]
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "<3>a"]
  1 -> F1
  1 -> 1 [label = "b"]
  1 -> 2 [label = "c"]
  2 -> F2
  2 -> 3 [label = "a", color = DimGray]
  3 -> 4 [label = "b", color = DimGray]
  5 -> F5 [color = DimGray]
  5 -> 1 [label = "b", color = DimGray]
  6 -> F6 [color = DimGray]
  6 -> 5 [label = "c", color = DimGray]
}''')

prefix_check(aut3,'''digraph
{
  vcsn_context = "lal_char(a)_z"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F0
  }
  {
    node [shape = circle]
    0
  }
  I0 -> 0
  0 -> F0
  0 -> 0 [label = "a"]
}''')

prefix_check(aut4, '''digraph
{
  vcsn_context = "lal_char(abc)_z"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    I1
    F0
    F2
    F4
    F5
  }
  {
    node [shape = circle]
    0
    1 [color = DimGray]
    2
    3 [color = DimGray]
    4
    5
  }
  I0 -> 0
  I1 -> 1 [color = DimGray]
  0 -> F0
  0 -> 2 [label = "a"]
  1 -> 3 [label = "b", color = DimGray]
  2 -> F2
  2 -> 2 [label = "b"]
  2 -> 4 [label = "c"]
  3 -> 3 [label = "b", color = DimGray]
  4 -> F4
  4 -> 3 [label = "a", color = DimGray]
  4 -> 5 [label = "b"]
  5 -> F5
}''')

aut5 = vcsn.automaton('''digraph
{
  vcsn_context = "lal_char(abc)_z"

  I0 -> 0
  0 -> 1 [label = "<3>a"]
  1 -> 2 [label = "b"]
  2 -> 2 [label = "a"]
  2 -> 3 [label = "c"]
  3 -> F3
}
''')

aut6 = vcsn.automaton('''digraph
{
  vcsn_context = "lal_char(abc)_z"
  I0 -> 0
  0 -> 1 [label = "<3>a"]
  1 -> 1 [label = "b"]
  1 -> 2 [label = "c"]
  2 -> 3 [label = "a"]
  3 -> 4 [label = "b"]
  3 -> 5 [label = "c"]
  4 -> F4
  6 -> 0 [label = "c"]
}
''')

aut7 = vcsn.automaton('''digraph
{
  vcsn_context = "lal_char(abc)_z"
  I0 -> 0
  0 -> 1 [label = "<3>a"]
  0 -> 2 [label = "b"]
  1 -> 3 [label = "c"]
  2 -> 3 [label = "b"]
  2 -> 4 [label = "a"]
  2 -> 5 [label = "c"]
  3 -> 6 [label = "b"]
  4 -> 2 [label = "a"]
  4 -> 5 [label = "b"]
  6 -> F6
  6 -> 7 [label = "a"]
  8 -> 0 [label = "c"]
}
''')


# factor checking
# ---------------
factor_check(aut5, '''digraph
{
  vcsn_context = "lal_char(abc)_z"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    I1
    I2
    I3
    F0
    F1
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
  I1 -> 1
  I2 -> 2
  I3 -> 3
  0 -> F0
  0 -> 1 [label = "<3>a"]
  1 -> F1
  1 -> 2 [label = "b"]
  2 -> F2
  2 -> 2 [label = "a"]
  2 -> 3 [label = "c"]
  3 -> F3
}''')

factor_check(aut6, '''digraph
{
  vcsn_context = "lal_char(abc)_z"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    I1
    I2
    I3
    I4
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
    5 [color = DimGray]
    6 [color = DimGray]
  }
  I0 -> 0
  I1 -> 1
  I2 -> 2
  I3 -> 3
  I4 -> 4
  0 -> F0
  0 -> 1 [label = "<3>a"]
  1 -> F1
  1 -> 1 [label = "b"]
  1 -> 2 [label = "c"]
  2 -> F2
  2 -> 3 [label = "a"]
  3 -> F3
  3 -> 4 [label = "b"]
  3 -> 5 [label = "c", color = DimGray]
  4 -> F4
  6 -> 0 [label = "c", color = DimGray]
}''')

factor_check(aut3, '''digraph
{
  vcsn_context = "lal_char(a)_z"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F0
  }
  {
    node [shape = circle]
    0
  }
  I0 -> 0
  0 -> F0
  0 -> 0 [label = "a"]
}''')

factor_check(aut7, '''digraph
{
  vcsn_context = "lal_char(abc)_z"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    I1
    I2
    I3
    I4
    I6
    F0
    F1
    F2
    F3
    F4
    F6
  }
  {
    node [shape = circle]
    0
    1
    2
    3
    4
    5 [color = DimGray]
    6
    7 [color = DimGray]
    8 [color = DimGray]
  }
  I0 -> 0
  I1 -> 1
  I2 -> 2
  I3 -> 3
  I4 -> 4
  I6 -> 6
  0 -> F0
  0 -> 1 [label = "<3>a"]
  0 -> 2 [label = "b"]
  1 -> F1
  1 -> 3 [label = "c"]
  2 -> F2
  2 -> 3 [label = "b"]
  2 -> 4 [label = "a"]
  2 -> 5 [label = "c", color = DimGray]
  3 -> F3
  3 -> 6 [label = "b"]
  4 -> F4
  4 -> 2 [label = "a"]
  4 -> 5 [label = "b", color = DimGray]
  6 -> F6
  6 -> 7 [label = "a", color = DimGray]
  8 -> 0 [label = "c", color = DimGray]
}''')

# subsequence checking
# ---------------
subsequence_check(to_nullable_automate(aut5), r'''digraph
{
  vcsn_context = "lan<lal_char(abc)>_z"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
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
  0 -> 1 [label = "<3>\\e, <3>a"]
  1 -> 2 [label = "\\e, b"]
  2 -> 2 [label = "\\e, a"]
  2 -> 3 [label = "\\e, c"]
  3 -> F3
}''')

subsequence_check(to_nullable_automate(aut6), r'''digraph
{
  vcsn_context = "lan<lal_char(abc)>_z"
  rankdir = LR
  {
    node [shape = point, width = 0]
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
    5 [color = DimGray]
    6 [color = DimGray]
  }
  I0 -> 0
  0 -> 1 [label = "<3>\\e, <3>a"]
  1 -> 1 [label = "\\e, b"]
  1 -> 2 [label = "\\e, c"]
  2 -> 3 [label = "\\e, a"]
  3 -> 4 [label = "\\e, b"]
  3 -> 5 [label = "\\e, c", color = DimGray]
  4 -> F4
  6 -> 0 [label = "\\e, c", color = DimGray]
}''')

subsequence_check(to_nullable_automate(aut3), r'''digraph
{
  vcsn_context = "lan<lal_char(a)>_z"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F0
  }
  {
    node [shape = circle]
    0
  }
  I0 -> 0
  0 -> F0
  0 -> 0 [label = "\\e, a"]
}''')

subsequence_check(to_nullable_automate(aut7), r'''digraph
{
  vcsn_context = "lan<lal_char(abc)>_z"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F6
  }
  {
    node [shape = circle]
    0
    1
    2
    3
    4
    5 [color = DimGray]
    6
    7 [color = DimGray]
    8 [color = DimGray]
  }
  I0 -> 0
  0 -> 1 [label = "<3>\\e, <3>a"]
  0 -> 2 [label = "\\e, b"]
  1 -> 3 [label = "\\e, c"]
  2 -> 3 [label = "\\e, b"]
  2 -> 4 [label = "\\e, a"]
  2 -> 5 [label = "\\e, c", color = DimGray]
  3 -> 6 [label = "\\e, b"]
  4 -> 2 [label = "\\e, a"]
  4 -> 5 [label = "\\e, b", color = DimGray]
  6 -> F6
  6 -> 7 [label = "\\e, a", color = DimGray]
  8 -> 0 [label = "\\e, c", color = DimGray]
}''')
