#! /usr/bin/env python

import vcsn
from test import *


# check suffix algorithm
# ----------------------
def suffix_check(i, o):
  i = vcsn.automaton(i)
  o = vcsn.automaton(o)
  CHECK_EQ(o, i.suffix())
  # suffix of sufix automaton is the same
  CHECK_EQ(o.suffix(), i.suffix())


# check prefix algorithm
# ----------------------
def prefix_check(i, o):
  i = vcsn.automaton(i)
  o = vcsn.automaton(o)
  CHECK_EQ(o, i.prefix())
  # prefix of prefix automaton is the same
  CHECK_EQ(o.prefix(), i.prefix())


# check factor algorithm
# ----------------------
def factor_check(i, o):
  i = vcsn.automaton(i)
  o = vcsn.automaton(o)
  CHECK_EQ(o, i.factor())
  # factor of factor automaton is the same
  CHECK_EQ(o.factor(), i.factor())

# check subsequence algorithm
# ----------------------
def subsequence_check(i, o):
  i = vcsn.automaton(i)
  o = vcsn.automaton(o)
  CHECK_EQ(o, i.subsequence())
  # subsequence of subsequence automaton is the same
  CHECK_EQ(o.subsequence(), i.subsequence())


# Define automates for testing
# ----------------------------
aut1 = '''
digraph
{
  vcsn_context = "lal_char(ab)_b"

  I0 -> 0
  0 -> 1 [label = "a"]
  1 -> 2 [label = "b"]
  2 -> 2 [label = "b"]
  2 -> F2
}
'''

aut2 = '''
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
'''

aut3 = '''
digraph
{
  vcsn_context = "lal_char(a)_b"

  I0 -> 0
  0 -> F0
  0 -> 0 [label = "a"]
}
'''

aut4 = '''
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
'''

# suffix checking
# ---------------
suffix_check(aut1, '''
digraph
{
  vcsn_context = "lal_char(ab)_b"

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
  0 -> 1 [label = "a"]
  1 -> 2 [label = "b"]
  2 -> F2
  2 -> 2 [label = "b"]
}
''')

suffix_check(aut2, '''
digraph
{
  vcsn_context = "lal_char(abc)_b"

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
    node [shape = cycle]
    0
    1
    2
    3
    4
    5
    6
  }
  I0 -> 0
  I1 -> 1
  I2 -> 2
  I3 -> 3
  I4 -> 4
  0 -> 1 [label = "a"]
  1 -> 1 [label = "b"]
  1 -> 2 [label = "c"]
  2 -> F2
  2 -> 3 [label = "a"]
  3 -> 4 [label = "b"]
  5 -> 1 [label = "b"]
  6 -> 5 [label = "c"]
}
''')

suffix_check(aut3, '''
digraph
{
  vcsn_context = "lal_char(a)_b"

  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F0
  }

  {
    node [shape = cycle]
    0
  }

  I0 -> 0
  0 -> F0
  0 -> 0 [label ="a"]
}
''')

suffix_check(aut4, '''
digraph
{
  vcsn_context = "lal_char(abc)_b"
  
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
  I2 -> 2
  I3 -> 3
  I4 -> 4
  I5 -> 5
  0 -> 2 [label = "a"]
  1 -> 3 [label = "b"]
  2 -> 2 [label = "b"]
  2 -> 4 [label = "c"]
  3 -> 3 [label = "b"]
  4 -> 3 [label = "a"]
  4 -> 5 [label = "b"]
  5 -> F5
}
''')


