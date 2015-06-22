#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context('lal_char(ab), b')

## --------------- ##
## standard(aut).  ##
## --------------- ##

# Try to be exhaustive: Several initials states, with weights, one of
# which is final, the other has a loop.
a = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab), q"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    I2
    F
  }
  {
    node [shape = circle]
    0
    1
    2
  }
  I0 -> 0 [label = "<1/2>"]
  I2 -> 2 [label = "<1/4>"]
  0 -> 0 [label = "a, <2>b"]
  0 -> 1 [label = "<3>a"]
  1 -> 2 [label = "b"]
  2 -> 2 [label = "a, b"]
  2 -> F
}''')

exp = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab), q"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I3
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
  I3 -> 3
  0 -> 0 [label = "a, <2>b"]
  0 -> 1 [label = "<3>a"]
  1 -> 2 [label = "b"]
  2 -> F2
  2 -> 2 [label = "a, b"]
  3 -> F3 [label = "<1/4>"]
  3 -> 0 [label = "<1/2>a, b"]
  3 -> 1 [label = "<3/2>a"]
  3 -> 2 [label = "<1/4>a, <1/4>b"]
}''')
CHECK_EQ(exp, a.standard())
CHECK_EQ(a.transpose().standard().transpose(), a.costandard())
CHECK_EQ(a.transpose().is_standard(), a.is_costandard())

# Make sure we deleted former initial states that become inaccessible.
a = vcsn.automaton('''digraph
{
 vcsn_context = "lal_char(a), expressionset<lal_char(xyz), b>"
 rankdir = LR
 {
   node [shape = point, width = 0]
   I0
   F0
   F1
 }
 {
   node [shape = circle]
   0
 }
 I0 -> 0 [label = "<x>"]
 0 -> F0 [label = "<y>"]
}
''')

exp = '''digraph
{
  vcsn_context = "letterset<char_letters(a)>, expressionset<letterset<char_letters(xyz)>, b>"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I1
    F1
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    1
  }
  I1 -> 1
  1 -> F1 [label = "<xy>"]
}'''
CHECK_EQ(exp, str(a.standard()))
CHECK_EQ(a.transpose().standard().transpose(), a.costandard())
CHECK_EQ(a.transpose().is_standard(), a.is_costandard())


## --------------- ##
## standard(exp).  ##
## --------------- ##

def check(re, exp):
    # We compare automata as strings, since when parsing the expected
    # automaton we drop the hole in the state numbers created by
    # standard.
    if isinstance(re, str):
        re = ctx.expression(re)
    a = re.standard()
    CHECK_EQ(exp, str(a))
    CHECK(a.is_standard())

def xfail(re):
    r = ctx.expression(re)
    XFAIL(lambda: r.standard())

## --- ##
## B.  ##
## --- ##

# B: "\z".
check('(?@lal_char(ab), b)\z',
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [color = DimGray]
  }
  I0 -> 0 [color = DimGray]
}''')

# B: "\e".
check('(?@lal_char(ab), b)\e',
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
  }
  I0 -> 0
  0 -> F0
}''')

