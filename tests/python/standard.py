#! /usr/bin/env python

import re
import vcsn
from test import *

ctx = vcsn.context('lal_char(ab), b')

## --------------- ##
## standard(aut).  ##
## --------------- ##

# Automata:
#
# 1. Try to be exhaustive: Several initials states, with weights, one
# of which is final, the other has a loop.
#
# 2. Make sure we deleted former initial states that become
# inaccessible.
#
# 3. This one shows we need to use add_transition, it aborts with
# new_transition.
for fs in ['1', '2', '3']:
    a = meaut(fs, 'in.gv')
    exp = metext(fs, 'out.gv')
    CHECK_EQ(exp, a.standard())
    CHECK_EQ(a.transpose().standard().transpose(), a.costandard())
    CHECK_EQ(a.transpose().is_standard(), a.is_costandard())


## --------------- ##
## standard(exp).  ##
## --------------- ##

def check(r, exp=None, file=None):
    if not isinstance(r, vcsn.expression):
        r = ctx.expression(r)
    print("check: {:u}".format(r))
    if file:
        exp = metext(file, 'gv')

    # Check inductive, standard flavor.
    a = r.inductive('standard')
    CHECK_EQ(exp, a)
    CHECK(a.is_standard(),
          'automaton for {} is not standard: {}'.format(r, a))

    # Check that we are equivalent to derived-term.  However,
    # derived-term sometimes needs a neutral to compute ldivide/rdivide.
    a_dt = r.automaton('expansion')
    CHECK_EQUIV(a, a_dt)

    if r.is_extended():
        XFAIL(lambda: r.standard())
    else:
        # Check that standard computes the same automaton.  Well, not
        # the same state numbers though: `standard` leaves gaps.
        CHECK_ISOMORPHIC(a, r.standard())


def xfail(re):
    'An invalid expression.'

    r = ctx.expression(re)
    XFAIL(lambda: r.standard())
    XFAIL(lambda: r.inductive('standard'))


## --- ##
## B.  ##
## --- ##

# B: "\z".
check(r'(?@lal_char(ab), b)\z',
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
check(r'(?@lal_char(ab), b)\e',
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
    F2
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> F1
  2 -> F2
}''')

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
  1 -> 2 [label = "b"]
  2 -> 3 [label = "c"]
  3 -> F3
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
    F2
    F4
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
  0 -> 3 [label = "c"]
  1 -> 2 [label = "b"]
  2 -> F2
  3 -> 4 [label = "d"]
  4 -> F4
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
    2
    3
    4
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  1 -> 2 [label = "b"]
  1 -> 3 [label = "c"]
  2 -> 4 [label = "d"]
  3 -> 4 [label = "d"]
  4 -> F4
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
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 3 [label = "c"]
  0 -> 5 [label = "a"]
  1 -> 2 [label = "b"]
  2 -> 9 [label = "a"]
  3 -> 4 [label = "d"]
  4 -> 9 [label = "a"]
  5 -> 6 [label = "b"]
  6 -> 7 [label = "c"]
  7 -> 8 [label = "d"]
  8 -> 9 [label = "a"]
  9 -> 10 [label = "b"]
  10 -> 11 [label = "c"]
  11 -> F11
}''')


## --------- ##
## B: Star.  ##
## --------- ##