# prefix checking
# ---------------
prefix_check(aut1, '''
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

prefix_check(aut2, '''
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

prefix_check(aut3,'''
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

prefix_check(aut4, '''
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

aut5 = '''
digraph
{
  vcsn_context = "lan_char(abc)_b"

  I0 -> 0
  0 -> 1 [label = "a"]
  1 -> 2 [label = "b"]
  2 -> 2 [label = "a"]
  2 -> 3 [label = "c"]
  3 -> F3
}
'''
aut6 = '''
digraph
{
  vcsn_context = "lan_char(abc)_b"
  I0 -> 0
  0 -> 1 [label = "a"]
  1 -> 1 [label = "b"]
  1 -> 2 [label = "c"]
  2 -> 3 [label = "a"]
  3 -> 4 [label = "b"]
  3 -> 5 [label = "c"]
  4 -> F4
  6 -> 0 [label = "c"]
}
'''

aut7 = '''
digraph
{
  vcsn_context = "lan_char(a)_b"

  I0 -> 0
  0 -> F0
  0 -> 0 [label = "a"]
}
'''

aut8 = '''
digraph
{
  vcsn_context = "lan_char(abc)_b"
  I0 -> 0
  0 -> 1 [label = "a"]
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
'''

# factor checking
# ---------------
factor_check('''
digraph
{
  vcsn_context = "lal_char(abc)_b"

  I0 -> 0
  0 -> 1 [label = "a"]
  1 -> 2 [label = "b"]
  2 -> 2 [label = "a"]
  2 -> 3 [label = "c"]
  3 -> F3
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
  0 -> 1 [label = "a"]
  1 -> F1
  1 -> 2 [label = "b"]
  2 -> F2
  2 -> 2 [label = "a"]
  2 -> 3 [label = "c"]
  3 -> F3
}
''')

factor_check('''
digraph
{
  vcsn_context = "lal_char(abc)_b"
  I0 -> 0
  0 -> 1 [label = "a"]
  1 -> 1 [label = "b"]
  1 -> 2 [label = "c"]
  2 -> 3 [label = "a"]
  3 -> 4 [label = "b"]
  3 -> 5 [label = "c"]
  4 -> F4
  6 -> 0 [label = "c"]
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
    5
    6
  }
  I0 -> 0
  I1 -> 1
  I2 -> 2
  I3 -> 3
  I4 -> 4
  0 -> F0
  0 -> 1 [label = "a"]
  1 -> F1
  1 -> 1 [label = "b"]
  1 -> 2 [label = "c"]
  2 -> F2
  2 -> 3 [label = "a"]
  3 -> F3
  3 -> 4 [label = "b"]
  3 -> 5 [label = "c"]
  4 -> F4
  6 -> 0 [label = "c"]
}
''')

factor_check(aut3, '''
digraph
{
  vcsn_context = "lal_char(a)_b"
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
}
''')

factor_check('''
digraph
{
  vcsn_context = "lal_char(abc)_b"
  I0 -> 0
  0 -> 1 [label = "a"]
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
''', '''
digraph
{
  vcsn_context = "lal_char(abc)_b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    I1
    I2
    I3
    I4
    I6
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
    5
    6
    7
    8
  }
  I0 -> 0
  I1 -> 1
  I2 -> 2
  I3 -> 3
  I4 -> 4
  I6 -> 6
  0 -> F0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> F1
  1 -> 3 [label = "c"]
  2 -> F2
  2 -> 3 [label = "b"]
  2 -> 4 [label = "a"]
  2 -> 5 [label = "c"]
  3 -> F3
  3 -> 6 [label = "b"]
  4 -> F4
  4 -> 2 [label = "a"]
  4 -> 5 [label = "b"]
  6 -> F6
  6 -> 7 [label = "a"]
  8 -> 0 [label = "c"]
}
''')

# subsequence checking
# ---------------
subsequence_check(aut5, r'''
digraph
{
  vcsn_context = "lan_char(abc)_b"

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
  0 -> 1 [label = "\\e, a"]
  1 -> 2 [label = "\\e, b"]
  2 -> 2 [label = "a"]
  2 -> 3 [label = "\\e, c"]
  3 -> F3
}
''')

subsequence_check(aut6, r'''
digraph
{
  vcsn_context = "lan_char(abc)_b"
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
    5
    6
  }
  I0 -> 0
  0 -> 1 [label = "\\e, a"]
  1 -> 1 [label = "b"]
  1 -> 2 [label = "\\e, c"]
  2 -> 3 [label = "\\e, a"]
  3 -> 4 [label = "\\e, b"]
  3 -> 5 [label = "\\e, c"]
  4 -> F4
  6 -> 0 [label = "\\e, c"]
}
''')

subsequence_check(aut7, r'''
digraph
{
  vcsn_context = "lan_char(a)_b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F0
  }
  {
    node [shape = point]
    0
  }
  I0 -> 0
  0 -> F0
  0 -> 0 [label = "a"]
}
''')

subsequence_check(aut8, r'''
digraph
{
  vcsn_context = "lan_char(abc)_b"
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
    5
    6
    7
    8
  }
  I0 -> 0
  0 -> 1 [label = "\\e, a"]
  0 -> 2 [label = "\\e, b"]
  1 -> 3 [label = "\\e, c"]
  2 -> 3 [label = "\\e, b"]
  2 -> 4 [label = "\\e, a"]
  2 -> 5 [label = "\\e, c"]
  3 -> 6 [label = "\\e, b"]
  4 -> 2 [label = "\\e, a"]
  4 -> 5 [label = "\\e, b"]
  6 -> F6
  6 -> 7 [label = "\\e, a"]
  8 -> 0 [label = "\\e, c"]
}
''')
