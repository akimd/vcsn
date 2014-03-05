#! /usr/bin/env python

import vcsn
from test import *

a1 = load('lal_char_b/a1.gv')
b1 = load('lal_char_z/b1.gv')
minab = load('lal_char_zmin/minab.gv')

## ----------- ##
## Accessible. ##
## ----------- ##

## check_accessible INPUT OUTPUT
## -----------------------------
def check_accessible(input, output):
  if isinstance(input, str):
    input = vcsn.automaton(input)
  if isinstance(output, str):
    output = vcsn.automaton(output)
  CHECK_EQ(input.accessible().sort(), output.sort())

## ----------------------------- ##
## Regression: standard(ab+cd).  ##
## ----------------------------- ##

a = vcsn.context('lal_char(abcd)_b').ratexp('ab+cd').standard()
check_accessible(a, a)

## ------- ##
## Cycle.  ##
## ------- ##

check_accessible('''
digraph {
    vcsn_context = "law_char(ab)_b"
    I0 -> 0
    0 -> 1 [label="a"]
    1 -> 2[label="a"]
    2 -> 3[label="a"]
    3 -> 5[label="a"]
    5 -> 2[label="a"]
    3 -> F5
    4 -> 1[label="a"]
}''',
'''
digraph
{
  vcsn_context = "law_char(ab)_b"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F3
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
  0 -> 1 [label = "a"]
  1 -> 2 [label = "a"]
  2 -> 3 [label = "a"]
  3 -> F3
  3 -> 4 [label = "a"]
  4 -> 2 [label = "a"]
}
''')


## ---------------- ##
## No final State.  ##
## ---------------- ##

check_accessible('''
digraph {
    vcsn_context = "law_char(ab)_b"
    I0 -> 0
    0 -> 1 [label="a"]
    1 -> 2[label="a"]
    2 -> 3[label="a"]
    4 -> 1[label="a"]
}
''',
'''
digraph
{
  vcsn_context = "law_char(ab)_b"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
  }
  {
    node [shape = circle]
    0 [color = DimGray]
    1 [color = DimGray]
    2 [color = DimGray]
    3 [color = DimGray]
  }
  I0 -> 0 [color = DimGray]
  0 -> 1 [label = "a", color = DimGray]
  1 -> 2 [label = "a", color = DimGray]
  2 -> 3 [label = "a", color = DimGray]
}
''')

## ------------------ ##
## No initial state.  ##
## ------------------ ##

check_accessible('''
digraph {
    vcsn_context = "law_char(ab)_b"
    0 -> 1 [label="a"]
    1 -> 2[label="a"]
    2 -> 3[label="a"]
    3 -> F5
    4 -> 1[label="a"]
}
''',
'''
digraph
{
  vcsn_context = "law_char(ab)_b"
  rankdir = LR
}
''')


## -------------- ##
## Simple input.  ##
## -------------- ##

check_accessible('''
digraph {
    vcsn_context = "law_char(ab)_b"
    I0 -> 0
    0 -> 1 [label="a"]
    1 -> 2[label="a"]
    2 -> 3[label="a"]
    3 -> F5
    4 -> 1[label="a"]
}
''',
'''
digraph
{
  vcsn_context = "law_char(ab)_b"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
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
  0 -> 1 [label = "a"]
  1 -> 2 [label = "a"]
  2 -> 3 [label = "a"]
  3 -> F3
}
''')


## ------- ##
## is-trim ##
## ------- ##

def check_is_trim (input, output):
  if isinstance(input, str):
    input = vcsn.automaton(input)
  CHECK_EQ(input.is_trim(), output)

check_is_trim(a1, True)
check_is_trim(b1, True)
check_is_trim(minab, True)

check_is_trim('''
digraph
{
  vcsn_context = "lal_char(a)_b"
  0 -> 1 [label = a]
  1 -> F1
  1 -> 2 [label = a]
  I0 -> 0
}
''', False)

