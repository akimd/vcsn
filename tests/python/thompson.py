#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context('lal_char(ab)_b')

def check(re, exp):
    # We compare automata as strings, since when parsing the expected
    # automaton we drop the hole in the state numbers created by
    # standard.
    a = ctx.ratexp(re).thompson()
    CHECK_EQ(exp, str(a.sort()))
    CHECK_EQ(True, a.is_normalized())

def xfail(re):
    r = ctx.ratexp(re)
    XFAIL(lambda: r.thompson())

# We don't support conjunction.
xfail('a*&b*')

## --- ##
## Z.  ##
## --- ##

# Z: "<12>\e".
check('(?@lan_char(a)_z)<12>\e',
r'''digraph
{
  vcsn_context = "lan<lal_char(a)>_z"
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
  0 -> 1 [label = "<12>\\e"]
  1 -> F1
}''')

## -------- ##
## Z: sum.  ##
## -------- ##

# Z: "\e+a+\e"
check('(?@lan_char(ab)_z)\e+a+\e',
r'''digraph
{
  vcsn_context = "lan<lal_char(ab)>_z"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F7
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
  }
  I0 -> 0
  0 -> 1 [label = "\\e"]
  0 -> 2 [label = "\\e"]
  0 -> 3 [label = "\\e"]
  1 -> 4 [label = "\\e"]
  2 -> 5 [label = "a"]
  3 -> 6 [label = "\\e"]
  4 -> 7 [label = "\\e"]
  5 -> 7 [label = "\\e"]
  6 -> 7 [label = "\\e"]
  7 -> F7
}''')

# Z: "<12>\e+<23>a+<34>b".
check('(?@lan_char(ab)_z)<12>\e+<23>a+<34>b',
r'''digraph
{
  vcsn_context = "lan<lal_char(ab)>_z"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F7
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
  }
  I0 -> 0
  0 -> 1 [label = "\\e"]
  0 -> 2 [label = "\\e"]
  0 -> 3 [label = "\\e"]
  1 -> 4 [label = "<12>\\e"]
  2 -> 5 [label = "<23>a"]
  3 -> 6 [label = "<34>b"]
  4 -> 7 [label = "\\e"]
  5 -> 7 [label = "\\e"]
  6 -> 7 [label = "\\e"]
  7 -> F7
}''')

# left weight.
check('(?@lan_char(ab)_z)<12>(\e+a+<10>b+<10>\e)',
r'''digraph
{
  vcsn_context = "lan<lal_char(ab)>_z"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F9
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
    9
  }
  I0 -> 0
  0 -> 1 [label = "<12>\\e"]
  0 -> 2 [label = "<12>\\e"]
  0 -> 3 [label = "<12>\\e"]
  0 -> 4 [label = "<12>\\e"]
  1 -> 5 [label = "\\e"]
  2 -> 6 [label = "a"]
  3 -> 7 [label = "<10>b"]
  4 -> 8 [label = "<10>\\e"]
  5 -> 9 [label = "\\e"]
  6 -> 9 [label = "\\e"]
  7 -> 9 [label = "\\e"]
  8 -> 9 [label = "\\e"]
  9 -> F9
}''')

# right weight.
check('(?@lan_char(ab)_z)(\e+a+<2>b+<3>\e)<10>',
r'''digraph
{
  vcsn_context = "lan<lal_char(ab)>_z"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F9
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
    9
  }
  I0 -> 0
  0 -> 1 [label = "\\e"]
  0 -> 2 [label = "\\e"]
  0 -> 3 [label = "\\e"]
  0 -> 4 [label = "\\e"]
  1 -> 5 [label = "\\e"]
  2 -> 6 [label = "a"]
  3 -> 7 [label = "<2>b"]
  4 -> 8 [label = "<3>\\e"]
  5 -> 9 [label = "<10>\\e"]
  6 -> 9 [label = "<10>\\e"]
  7 -> 9 [label = "<10>\\e"]
  8 -> 9 [label = "<10>\\e"]
  9 -> F9
}''')

## ------------ ##
## Z: product.  ##
## ------------ ##

# Z: "<12>(ab)<23>".
check('(?@lan_char(ab)_z)<12>(ab)<23>',
r'''digraph
{
  vcsn_context = "lan<lal_char(ab)>_z"
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
  0 -> 1 [label = "<12>a"]
  1 -> 2 [label = "\\e"]
  2 -> 3 [label = "<23>b"]
  3 -> F3
}''')

## --------- ##
## Z: star.  ##
## --------- ##

check('(?@lan_char(ab)_z)\z*',
r'''digraph
{
  vcsn_context = "lan<lal_char(ab)>_z"
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
  0 -> 1 [label = "\\e"]
  1 -> F1
}''')

check('(?@lan_char(ab)_b)\e*',
r'''digraph
{
  vcsn_context = "lan<lal_char(ab)>_b"
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
    3
  }
  I0 -> 0
  0 -> 1 [label = "\\e"]
  0 -> 2 [label = "\\e"]
  1 -> 3 [label = "\\e"]
  2 -> F2
  3 -> 1 [label = "\\e"]
  3 -> 2 [label = "\\e"]
}''')

check('(?@lan_char(ab)_z)(<2>a)*',
r'''digraph
{
  vcsn_context = "lan<lal_char(ab)>_z"
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
    3
  }
  I0 -> 0
  0 -> 1 [label = "\\e"]
  0 -> 2 [label = "\\e"]
  1 -> 3 [label = "<2>a"]
  2 -> F2
  3 -> 1 [label = "\\e"]
  3 -> 2 [label = "\\e"]
}''')