# B: "a"
check('(?@lal_char(ab), b)a',
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, b"
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

# B: "a+b"
check('(?@lal_char(ab), b)a+b',
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F1
    F3
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    3
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 3 [label = "b"]
  1 -> F1
  3 -> F3
}''')

# B: conjunction.
xfail('(?@lal_char(abc), b)a*&b*')

# B: "abc".
check('(?@lal_char(abc), b)abc',
'''digraph
{
  vcsn_context = "letterset<char_letters(abc)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F4
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    3
    4
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  1 -> 3 [label = "b"]
  3 -> 4 [label = "c"]
  4 -> F4
}''')

# B: "ab+cd".
check('(?@lal_char(abcd), b)ab+cd',
'''digraph
{
  vcsn_context = "letterset<char_letters(abcd)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F3
    F6
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    3
    4
    6
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 4 [label = "c"]
  1 -> 3 [label = "b"]
  3 -> F3
  4 -> 6 [label = "d"]
  6 -> F6
}''')

# B: "a(b+c)d".
check('(?@lal_char(abcd), b)a(b+c)d',
'''digraph
{
  vcsn_context = "letterset<char_letters(abcd)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F4
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    3
    4
    5
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  1 -> 3 [label = "b"]
  1 -> 5 [label = "c"]
  3 -> 4 [label = "d"]
  4 -> F4
  5 -> 4 [label = "d"]
}''')

# B: "(ab+cd+abcd)abc".
check('(?@lal_char(abcd), b)(ab+cd+abcd)abc',
'''digraph
{
  vcsn_context = "letterset<char_letters(abcd)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F12
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    3
    4
    5
    6
    7
    8
    9
    10
    11
    12
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 4 [label = "c"]
  0 -> 5 [label = "a"]
  1 -> 3 [label = "b"]
  3 -> 7 [label = "a"]
  4 -> 6 [label = "d"]
  5 -> 8 [label = "b"]
  6 -> 7 [label = "a"]
  7 -> 11 [label = "b"]
  8 -> 9 [label = "c"]
  9 -> 10 [label = "d"]
  10 -> 7 [label = "a"]
  11 -> 12 [label = "c"]
  12 -> F12
}''')


## --------- ##
## B: Star.  ##
## --------- ##

check('(?@lal_char(abcd), b)\z*',
'''digraph
{
  vcsn_context = "letterset<char_letters(abcd)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
  }
  I0 -> 0
  0 -> F0
}''')

check('(?@lal_char(abcd), b)\e*',
'''digraph
{
  vcsn_context = "letterset<char_letters(abcd)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
  }
  I0 -> 0
  0 -> F0
}''')

check('(?@lal_char(abcd), b)a*',
'''digraph
{
  vcsn_context = "letterset<char_letters(abcd)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F1
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "a"]
  1 -> F1
  1 -> 1 [label = "a"]
}''')

check('(?@lal_char(abcd), b)(a+b)*',
'''digraph
{
  vcsn_context = "letterset<char_letters(abcd)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F1
    F3
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    3
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "a"]
  0 -> 3 [label = "b"]
  1 -> F1
  1 -> 1 [label = "a"]
  1 -> 3 [label = "b"]
  3 -> F3
  3 -> 1 [label = "a"]
  3 -> 3 [label = "b"]
}''')

check('(?@lal_char(abcd), b)(ab)*',
'''digraph
{
  vcsn_context = "letterset<char_letters(abcd)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F3
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    3
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "a"]
  1 -> 3 [label = "b"]
  3 -> F3
  3 -> 1 [label = "a"]
}''')

check('(?@lal_char(abcd), b)a**',
'''digraph
{
  vcsn_context = "letterset<char_letters(abcd)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F1
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "a"]
  1 -> F1
  1 -> 1 [label = "a"]
}''')

## ---- ##
## BR.  ##
## ---- ##

# Make sure that the initial weight of the rhs of the concatenation is
# properly handled.
check('(?@lal_char(a), expressionset<lal_char(xyz), b>)<x>a(<y>\e+<z>a)',
'''digraph
{
  vcsn_context = "letterset<char_letters(a)>, expressionset<letterset<char_letters(xyz)>, b>"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F1
    F4
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    4
  }
  I0 -> 0
  0 -> 1 [label = "<x>a"]
  1 -> F1 [label = "<y>"]
  1 -> 4 [label = "<z>a"]
  4 -> F4
}''')


## --- ##
## Z.  ##
## --- ##


# Z: "<12>\e".
check('(?@lal_char(ab), z)<12>\e',
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
  }
  I0 -> 0
  0 -> F0 [label = "<12>"]
}''')

## -------- ##
## Z: sum.  ##
## -------- ##

# Z: "\e+a+\e"
check('(?@lal_char(ab), z)\e+a+\e',
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F2
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    2
  }
  I0 -> 0
  0 -> F0 [label = "<2>"]
  0 -> 2 [label = "a"]
  2 -> F2
}''')

# Z: "<12>\e+<23>a+<34>b".
check('(?@lal_char(ab), z)<12>\e+<23>a+<34>b',
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F2
    F3
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    2
    3
  }
  I0 -> 0
  0 -> F0 [label = "<12>"]
  0 -> 2 [label = "<23>a"]
  0 -> 3 [label = "<34>b"]
  2 -> F2
  3 -> F3
}''')

