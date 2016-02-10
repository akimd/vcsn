#! /usr/bin/env python

import vcsn
from test import *

a1 = load('lal_char_b/a1.gv')
b1 = load('lal_char_z/b1.gv')
minab = load('lal_char_zmin/minab.gv')

## ------------ ##
## Accessible.  ##
## ------------ ##

## check_accessible INPUT OUTPUT
## -----------------------------
def check_accessible(input, output):
    if isinstance(input, str):
        input = vcsn.automaton(input)
    CHECK_EQ(output, input.accessible())
    CHECK(input.accessible().is_accessible())

# Regression.
a = vcsn.context('lal_char(abcd), b').expression('ab+cd').standard().sort().strip()
check_accessible(a, a)

# Cycle.
check_accessible('''
digraph {
    vcsn_context = "law_char(ab), b"
    I0 -> 0
    0 -> 1 [label="a"]
    1 -> 2 [label="a"]
    2 -> 3 [label="a"]
    3 -> 5 [label="a"]
    5 -> 2 [label="a"]
    3 -> F5
    4 -> 1 [label="a"]
}''',
'''digraph
{
  vcsn_context = "wordset<char_letters(ab)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F3
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
    3
    4
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  1 -> 2 [label = "a"]
  2 -> 3 [label = "a"]
  3 -> F3
  3 -> 4 [label = "a"]
  4 -> 2 [label = "a"]
}''')


# No final state.
check_accessible('''
digraph {
    vcsn_context = "law_char(ab), b"
    I0 -> 0
    0 -> 1 [label="a"]
    1 -> 2 [label="a"]
    2 -> 3 [label="a"]
    4 -> 1 [label="a"]
}
''',
'''digraph
{
  vcsn_context = "wordset<char_letters(ab)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [color = DimGray]
    1 [color = DimGray]
    2 [color = DimGray]
    3 [color = DimGray]
  }
  I0 -> 0 [color = DimGray]
  0 -> 1 [label = "a", color = DimGray]
  1 -> 2 [label = "a", color = DimGray]
  2 -> 3 [label = "a", color = DimGray]
}''')

# No initial state.
check_accessible('''
digraph {
    vcsn_context = "law_char(ab), b"
    0 -> 1 [label="a"]
    1 -> 2[label="a"]
    2 -> 3[label="a"]
    3 -> F5
    4 -> 1[label="a"]
}
''',
'''digraph
{
  vcsn_context = "wordset<char_letters(ab)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
}''')


# Simple input.
check_accessible('''
digraph {
    vcsn_context = "law_char(ab), b"
    I0 -> 0
    0 -> 1 [label="a"]
    1 -> 2[label="a"]
    2 -> 3[label="a"]
    3 -> F5
    4 -> 1[label="a"]
}
''',
'''digraph
{
  vcsn_context = "wordset<char_letters(ab)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
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
  1 -> 2 [label = "a"]
  2 -> 3 [label = "a"]
  3 -> F3
}''')


## --------- ##
## is-trim.  ##
## --------- ##

def check_is_trim(input, exp):
    if isinstance(input, str):
        input = vcsn.automaton(input)
    CHECK_EQ(exp, input.is_trim())

check_is_trim(a1, True)
check_is_trim(b1, True)
check_is_trim(minab, True)

check_is_trim('''
digraph
{
  vcsn_context = "lal_char(a), b"
  0 -> 1 [label = a]
  1 -> F1
  1 -> 2 [label = a]
  I0 -> 0
}
''', False)

# No initial.
check_is_trim('''
digraph
{
  vcsn_context = "lal_char(a), b"
  0 -> 1 [label = a]
  1 -> F1
  1 -> 2 [label = a]
  2 -> 1 [label = a]
}
''', False)

# No final.
check_is_trim('''
digraph
{
  vcsn_context = "lal_char(a), b"
  0 -> 1 [label = a]
  1 -> 2 [label = a]
  2 -> 1 [label = a]
  I0 -> 0
}
''', False)


## ---------- ##
## is-empty.  ##
## ---------- ##
def check_is_empty(input, exp):
    if isinstance(input, str):
        input = vcsn.automaton(input)
    CHECK_EQ(exp, input.is_empty())
check_is_empty(a1, False)
check_is_empty(b1, False)
check_is_empty(minab, False)

check_is_empty('''
digraph
{
  vcsn_context = "lal_char(a), b"
  I0 -> 0
}
''', False)

check_is_empty('''
digraph
{
  vcsn_context = "lal_char(a), b"
  0 -> F0
}
''', False)

check_is_empty('''
digraph
{
  vcsn_context = "lal_char(a), b"
  I0
  F0
}
''', True)
check_is_empty('''
digraph
{
  vcsn_context = "lal_char(a), b"
}
''', True)

## ------------ ##
## is-useless.  ##
## ------------ ##


def check_is_useless(input, exp):
    if isinstance(input, str):
        input = vcsn.automaton(input)
    CHECK_EQ(exp, input.is_useless())

check_is_useless(a1, False)
check_is_useless(b1, False)
check_is_useless(minab, False)

check_is_useless('''
digraph
{
  vcsn_context = "lal_char(a), b"
  I0 -> 0
}
''', True)

check_is_useless('''
digraph
{
  vcsn_context = "lal_char(a), b"
  0 -> F0
}
''', True)

# no initial.
check_is_useless('''
digraph
{
  vcsn_context = "lal_char(a), b"
  0 -> 1 [label = a]
  1 -> F1
  1 -> 2 [label = a]
  2 -> 1 [label = a]
}
''', True)

# no final.
check_is_useless('''
digraph
{
  vcsn_context = "lal_char(a), b"
  0 -> 1 [label = a]
  1 -> 2 [label = a]
  2 -> 1 [label = a]
  I0 -> 0
}
''', True)


## ------ ##
## trim.  ##
## ------ ##
def check_trim(input, exp):
    if isinstance(input, str):
        input = vcsn.automaton(input)
    aut = input.trim()
    CHECK_EQ(exp, aut)
    CHECK(aut.is_trim())
    CHECK(aut.is_accessible())
    CHECK(aut.is_coaccessible())

    CHECK_EQ(exp, input.coaccessible().accessible())
    CHECK_EQ(exp, input.accessible().coaccessible())

check_trim(a1, a1)
check_trim(b1, b1)
check_trim(minab, minab)

check_trim('''
digraph
{
  vcsn_context = "lal_char(a), b"
  0 -> 1 [label = a]
  1 -> F1
  1 -> 2 [label = a]
  I0 -> 0
}
''', '''digraph
{
  vcsn_context = "letterset<char_letters(a)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F1
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  1 -> F1
}''')

check_trim('''
digraph
{
  vcsn_context = "lal_char(a), b"
  0 -> 1 [label = a]
  0 -> 4 [label = a]
  1 -> F1
  1 -> 2 [label = a]
  I0 -> 0
  3 -> 4 [label = a]
  4 -> 0 [label = a]
  4 -> 5 [label = a]
}
''', '''digraph
{
  vcsn_context = "letterset<char_letters(a)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F1
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "a"]
  1 -> F1
  2 -> 0 [label = "a"]
}''')
