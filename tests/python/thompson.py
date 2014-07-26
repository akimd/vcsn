#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context('lal_char(ab)_b')

def check(re, exp):
    # We compare automata as strings, since when parsing the expected
    # automaton we drop the hole in the state numbers created by
    # standard.
    a = ctx.ratexp(re).thompson()
    CHECK_EQ(exp, a)
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
    3
    4
    5
    6
    7
  }
  I0 -> 0
  0 -> 2 [label = "\\e"]
  0 -> 4 [label = "\\e"]
  0 -> 6 [label = "\\e"]
  1 -> F1
  2 -> 3 [label = "\\e"]
  3 -> 1 [label = "\\e"]
  4 -> 5 [label = "a"]
  5 -> 1 [label = "\\e"]
  6 -> 7 [label = "\\e"]
  7 -> 1 [label = "\\e"]
}''')

# Z: "<12>\e+<23>a+<34>b".
check('(?@lan_char(ab)_z)<12>\e+<23>a+<34>b',
r'''digraph
{
  vcsn_context = "lan<lal_char(ab)>_z"
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
    3
    4
    5
    6
    7
  }
  I0 -> 0
  0 -> 2 [label = "\\e"]
  0 -> 4 [label = "\\e"]
  0 -> 6 [label = "\\e"]
  1 -> F1
  2 -> 3 [label = "<12>\\e"]
  3 -> 1 [label = "\\e"]
  4 -> 5 [label = "<23>a"]
  5 -> 1 [label = "\\e"]
  6 -> 7 [label = "<34>b"]
  7 -> 1 [label = "\\e"]
}''')

# left weight.
check('(?@lan_char(ab)_z)<12>(\e+a+<10>b+<10>\e)',
r'''digraph
{
  vcsn_context = "lan<lal_char(ab)>_z"
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
    3
    4
    5
    6
    7
    8
    9
  }
  I0 -> 0
  0 -> 2 [label = "<12>\\e"]
  0 -> 4 [label = "<12>\\e"]
  0 -> 6 [label = "<12>\\e"]
  0 -> 8 [label = "<12>\\e"]
  1 -> F1
  2 -> 3 [label = "\\e"]
  3 -> 1 [label = "\\e"]
  4 -> 5 [label = "a"]
  5 -> 1 [label = "\\e"]
  6 -> 7 [label = "<10>b"]
  7 -> 1 [label = "\\e"]
  8 -> 9 [label = "<10>\\e"]
  9 -> 1 [label = "\\e"]
}''')

# right weight.
check('(?@lan_char(ab)_z)(\e+a+<2>b+<3>\e)<10>',
r'''digraph
{
  vcsn_context = "lan<lal_char(ab)>_z"
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
    3
    4
    5
    6
    7
    8
    9
  }
  I0 -> 0
  0 -> 2 [label = "\\e"]
  0 -> 4 [label = "\\e"]
  0 -> 6 [label = "\\e"]
  0 -> 8 [label = "\\e"]
  1 -> F1
  2 -> 3 [label = "\\e"]
  3 -> 1 [label = "<10>\\e"]
  4 -> 5 [label = "a"]
  5 -> 1 [label = "<10>\\e"]
  6 -> 7 [label = "<2>b"]
  7 -> 1 [label = "<10>\\e"]
  8 -> 9 [label = "<3>\\e"]
  9 -> 1 [label = "<10>\\e"]
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
  0 -> 1 [label = "\\e"]
  1 -> F1
}''')

check('(?@lan_char(ab)_b)\e*',
r'''digraph
{
  vcsn_context = "lan<lal_char(ab)>_b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I2
    F3
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
    3
  }
  I2 -> 2
  0 -> 1 [label = "\\e"]
  1 -> 0 [label = "\\e"]
  1 -> 3 [label = "\\e"]
  2 -> 0 [label = "\\e"]
  2 -> 3 [label = "\\e"]
  3 -> F3
}''')

check('(?@lan_char(ab)_z)(<2>a)*',
r'''digraph
{
  vcsn_context = "lan<lal_char(ab)>_z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I2
    F3
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
    3
  }
  I2 -> 2
  0 -> 1 [label = "<2>a"]
  1 -> 0 [label = "\\e"]
  1 -> 3 [label = "\\e"]
  2 -> 0 [label = "\\e"]
  2 -> 3 [label = "\\e"]
  3 -> F3
}''')

check('(?@lan_char(ab)_z)<2>a*<3>',
r'''digraph
{
  vcsn_context = "lan<lal_char(ab)>_z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I2
    F3
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
    3
  }
  I2 -> 2
  0 -> 1 [label = "a"]
  1 -> 0 [label = "\\e"]
  1 -> 3 [label = "<3>\\e"]
  2 -> 0 [label = "<2>\\e"]
  2 -> 3 [label = "<6>\\e"]
  3 -> F3
}''')

check('(?@lan_char(ab)_z)(<2>a+<3>b)*',
r'''digraph
{
  vcsn_context = "lan<lal_char(ab)>_z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I6
    F7
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
    3
    4
    5
    6
    7
  }
  I6 -> 6
  0 -> 2 [label = "\\e"]
  0 -> 4 [label = "\\e"]
  1 -> 0 [label = "\\e"]
  1 -> 7 [label = "\\e"]
  2 -> 3 [label = "<2>a"]
  3 -> 1 [label = "\\e"]
  4 -> 5 [label = "<3>b"]
  5 -> 1 [label = "\\e"]
  6 -> 0 [label = "\\e"]
  6 -> 7 [label = "\\e"]
  7 -> F7
}''')

check('(?@lan_char(ab)_z)<2>(<3>a+<5>b)*<7>',
r'''digraph
{
  vcsn_context = "lan<lal_char(ab)>_z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I6
    F7
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
    3
    4
    5
    6
    7
  }
  I6 -> 6
  0 -> 2 [label = "\\e"]
  0 -> 4 [label = "\\e"]
  1 -> 0 [label = "\\e"]
  1 -> 7 [label = "<7>\\e"]
  2 -> 3 [label = "<3>a"]
  3 -> 1 [label = "\\e"]
  4 -> 5 [label = "<5>b"]
  5 -> 1 [label = "\\e"]
  6 -> 0 [label = "<2>\\e"]
  6 -> 7 [label = "<14>\\e"]
  7 -> F7
}''')

check('(?@lan_char(ab)_z)<2>(<3>(ab)<5>)*<7>',
r'''digraph
{
  vcsn_context = "lan<lal_char(ab)>_z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I4
    F5
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
    3
    4
    5
  }
  I4 -> 4
  0 -> 1 [label = "<3>a"]
  1 -> 2 [label = "\\e"]
  2 -> 3 [label = "<5>b"]
  3 -> 0 [label = "\\e"]
  3 -> 5 [label = "<7>\\e"]
  4 -> 0 [label = "<2>\\e"]
  4 -> 5 [label = "<14>\\e"]
  5 -> F5
}''')

check('(?@lan_char(ab)_z)a**',
r'''digraph
{
  vcsn_context = "lan<lal_char(ab)>_z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I4
    F5
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
    3
    4
    5
  }
  I4 -> 4
  0 -> 1 [label = "a"]
  1 -> 0 [label = "\\e"]
  1 -> 3 [label = "\\e"]
  2 -> 0 [label = "\\e"]
  2 -> 3 [label = "\\e"]
  3 -> 2 [label = "\\e"]
  3 -> 5 [label = "\\e"]
  4 -> 2 [label = "\\e"]
  4 -> 5 [label = "\\e"]
  5 -> F5
}''')

## ---------- ##
## ZR: star.  ##
## ---------- ##

check('(?@lan_char(abcd)_ratexpset<lal_char(efgh)_z>)(<e>\e+abc)*',
r'''digraph
{
  vcsn_context = "lan<lal_char(abcd)>_ratexpset<lal_char(efgh)_z>"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I10
    F11
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
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
  I10 -> 10
  0 -> 2 [label = "\\e"]
  0 -> 4 [label = "\\e"]
  1 -> 0 [label = "\\e"]
  1 -> 11 [label = "\\e"]
  2 -> 3 [label = "<e>\\e"]
  3 -> 1 [label = "\\e"]
  4 -> 5 [label = "a"]
  5 -> 6 [label = "\\e"]
  6 -> 7 [label = "b"]
  7 -> 8 [label = "\\e"]
  8 -> 9 [label = "c"]
  9 -> 1 [label = "\\e"]
  10 -> 0 [label = "\\e"]
  10 -> 11 [label = "\\e"]
  11 -> F11
}''')

check('(?@lan_char(abcd)_ratexpset<lal_char(efgh)_z>)(<e>\e+ab<f>)*',
r'''digraph
{
  vcsn_context = "lan<lal_char(abcd)>_ratexpset<lal_char(efgh)_z>"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I8
    F9
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
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
  I8 -> 8
  0 -> 2 [label = "\\e"]
  0 -> 4 [label = "\\e"]
  1 -> 0 [label = "\\e"]
  1 -> 9 [label = "\\e"]
  2 -> 3 [label = "<e>\\e"]
  3 -> 1 [label = "\\e"]
  4 -> 5 [label = "a"]
  5 -> 6 [label = "\\e"]
  6 -> 7 [label = "<f>b"]
  7 -> 1 [label = "\\e"]
  8 -> 0 [label = "\\e"]
  8 -> 9 [label = "\\e"]
  9 -> F9
}''')

# Make sure that the initial weight of the rhs of the concatenation is
# properly handled.
check('(?@lan_char(a)_ratexpset<lal_char(xyz)_z>)<x>a(<y>\e+<z>a)',
r'''digraph
{
  vcsn_context = "lan<lal_char(a)>_ratexpset<lal_char(xyz)_z>"
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
    5
    6
    7
  }
  I0 -> 0
  0 -> 1 [label = "<x>a"]
  1 -> 2 [label = "\\e"]
  2 -> 4 [label = "\\e"]
  2 -> 6 [label = "\\e"]
  3 -> F3
  4 -> 5 [label = "<y>\\e"]
  5 -> 3 [label = "\\e"]
  6 -> 7 [label = "<z>a"]
  7 -> 3 [label = "\\e"]
}''')