check(r'(?@lal_char(abcd), b)\z*',
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

check(r'(?@lal_char(abcd), b)\e*',
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
    F2
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> F1
  1 -> 1 [label = "a"]
  1 -> 2 [label = "b"]
  2 -> F2
  2 -> 1 [label = "a"]
  2 -> 2 [label = "b"]
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
    F2
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "a"]
  1 -> 2 [label = "b"]
  2 -> F2
  2 -> 1 [label = "a"]
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
check(r'(?@lal_char(a), expressionset<lal_char(xyz), b>)<x>a(<y>\e+<z>a)',
'''digraph
{
  vcsn_context = "letterset<char_letters(a)>, expressionset<letterset<char_letters(xyz)>, b>"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F1
    F2
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
  }
  I0 -> 0
  0 -> 1 [label = "<x>a"]
  1 -> F1 [label = "<y>"]
  1 -> 2 [label = "<z>a"]
  2 -> F2
}''')


## --- ##
## Q.  ##
## --- ##


# Q: "<12>\e".
check(r'(?@lal_char(ab), q)<12>\e',
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, q"
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
## Q: add.  ##
## -------- ##

# Q: "\e+a+\e"
check(r'(?@lal_char(ab), q)\e+a+\e',
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, q"
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
  0 -> F0 [label = "<2>"]
  0 -> 1 [label = "a"]
  1 -> F1
}''')

# Q: "<12>\e+<23>a+<34>b".
check(r'(?@lal_char(ab), q)<12>\e+<23>a+<34>b',
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, q"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F1
    F2
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
  }
  I0 -> 0
  0 -> F0 [label = "<12>"]
  0 -> 1 [label = "<23>a"]
  0 -> 2 [label = "<34>b"]
  1 -> F1
  2 -> F2
}''')

# left weight.
check(r'(?@lal_char(ab), q)<12>(\e+a+<10>b+<10>\e)',
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, q"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F1
    F2
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
  }
  I0 -> 0
  0 -> F0 [label = "<132>"]
  0 -> 1 [label = "<12>a"]
  0 -> 2 [label = "<120>b"]
  1 -> F1
  2 -> F2
}''')

# right weight.
check(vcsn.Q.expression(r'(\e+a+<2>b+<3>\e)<10>', 'associative'),
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, q"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F1
    F2
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
  }
  I0 -> 0
  0 -> F0 [label = "<40>"]
  0 -> 1 [label = "a"]
  0 -> 2 [label = "<2>b"]
  1 -> F1 [label = "<10>"]
  2 -> F2 [label = "<10>"]
}''')

## ------------ ##
## Q: product.  ##
## ------------ ##

# Q: "<12>(ab)<23>".
check(vcsn.Q.expression('<12>(ab)<23>', 'associative'),
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, q"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F2
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
  }
  I0 -> 0
  0 -> 1 [label = "<12>a"]
  1 -> 2 [label = "b"]
  2 -> F2 [label = "<23>"]
}''')

## --------- ##
## Q: star.  ##
## --------- ##

check(r'(?@lal_char(ab), q)\z*',
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, q"
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

xfail(r'(?@lal_char(ab), q)\e*')

check('(?@lal_char(ab), q)(<2>a)*',
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, q"
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

check(vcsn.Q.expression('<2>a*<3>', 'associative'),
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, q"
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

check(vcsn.Q.expression('(<2>a+<3>b)*', 'associative'),
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, q"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F1
    F2
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "<2>a"]
  0 -> 2 [label = "<3>b"]
  1 -> F1
  1 -> 1 [label = "<2>a"]
  1 -> 2 [label = "<3>b"]
  2 -> F2
  2 -> 1 [label = "<2>a"]
  2 -> 2 [label = "<3>b"]
}''')

check(vcsn.Q.expression('<2>(<3>a+<5>b)*<7>', 'associative'),
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, q"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F1
    F2
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
  }
  I0 -> 0
  0 -> F0 [label = "<14>"]
  0 -> 1 [label = "<6>a"]
  0 -> 2 [label = "<10>b"]
  1 -> F1 [label = "<7>"]
  1 -> 1 [label = "<3>a"]
  1 -> 2 [label = "<5>b"]
  2 -> F2 [label = "<7>"]
  2 -> 1 [label = "<3>a"]
  2 -> 2 [label = "<5>b"]
}''')

check(vcsn.Q.expression('<2>(<3>(ab)<5>)*<7>', 'associative'),
'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, q"
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
    1
    2
  }
  I0 -> 0
  0 -> F0 [label = "<14>"]
  0 -> 1 [label = "<6>a"]
  1 -> 2 [label = "b"]
  2 -> F2 [label = "<35>"]
  2 -> 1 [label = "<15>a"]
}''')