check('(?@lan_char(ab)_z)<2>a*<3>',
r'''digraph
{
  vcsn_context = "lan<lal_char(ab)>_z"
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
    3
  }
  I0 -> 0
  0 -> 1 [label = "<2>\\e"]
  0 -> 2 [label = "<6>\\e"]
  1 -> 3 [label = "a"]
  2 -> F2
  3 -> 1 [label = "\\e"]
  3 -> 2 [label = "<3>\\e"]
}''')

check('(?@lan_char(ab)_z)(<2>a+<3>b)*',
r'''digraph
{
  vcsn_context = "lan<lal_char(ab)>_z"
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
    3
    4
    5
    6
    7
  }
  I0 -> 0
  0 -> 1 [label = "\\e"]
  0 -> 2 [label = "\\e"]
  1 -> 3 [label = "\\e"]
  1 -> 4 [label = "\\e"]
  2 -> F2
  3 -> 5 [label = "<2>a"]
  4 -> 6 [label = "<3>b"]
  5 -> 7 [label = "\\e"]
  6 -> 7 [label = "\\e"]
  7 -> 1 [label = "\\e"]
  7 -> 2 [label = "\\e"]
}''')

check('(?@lan_char(ab)_z)<2>(<3>a+<5>b)*<7>',
r'''digraph
{
  vcsn_context = "lan<lal_char(ab)>_z"
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
    3
    4
    5
    6
    7
  }
  I0 -> 0
  0 -> 1 [label = "<2>\\e"]
  0 -> 2 [label = "<14>\\e"]
  1 -> 3 [label = "\\e"]
  1 -> 4 [label = "\\e"]
  2 -> F2
  3 -> 5 [label = "<3>a"]
  4 -> 6 [label = "<5>b"]
  5 -> 7 [label = "\\e"]
  6 -> 7 [label = "\\e"]
  7 -> 1 [label = "\\e"]
  7 -> 2 [label = "<7>\\e"]
}''')

check('(?@lan_char(ab)_z)<2>(<3>(ab)<5>)*<7>',
r'''digraph
{
  vcsn_context = "lan<lal_char(ab)>_z"
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
    3
    4
    5
  }
  I0 -> 0
  0 -> 1 [label = "<2>\\e"]
  0 -> 2 [label = "<14>\\e"]
  1 -> 3 [label = "<3>a"]
  2 -> F2
  3 -> 4 [label = "\\e"]
  4 -> 5 [label = "<5>b"]
  5 -> 1 [label = "\\e"]
  5 -> 2 [label = "<7>\\e"]
}''')

check('(?@lan_char(ab)_z)a**',
r'''digraph
{
  vcsn_context = "lan<lal_char(ab)>_z"
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
    3
    4
    5
  }
  I0 -> 0
  0 -> 1 [label = "\\e"]
  0 -> 2 [label = "\\e"]
  1 -> 3 [label = "\\e"]
  1 -> 4 [label = "\\e"]
  2 -> F2
  3 -> 5 [label = "a"]
  4 -> 1 [label = "\\e"]
  4 -> 2 [label = "\\e"]
  5 -> 3 [label = "\\e"]
  5 -> 4 [label = "\\e"]
}''')

## ---------- ##
## ZR: star.  ##
## ---------- ##

check('(?@lan_char(abcd)_ratexpset<lal_char(efgh)_z>)(<e>\e+abc)*',
r'''digraph
{
  vcsn_context = "lan<lal_char(abcd)>_ratexpset<lal_char(efgh)_z>"
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
    3
    4
    5
    6
    7
    8
    9
    10
    11
  }
  I0 -> 0
  0 -> 1 [label = "\\e"]
  0 -> 2 [label = "\\e"]
  1 -> 3 [label = "\\e"]
  1 -> 4 [label = "\\e"]
  2 -> F2
  3 -> 5 [label = "<e>\\e"]
  4 -> 6 [label = "a"]
  5 -> 7 [label = "\\e"]
  6 -> 8 [label = "\\e"]
  7 -> 1 [label = "\\e"]
  7 -> 2 [label = "\\e"]
  8 -> 9 [label = "b"]
  9 -> 10 [label = "\\e"]
  10 -> 11 [label = "c"]
  11 -> 7 [label = "\\e"]
}''')

check('(?@lan_char(abcd)_ratexpset<lal_char(efgh)_z>)(<e>\e+ab<f>)*',
r'''digraph
{
  vcsn_context = "lan<lal_char(abcd)>_ratexpset<lal_char(efgh)_z>"
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
    3
    4
    5
    6
    7
    8
    9
  }
  I0 -> 0
  0 -> 1 [label = "\\e"]
  0 -> 2 [label = "\\e"]
  1 -> 3 [label = "\\e"]
  1 -> 4 [label = "\\e"]
  2 -> F2
  3 -> 5 [label = "<e>\\e"]
  4 -> 6 [label = "a"]
  5 -> 7 [label = "\\e"]
  6 -> 8 [label = "\\e"]
  7 -> 1 [label = "\\e"]
  7 -> 2 [label = "\\e"]
  8 -> 9 [label = "<f>b"]
  9 -> 7 [label = "\\e"]
}''')

# Make sure that the initial weight of the rhs of the concatenation is
# properly handled.
check('(?@lan_char(a)_ratexpset<lal_char(xyz)_z>)<x>a(<y>\e+<z>a)',
r'''digraph
{
  vcsn_context = "lan<lal_char(a)>_ratexpset<lal_char(xyz)_z>"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F7
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
  }
  I0 -> 0
  0 -> 1 [label = "<x>a"]
  1 -> 2 [label = "\\e"]
  2 -> 3 [label = "\\e"]
  2 -> 4 [label = "\\e"]
  3 -> 5 [label = "<y>\\e"]
  4 -> 6 [label = "<z>a"]
  5 -> 7 [label = "\\e"]
  6 -> 7 [label = "\\e"]
  7 -> F7
}''')