## ---------- ##
## no initial ##
## ---------- ##
check_is_trim('''
digraph
{
  vcsn_context = "lal_char(a)_b"
  0 -> 1 [label = a]
  1 -> F1
  1 -> 2 [label = a]
  2 -> 1 [label = a]
}
''', False)

## -------- ##
## no final. ##
## -------- ##
check_is_trim('''
digraph
{
  vcsn_context = "lal_char(a)_b"
  0 -> 1 [label = a]
  1 -> 2 [label = a]
  2 -> 1 [label = a]
  I0 -> 0
}
''', False)


## -------- ##
## is-empty ##
## -------- ##
def check_is_empty (input, output):
  if isinstance(input, str):
    input = vcsn.automaton(input)
  CHECK_EQ(input.is_empty(), output)
check_is_empty(a1, False)
check_is_empty(b1, False)
check_is_empty(minab, False)

check_is_empty('''
digraph
{
  vcsn_context = "lal_char(a)_b"
  I0 -> 0
}
''', False)

check_is_empty('''
digraph
{
  vcsn_context = "lal_char(a)_b"
  0 -> F0
}
''', False)

check_is_empty('''
digraph
{
  vcsn_context = "lal_char(a)_b"
  I0
  F0
}
''', True)
check_is_empty('''
digraph
{
  vcsn_context = "lal_char(a)_b"
}
''', True)

## ---------- ##
## is-useless ##
## ---------- ##
def check_is_useless (input, output):
  if isinstance(input, str):
    input = vcsn.automaton(input)
  CHECK_EQ(input.is_useless(), output)

check_is_useless(a1, False)
check_is_useless(b1, False)
check_is_useless(minab, False)

check_is_useless('''
digraph
{
  vcsn_context = "lal_char(a)_b"
  I0 -> 0
}
''', True)

check_is_useless('''
digraph
{
  vcsn_context = "lal_char(a)_b"
  0 -> F0
}
''', True)

## ---------- ##
## no initial ##
## ---------- ##
check_is_useless('''
digraph
{
  vcsn_context = "lal_char(a)_b"
  0 -> 1 [label = a]
  1 -> F1
  1 -> 2 [label = a]
  2 -> 1 [label = a]
}
''', True)

## -------- ##
## no final ##
## -------- ##
check_is_useless('''
digraph
{
  vcsn_context = "lal_char(a)_b"
  0 -> 1 [label = a]
  1 -> 2 [label = a]
  2 -> 1 [label = a]
  I0 -> 0
}
''', True)


## ---- ##
## trim ##
## ---- ##
def check_trim(input, output):
  if isinstance(input, str):
    input = vcsn.automaton(input)
  if isinstance(output, str):
    output = vcsn.automaton(output)
  CHECK_EQ(input.trim(), output)

check_trim(a1, a1)
check_trim(b1, b1)
check_trim(minab, minab)

check_trim('''
digraph
{
  vcsn_context = "lal_char(a)_b"
  0 -> 1 [label = a]
  1 -> F1
  1 -> 2 [label = a]
  I0 -> 0
}
''', '''
digraph
{
  vcsn_context = "lal_char(a)_b"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F1
  }
  {
    node [shape = circle]
    0
    1
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  1 -> F1
}
''')

check_trim('''
digraph
{
  vcsn_context = "lal_char(a)_b"
  0 -> 1 [label = a]
  0 -> 4 [label = a]
  1 -> F1
  1 -> 2 [label = a]
  I0 -> 0
  3 -> 4 [label = a]
  4 -> 0 [label = a]
  4 -> 5 [label = a]
}
''', '''
digraph
{
  vcsn_context = "lal_char(a)_b"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F1
  }
  {
    node [shape = circle]
    0
    1
    2
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "a"]
  1 -> F1
  2 -> 0 [label = "a"]
}
''')