xfail('(?@lal_char(ab), q)a**')

## ---------- ##
## QR: star.  ##
## ---------- ##

check(r'(?@lal_char(abcd), expressionset<lal_char(efgh), q>)(<e>\e+abc)*',
'''digraph
{
  vcsn_context = "letterset<char_letters(abcd)>, expressionset<letterset<char_letters(efgh)>, q>"
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
    2
    3
  }
  I0 -> 0
  0 -> F0 [label = "<e*>"]
  0 -> 1 [label = "<e*>a"]
  1 -> 2 [label = "b"]
  2 -> 3 [label = "c"]
  3 -> F3 [label = "<e*>"]
  3 -> 1 [label = "<e*>a"]
}''')

check(r'(?@lal_char(abcd), expressionset<lal_char(efgh), q>)(<e>\e+(ab)<f>)*',
'''digraph
{
  vcsn_context = "letterset<char_letters(abcd)>, expressionset<letterset<char_letters(efgh)>, q>"
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
    1
    2
  }
  I0 -> 0
  0 -> F0 [label = "<e*>"]
  0 -> 1 [label = "<e*>a"]
  1 -> 2 [label = "b"]
  2 -> F2 [label = "<fe*>"]
  2 -> 1 [label = "<fe*>a"]
}''')


# Extended operators.
def exp(e, ctx):
    return ctx.expression(e)

def qexp(e, gens=None):
    if gens:
        c = vcsn.context('lal({}), q'.format(gens))
    else:
        c = vcsn.context('lal, q')
    return exp(e, c)

def q2(e):
    return exp(e, vcsn.context('lat<lal, lal>, q'))

zero = lambda gens: qexp(r'\z', gens).standard()

# Conjunction.
check(qexp('a*b*c* & abc*'), file='conjunction-1')
check(qexp('[ab]*a[ab] & [ab]*a[ab]{2} & [ab]*a[ab]{3}'),
      file='conjunction-2')
# Make sure we preserve standardness.
check(qexp(r'\e & a'), zero('a'))

# Shuffle and infiltrate.
check(qexp('ab : cd : ef'), file='shuffle-1')
check(qexp('ab &: ab &: ab'), file='infiltrate-1')

# Complement.
check(qexp('! [ab]*a[ab]{2}'), file='complement-1')
check(qexp('[abc]*{c}'), zero('abc'))

# Transposition.
check(qexp('abcd & (dcba){T}'), file='transposition-1')
check(qexp('(a* & a*a*){T}'), file='transposition-5')
check(qexp('([ab]*[bc]* & [bc]*[ab]*){T}'), file='transposition-2')
check(qexp('(ab : cd){T}'), file='transposition-3')
check(qexp('(ab &: ab){T}'), file='transposition-4')

# Divide.
check(qexp(r'<2>abc {\} <4>abcd*'), file='ldivide-1')
check(qexp(r'a? {\} a'), file='ldivide-2')
check(qexp(r'(a* {\} a) {\} \e'), file='ldivide-3')
check(qexp('<4>a*bcd {/} <2>bcd'), file='rdivide-1')

# Tuple.
def tuple(*exps):
    return vcsn.expression._tuple([vcsn.context('lal, q').expression(e)
                                   for e in exps])
check(tuple('abc', 'xyz'), file='tuple-1')
check(tuple('abc', 'xy'),  file='tuple-2')
check(tuple('ab*', 'cd*', 'ef*'), file='tuple-3')

# Compose.
def q2(e):
    return exp(e, vcsn.context('lat<lal, lal>, q'))
check(q2('[abc]|[xyz] @ [xz]|Y'), file='compose-1')

# Transpose.
check(q2('((a|x) (b|y) @ (x|i) (y|j)){t}'), file='transpose-1')
