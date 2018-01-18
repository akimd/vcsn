#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context('lal(ab), b')

def check(re, exp):
    # We compare automata as strings, since when parsing the expected
    # automaton we drop the hole in the state numbers created by
    # standard.
    if isinstance(re, str):
        re = ctx.expression(re)
    a = re.thompson()
    CHECK_EQ(exp, a)
    CHECK(a.is_normalized())

def xfail(re, err = None):
    r = ctx.expression(re)
    XFAIL(lambda: r.thompson(), err)

# We don't support extended expressions.
xfail(r'a*&b*')
xfail(r'a:b')
xfail(r'a{c}')
xfail(r'a{\}b')
xfail(r'(ab){T}')


## --- ##
## Z.  ##
## --- ##

# Z: "\z".
check(r'(?@lal(), z)\z',
r'''digraph
{
  vcsn_context = "letterset<char_letters()>, z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F1
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [color = DimGray]
    1 [color = DimGray]
  }
  I0 -> 0 [color = DimGray]
  1 -> F1 [color = DimGray]
}''')

# Z: "<12>\e".
check(r'(?@lal(a), z)<12>\e',
r'''digraph
{
  vcsn_context = "letterset<char_letters(a)>, z"
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
## Z: add.  ##
## -------- ##

# Z: "\e+a+\e"
check(r'(?@lal(ab), z)\e+a+\e',
r'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, z"
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
  }
  I0 -> 0
  0 -> 2 [label = "\\e"]
  0 -> 4 [label = "\\e"]
  1 -> F1
  2 -> 3 [label = "<2>\\e"]
  3 -> 1 [label = "\\e"]
  4 -> 5 [label = "a"]
  5 -> 1 [label = "\\e"]
}''')

# Z: "<12>\e+<23>a+<34>b".
check(r'(?@law_char(ab), z)<12>\e+<23>a+<34>b',
r'''digraph
{
  vcsn_context = "wordset<char_letters(ab)>, z"
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
check(r'(?@lal(ab), z)<10>(<2>\e+<3>a+<5>b)',
r'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, z"
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
  0 -> 2 [label = "<10>\\e"]
  0 -> 4 [label = "<10>\\e"]
  0 -> 6 [label = "<10>\\e"]
  1 -> F1
  2 -> 3 [label = "<2>\\e"]
  3 -> 1 [label = "\\e"]
  4 -> 5 [label = "<3>a"]
  5 -> 1 [label = "\\e"]
  6 -> 7 [label = "<5>b"]
  7 -> 1 [label = "\\e"]
}''')

# right weight.
check(vcsn.context('lal(ab), z')
      .expression(r'(<2>\e+<3>a+<5>b)<10>', 'associative'),
r'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, z"
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
  2 -> 3 [label = "<2>\\e"]
  3 -> 1 [label = "<10>\\e"]
  4 -> 5 [label = "<3>a"]
  5 -> 1 [label = "<10>\\e"]
  6 -> 7 [label = "<5>b"]
  7 -> 1 [label = "<10>\\e"]
}''')

## ------------ ##
## Z: product.  ##
## ------------ ##

# Z: "<12>(ab)<23>".
check(vcsn.Z.expression('<12>(ab)<23>', 'associative'),
r'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, z"
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

check(r'(?@lal(ab), z)\z*',
r'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, z"
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

check(r'(?@lal(ab), b)\e*',
r'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, b"
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

check('(?@lal(ab), z)(<2>a)*',
r'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, z"
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

check(vcsn.context('lal(ab), z').expression('<2>a*<3>', 'associative'),
r'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, z"
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

check('(?@lal(ab), z)(<2>a+<3>b)*',
r'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, z"
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

check(vcsn.context('lal(ab), z')
      .expression('<2>(<3>a+<5>b)*<7>', 'associative'),
r'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, z"
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

check(vcsn.context('lal(ab), z')
      .expression('<2>(<3>(ab)<5>)*<7>', 'associative'),
r'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, z"
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

check('(?@lal(ab), z)a**',
r'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, z"
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

check(r'(?@lal(abcd), expressionset<lal(efgh), q>)(<e>\e+abc)*',
r'''digraph
{
  vcsn_context = "letterset<char_letters(abcd)>, expressionset<letterset<char_letters(efgh)>, q>"
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

check(r'(?@lal(abcd), expressionset<lal(efgh), q>)(<e>\e+ab<f>)*',
r'''digraph
{
  vcsn_context = "letterset<char_letters(abcd)>, expressionset<letterset<char_letters(efgh)>, q>"
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
check(r'(?@lal(a), expressionset<lal(xyz), q>)<x>a(<y>\e+<z>a)',
r'''digraph
{
  vcsn_context = "letterset<char_letters(a)>, expressionset<letterset<char_letters(xyz)>, q>"
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