# left weight.
check('(?@lal_char(ab), z)<12>(\e+a+<10>b+<10>\e)',
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F2
    F3
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    2
    3
  }
  I0 -> 0
  0 -> F0 [label = "<132>"]
  0 -> 2 [label = "<12>a"]
  0 -> 3 [label = "<120>b"]
  2 -> F2
  3 -> F3
}''')

# right weight.
check(vcsn.Z.expression('(\e+a+<2>b+<3>\e)<10>', 'associative'),
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F2
    F3
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    2
    3
  }
  I0 -> 0
  0 -> F0 [label = "<40>"]
  0 -> 2 [label = "a"]
  0 -> 3 [label = "<2>b"]
  2 -> F2 [label = "<10>"]
  3 -> F3 [label = "<10>"]
}''')

## ------------ ##
## Z: product.  ##
## ------------ ##

# Z: "<12>(ab)<23>".
check(vcsn.Z.expression('<12>(ab)<23>', 'associative'),
'''digraph
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
    3
  }
  I0 -> 0
  0 -> 1 [label = "<12>a"]
  1 -> 3 [label = "b"]
  3 -> F3 [label = "<23>"]
}''')

## --------- ##
## Z: star.  ##
## --------- ##

check('(?@lal_char(ab), z)\z*',
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
  }
  I0 -> 0
  0 -> F0
}''')

xfail('(?@lal_char(ab), z)\e*')

check('(?@lal_char(ab), z)(<2>a)*',
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F1
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "<2>a"]
  1 -> F1
  1 -> 1 [label = "<2>a"]
}''')

check(vcsn.Z.expression('<2>a*<3>', 'associative'),
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F1
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
  }
  I0 -> 0
  0 -> F0 [label = "<6>"]
  0 -> 1 [label = "<2>a"]
  1 -> F1 [label = "<3>"]
  1 -> 1 [label = "a"]
}''')

check(vcsn.Z.expression('(<2>a+<3>b)*', 'associative'),
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F1
    F3
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    3
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "<2>a"]
  0 -> 3 [label = "<3>b"]
  1 -> F1
  1 -> 1 [label = "<2>a"]
  1 -> 3 [label = "<3>b"]
  3 -> F3
  3 -> 1 [label = "<2>a"]
  3 -> 3 [label = "<3>b"]
}''')

check(vcsn.Z.expression('<2>(<3>a+<5>b)*<7>', 'associative'),
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F1
    F3
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    3
  }
  I0 -> 0
  0 -> F0 [label = "<14>"]
  0 -> 1 [label = "<6>a"]
  0 -> 3 [label = "<10>b"]
  1 -> F1 [label = "<7>"]
  1 -> 1 [label = "<3>a"]
  1 -> 3 [label = "<5>b"]
  3 -> F3 [label = "<7>"]
  3 -> 1 [label = "<3>a"]
  3 -> 3 [label = "<5>b"]
}''')

check(vcsn.Z.expression('<2>(<3>(ab)<5>)*<7>', 'associative'),
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F3
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    3
  }
  I0 -> 0
  0 -> F0 [label = "<14>"]
  0 -> 1 [label = "<6>a"]
  1 -> 3 [label = "b"]
  3 -> F3 [label = "<35>"]
  3 -> 1 [label = "<15>a"]
}''')

xfail('(?@lal_char(ab), z)a**')

## ---------- ##
## ZR: star.  ##
## ---------- ##

check('(?@lal_char(abcd), expressionset<lal_char(efgh), z>)(<e>\e+abc)*',
'''digraph
{
  vcsn_context = "letterset<char_letters(abcd)>, expressionset<letterset<char_letters(efgh)>, z>"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F5
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    2
    4
    5
  }
  I0 -> 0
  0 -> F0 [label = "<e*>"]
  0 -> 2 [label = "<e*>a"]
  2 -> 4 [label = "b"]
  4 -> 5 [label = "c"]
  5 -> F5 [label = "<e*>"]
  5 -> 2 [label = "<e*>a"]
}''')

check('(?@lal_char(abcd), expressionset<lal_char(efgh), z>)(<e>\e+(ab)<f>)*',
'''digraph
{
  vcsn_context = "letterset<char_letters(abcd)>, expressionset<letterset<char_letters(efgh)>, z>"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F4
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    2
    4
  }
  I0 -> 0
  0 -> F0 [label = "<e*>"]
  0 -> 2 [label = "<e*>a"]
  2 -> 4 [label = "b"]
  4 -> F4 [label = "<fe*>"]
  4 -> 2 [label = "<fe*>a"]
}''')
