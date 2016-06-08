#! /usr/bin/env python

import vcsn
from test import *

def aut(ctx, exp, algo):
    if isinstance(ctx, str):
        ctx = vcsn.context(ctx)
    return ctx.expression(exp).automaton(algo)

def std(ctx, exp, algo='standard'):
    return aut(ctx, exp, algo)

def dt(ctx, exp):
    return std(ctx, exp, 'expansion')

## ---------------------- ##
## Existing transitions.  ##
## ---------------------- ##

# See the actual code of conjunction to understand the point of this test
# (which is new_transition vs. add_transition).
a1 = dt('lal_char(abcd), b', 'a*a')
a2 = a1 & a1
CHECK_EQ('a*a', a2.expression())

## ---------------------- ##
## (a+b)* & (b+c)* = b*.  ##
## ---------------------- ##

lhs = std('lal_char(ab), b', '(a+b)*')
rhs = std('lal_char(bc), b', '(b+c)*')
CHECK_EQ('''digraph
{
  vcsn_context = "letterset<char_letters(b)>, b"
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
    0 [label = "0, 0", shape = box]
    1 [label = "3, 1", shape = box]
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "b"]
  1 -> F1
  1 -> 1 [label = "b"]
}''',
         lhs & rhs)

## ------------- ##
## ab & cd = 0.  ##
## ------------- ##

lhs = std('lal_char(ab), b', 'ab')
rhs = std('lal_char(cd), b', 'cd')
CHECK_EQ('''digraph
{
  vcsn_context = "letterset<char_letters()>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "0, 0", shape = box, color = DimGray]
  }
  I0 -> 0 [color = DimGray]
}''', lhs & rhs)



## ---------------------- ##
## (a+b)* & (c+d)* = \e.  ##
## ---------------------- ##

lhs = std('lal_char(ab), b', '(a+b)*')
rhs = std('lal_char(cd), b', '(c+d)*')
CHECK_EQ('''digraph
{
  vcsn_context = "letterset<char_letters()>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "0, 0", shape = box]
  }
  I0 -> 0
  0 -> F0
}''',
         lhs & rhs)



## ------------ ##
## lal_char_z.  ##
## ------------ ##

# <2>(a*b*a*)
lhs = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abc), z"
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
    0 [label = "0, 0", shape = box, style = rounded]
    1 [label = "3, 1", shape = box, style = rounded]
    2
  }
  I0 -> 0 [label = "<2>"]
  0 -> F0
  0 -> 0 [label = "a"]
  0 -> 1 [label = "b"]
  0 -> 2 [label = "a"]
  1 -> F1
  1 -> 1 [label = "b"]
  1 -> 2 [label = "a"]
  2 -> F2
  2 -> 2 [label = "a"]
}
''')

# (<3>(ab))*
rhs = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abc), z"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F0
  }
  {
    node [shape = circle]
    0 [label = "0, 0", shape = box, style = rounded]
    1 [label = "3, 1", shape = box, style = rounded]
  }
  I0 -> 0 [label = "<3>"]
  0 -> F0
  0 -> 1 [label = "a"]
  1 -> 0 [label = "<3>b"]
}
''')

exp = '''digraph
{
  vcsn_context = "letterset<char_letters(abc)>, z"
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
    0 [label = "0, 0", shape = box]
    1 [label = "0, 1", shape = box]
    2 [label = "2, 1", shape = box, color = DimGray]
    3 [label = "1, 0", shape = box]
  }
  I0 -> 0 [label = "<6>"]
  0 -> F0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "a", color = DimGray]
  1 -> 3 [label = "<3>b"]
  3 -> F3
  3 -> 2 [label = "a", color = DimGray]
}'''

CHECK_EQ(exp, lhs & rhs)

## ------------------------------------ ##
## Heterogeneous (and variadic) input.  ##
## ------------------------------------ ##

# check RES AUT
# -------------
def check(exp, aut):
    CHECK_EQ(exp, aut.expression())

# RatE and B, in both directions.
a1 = std('lal_char(ab), seriesset<lal_char(uv), q>', '(<u>a+<v>b)*')
a2 = std('lal_char(ab), b', 'a{+}')
check(r'<u>a(\e+<u>a(<u>a)*)', a1 & a2)
check(r'<u>a(\e+<u>a(<u>a)*)', a2 & a1)

# Z, Q, R.
z = dt('lal_char(ab), z', '(<2>a+<3>b)*')
q = dt('lal_char(ab), q', '(<1/2>a+<1/3>b)*')
r = dt('lal_char(ab), r', '(<.2>a+<.3>b)*')

check('(a+b)*', z & q)
check('(a+b)*', q & z)
check('(<2>a+<3>b)*', z & q & z)
check('(<1/2>a+<1/3>b)*', z & q & q)

check('(<0.4>a+<0.9>b)*', z & r)
check('(<0.4>a+<0.9>b)*', r & z)

check('(<0.1>a+<0.1>b)*', q & r)
check('(<0.1>a+<0.1>b)*', r & q)


## ----------------- ##
## Non-commutative.  ##
## ----------------- ##

a1 = std('lal_char(ab), seriesset<lal_char(uv), q>',
         '<u>a<v>b')
a2 = std('lal_char(ab), seriesset<lal_char(xy), q>',
         '<x>a<y>b')

def check_enumerate(exp, aut):
    CHECK_EQ(exp, aut.strip().shortest(len=4))

check_enumerate('<uxvy>ab', a1 & a2)
check_enumerate(r'\z', a1.transpose() & a2)
check_enumerate(r'\z', a1 & a2.transpose())
check_enumerate('<vyux>ba', a1.transpose() & a2.transpose())


## ---------- ##
## Variadic.  ##
## ---------- ##

# unary case: return only the accessible part.
CHECK_EQ('''digraph
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
    0 [label = "0", shape = box, color = DimGray]
    1 [label = "1", shape = box, color = DimGray]
    2 [label = "2", shape = box, color = DimGray]
  }
  I0 -> 0 [color = DimGray]
  0 -> 1 [label = "a", color = DimGray]
  1 -> 2 [label = "a", color = DimGray]
}''', vcsn.automaton('''
digraph
{
  vcsn_context = "letterset<char_letters(ab)>, b"
  I -> 0
  0 -> 1 [label = "a"]
  1 -> 2 [label = "a"]
  3 -> 0 [label = "b"]
  3 -> 1 [label = "b"]
  3 -> 2 [label = "b"]
  3 -> F
}
''').conjunction())

# Four arguments.
ctx = vcsn.context('lal_char(x), seriesset<lal_char(abcd), q>')
a = dict()
for l in ['a', 'b', 'c', 'd']:
    a[l] = std(ctx, '<{}>x'.format(l))
check_enumerate('<abcd>x', a['a'] & a['b'] & a['c'] & a['d'])


## ------------------------- ##
## expression & expression.  ##
## ------------------------- ##

# Add stars (<u>a*, not <u>a) to avoid that the trivial identities
# (a&b -> \z) fire and yield a global \z.
qr = vcsn.context('lal_char(a), seriesset<lal_char(uv), q>') \
         .expression('<u>a*')
z = vcsn.context('lal_char(b), z').expression('<2>b*')
q = vcsn.context('lal_char(c), q').expression('<1/3>c*')
r = vcsn.context('lal_char(d), r').expression('<.4>d*')
CHECK_EQ(r'<u>a*&<<2>\e>b*&<<0.333333>\e>c*&<<0.4>\e>d*', qr & z & q & r)

## ------------------------- ##
## polynomial & polynomial.  ##
## ------------------------- ##

poly = vcsn.context('lal_char(abc), q').polynomial
a = poly('<2>a+<3>b')
b = poly('<2>b+<3>c')
c = poly('<3>c')
CHECK_EQ('<4>a + <9>b', a & a)
CHECK_EQ('<6>b', a & b)
CHECK_EQ('<6>b', b & a)
CHECK_EQ(r'\z', a & c)
CHECK_EQ('<9>c', b & c)

poly = vcsn.context('lan_char, q').polynomial
a = poly(r'<2>\e')
b = poly(r'<2>a+<3>\e')
c = poly('<2>a+<3>c')
CHECK_EQ(r'<4>\e', a & a)
CHECK_EQ(r'\z', a & c)
CHECK_EQ(r'<6>\e', a & b)
CHECK_EQ('<4>a', b & c)

poly = vcsn.context('law_char, q').polynomial
a = poly('<1>aa')
b = poly('<2>ab')
CHECK_EQ(r'\z', a & b)
CHECK_EQ('aa', a & a)

## ----------------- ##
## nullable labels.  ##
## ----------------- ##

lhs = aut('lan_char(ab), b', '(a+b)*', 'thompson')
rhs = aut('lan_char(bc), b', '(b+c)*', 'thompson')
res = r'''digraph
{
  vcsn_context = "nullableset<letterset<char_letters(b)>>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F6
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "6, (6, !\\e)", shape = box]
    1 [label = "0, (6, !\\e)", shape = box]
    2 [label = "7, (6, !\\e)", shape = box]
    3 [label = "2, (6, !\\e)", shape = box, color = DimGray]
    4 [label = "4, (6, !\\e)", shape = box]
    5 [label = "7, (0, \\e)", shape = box, color = DimGray]
    6 [label = "7, (7, \\e)", shape = box]
    7 [label = "2, (0, \\e)", shape = box, color = DimGray]
    8 [label = "2, (7, \\e)", shape = box, color = DimGray]
    9 [label = "4, (0, \\e)", shape = box]
    10 [label = "4, (7, \\e)", shape = box, color = DimGray]
    11 [label = "7, (2, \\e)", shape = box, color = DimGray]
    12 [label = "7, (4, \\e)", shape = box, color = DimGray]
    13 [label = "2, (2, \\e)", shape = box, color = DimGray]
    14 [label = "2, (4, \\e)", shape = box, color = DimGray]
    15 [label = "4, (2, \\e)", shape = box]
    16 [label = "4, (4, \\e)", shape = box, color = DimGray]
    17 [label = "5, (3, !\\e)", shape = box]
    18 [label = "1, (3, !\\e)", shape = box]
    19 [label = "7, (3, !\\e)", shape = box]
    20 [label = "0, (3, !\\e)", shape = box]
    21 [label = "7, (1, \\e)", shape = box]
    22 [label = "2, (3, !\\e)", shape = box, color = DimGray]
    23 [label = "4, (3, !\\e)", shape = box]
    24 [label = "2, (1, \\e)", shape = box, color = DimGray]
    25 [label = "4, (1, \\e)", shape = box]
  }
  I0 -> 0
  0 -> 1 [label = "\\e"]
  0 -> 2 [label = "\\e"]
  1 -> 3 [label = "\\e", color = DimGray]
  1 -> 4 [label = "\\e"]
  2 -> 5 [label = "\\e", color = DimGray]
  2 -> 6 [label = "\\e"]
  3 -> 7 [label = "\\e", color = DimGray]
  3 -> 8 [label = "\\e", color = DimGray]
  4 -> 9 [label = "\\e"]
  4 -> 10 [label = "\\e", color = DimGray]
  5 -> 11 [label = "\\e", color = DimGray]
  5 -> 12 [label = "\\e", color = DimGray]
  6 -> F6
  7 -> 13 [label = "\\e", color = DimGray]
  7 -> 14 [label = "\\e", color = DimGray]
  9 -> 15 [label = "\\e"]
  9 -> 16 [label = "\\e", color = DimGray]
  15 -> 17 [label = "b"]
  17 -> 18 [label = "\\e"]
  18 -> 19 [label = "\\e"]
  18 -> 20 [label = "\\e"]
  19 -> 21 [label = "\\e"]
  20 -> 22 [label = "\\e", color = DimGray]
  20 -> 23 [label = "\\e"]
  21 -> 5 [label = "\\e", color = DimGray]
  21 -> 6 [label = "\\e"]
  22 -> 24 [label = "\\e", color = DimGray]
  23 -> 25 [label = "\\e"]
  24 -> 7 [label = "\\e", color = DimGray]
  24 -> 8 [label = "\\e", color = DimGray]
  25 -> 9 [label = "\\e"]
  25 -> 10 [label = "\\e", color = DimGray]
}'''
CHECK_EQ(res, lhs & rhs)
CHECK_EQUIV(vcsn.automaton(res),
            std('lal_char(b), b', 'b*'))

third = aut('lan_char(bcd), b', '(b+c+d)*', 'thompson')
res = r'''digraph
{
  vcsn_context = "nullableset<letterset<char_letters(b)>>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F14
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "6, (6, !\\e), (8, !\\e)", shape = box]
    1 [label = "0, (6, !\\e), (8, !\\e)", shape = box]
    2 [label = "7, (6, !\\e), (8, !\\e)", shape = box]
    3 [label = "2, (6, !\\e), (8, !\\e)", shape = box, color = DimGray]
    4 [label = "4, (6, !\\e), (8, !\\e)", shape = box]
    5 [label = "7, (0, \\e), (8, !\\e)", shape = box, color = DimGray]
    6 [label = "7, (7, \\e), (8, !\\e)", shape = box]
    7 [label = "2, (0, \\e), (8, !\\e)", shape = box, color = DimGray]
    8 [label = "2, (7, \\e), (8, !\\e)", shape = box, color = DimGray]
    9 [label = "4, (0, \\e), (8, !\\e)", shape = box]
    10 [label = "4, (7, \\e), (8, !\\e)", shape = box, color = DimGray]
    11 [label = "7, (2, \\e), (8, !\\e)", shape = box, color = DimGray]
    12 [label = "7, (4, \\e), (8, !\\e)", shape = box, color = DimGray]
    13 [label = "7, (7, \\e), (0, \\e)", shape = box, color = DimGray]
    14 [label = "7, (7, \\e), (9, \\e)", shape = box]
    15 [label = "2, (2, \\e), (8, !\\e)", shape = box, color = DimGray]
    16 [label = "2, (4, \\e), (8, !\\e)", shape = box, color = DimGray]
    17 [label = "2, (7, \\e), (0, \\e)", shape = box, color = DimGray]
    18 [label = "2, (7, \\e), (9, \\e)", shape = box, color = DimGray]
    19 [label = "4, (2, \\e), (8, !\\e)", shape = box]
    20 [label = "4, (4, \\e), (8, !\\e)", shape = box, color = DimGray]
    21 [label = "4, (7, \\e), (0, \\e)", shape = box, color = DimGray]
    22 [label = "4, (7, \\e), (9, \\e)", shape = box, color = DimGray]
    23 [label = "7, (2, \\e), (0, \\e)", shape = box, color = DimGray]
    24 [label = "7, (2, \\e), (9, \\e)", shape = box, color = DimGray]
    25 [label = "7, (4, \\e), (0, \\e)", shape = box, color = DimGray]
    26 [label = "7, (4, \\e), (9, \\e)", shape = box, color = DimGray]
    27 [label = "7, (7, \\e), (2, \\e)", shape = box, color = DimGray]
    28 [label = "7, (7, \\e), (4, \\e)", shape = box, color = DimGray]
    29 [label = "7, (7, \\e), (6, \\e)", shape = box, color = DimGray]
    30 [label = "2, (2, \\e), (0, \\e)", shape = box, color = DimGray]
    31 [label = "2, (2, \\e), (9, \\e)", shape = box, color = DimGray]
    32 [label = "2, (4, \\e), (0, \\e)", shape = box, color = DimGray]
    33 [label = "2, (4, \\e), (9, \\e)", shape = box, color = DimGray]
    34 [label = "2, (7, \\e), (2, \\e)", shape = box, color = DimGray]
    35 [label = "2, (7, \\e), (4, \\e)", shape = box, color = DimGray]
    36 [label = "2, (7, \\e), (6, \\e)", shape = box, color = DimGray]
    37 [label = "4, (2, \\e), (0, \\e)", shape = box]
    38 [label = "4, (2, \\e), (9, \\e)", shape = box, color = DimGray]
    39 [label = "4, (4, \\e), (0, \\e)", shape = box, color = DimGray]
    40 [label = "4, (4, \\e), (9, \\e)", shape = box, color = DimGray]
    41 [label = "4, (7, \\e), (2, \\e)", shape = box, color = DimGray]
    42 [label = "4, (7, \\e), (4, \\e)", shape = box, color = DimGray]
    43 [label = "4, (7, \\e), (6, \\e)", shape = box, color = DimGray]
    44 [label = "7, (2, \\e), (2, \\e)", shape = box, color = DimGray]
    45 [label = "7, (2, \\e), (4, \\e)", shape = box, color = DimGray]
    46 [label = "7, (2, \\e), (6, \\e)", shape = box, color = DimGray]
    47 [label = "7, (4, \\e), (2, \\e)", shape = box, color = DimGray]
    48 [label = "7, (4, \\e), (4, \\e)", shape = box, color = DimGray]
    49 [label = "7, (4, \\e), (6, \\e)", shape = box, color = DimGray]
    50 [label = "2, (2, \\e), (2, \\e)", shape = box, color = DimGray]
    51 [label = "2, (2, \\e), (4, \\e)", shape = box, color = DimGray]
    52 [label = "2, (2, \\e), (6, \\e)", shape = box, color = DimGray]
    53 [label = "2, (4, \\e), (2, \\e)", shape = box, color = DimGray]
    54 [label = "2, (4, \\e), (4, \\e)", shape = box, color = DimGray]
    55 [label = "2, (4, \\e), (6, \\e)", shape = box, color = DimGray]
    56 [label = "4, (2, \\e), (2, \\e)", shape = box]
    57 [label = "4, (2, \\e), (4, \\e)", shape = box, color = DimGray]
    58 [label = "4, (2, \\e), (6, \\e)", shape = box, color = DimGray]
    59 [label = "4, (4, \\e), (2, \\e)", shape = box, color = DimGray]
    60 [label = "4, (4, \\e), (4, \\e)", shape = box, color = DimGray]
    61 [label = "4, (4, \\e), (6, \\e)", shape = box, color = DimGray]
    62 [label = "5, (3, !\\e), (3, !\\e)", shape = box]
    63 [label = "1, (3, !\\e), (3, !\\e)", shape = box]
    64 [label = "7, (3, !\\e), (3, !\\e)", shape = box]
    65 [label = "0, (3, !\\e), (3, !\\e)", shape = box]
    66 [label = "7, (1, \\e), (3, !\\e)", shape = box]
    67 [label = "2, (3, !\\e), (3, !\\e)", shape = box, color = DimGray]
    68 [label = "4, (3, !\\e), (3, !\\e)", shape = box]
    69 [label = "7, (7, \\e), (3, !\\e)", shape = box]
    70 [label = "7, (0, \\e), (3, !\\e)", shape = box, color = DimGray]
    71 [label = "2, (1, \\e), (3, !\\e)", shape = box, color = DimGray]
    72 [label = "4, (1, \\e), (3, !\\e)", shape = box]
    73 [label = "7, (7, \\e), (1, \\e)", shape = box]
    74 [label = "7, (2, \\e), (3, !\\e)", shape = box, color = DimGray]
    75 [label = "7, (4, \\e), (3, !\\e)", shape = box, color = DimGray]
    76 [label = "2, (7, \\e), (3, !\\e)", shape = box, color = DimGray]
    77 [label = "2, (0, \\e), (3, !\\e)", shape = box, color = DimGray]
    78 [label = "4, (7, \\e), (3, !\\e)", shape = box, color = DimGray]
    79 [label = "4, (0, \\e), (3, !\\e)", shape = box]
    80 [label = "7, (2, \\e), (1, \\e)", shape = box, color = DimGray]
    81 [label = "7, (4, \\e), (1, \\e)", shape = box, color = DimGray]
    82 [label = "2, (7, \\e), (1, \\e)", shape = box, color = DimGray]
    83 [label = "2, (2, \\e), (3, !\\e)", shape = box, color = DimGray]
    84 [label = "2, (4, \\e), (3, !\\e)", shape = box, color = DimGray]
    85 [label = "4, (7, \\e), (1, \\e)", shape = box, color = DimGray]
    86 [label = "4, (2, \\e), (3, !\\e)", shape = box]
    87 [label = "4, (4, \\e), (3, !\\e)", shape = box, color = DimGray]
    88 [label = "2, (2, \\e), (1, \\e)", shape = box, color = DimGray]
    89 [label = "2, (4, \\e), (1, \\e)", shape = box, color = DimGray]
    90 [label = "4, (2, \\e), (1, \\e)", shape = box]
    91 [label = "4, (4, \\e), (1, \\e)", shape = box, color = DimGray]
  }
  I0 -> 0
  0 -> 1 [label = "\\e"]
  0 -> 2 [label = "\\e"]
  1 -> 3 [label = "\\e", color = DimGray]
  1 -> 4 [label = "\\e"]
  2 -> 5 [label = "\\e", color = DimGray]
  2 -> 6 [label = "\\e"]
  3 -> 7 [label = "\\e", color = DimGray]
  3 -> 8 [label = "\\e", color = DimGray]
  4 -> 9 [label = "\\e"]
  4 -> 10 [label = "\\e", color = DimGray]
  5 -> 11 [label = "\\e", color = DimGray]
  5 -> 12 [label = "\\e", color = DimGray]
  6 -> 13 [label = "\\e", color = DimGray]
  6 -> 14 [label = "\\e"]
  7 -> 15 [label = "\\e", color = DimGray]
  7 -> 16 [label = "\\e", color = DimGray]
  8 -> 17 [label = "\\e", color = DimGray]
  8 -> 18 [label = "\\e", color = DimGray]
  9 -> 19 [label = "\\e"]
  9 -> 20 [label = "\\e", color = DimGray]
  10 -> 21 [label = "\\e", color = DimGray]
  10 -> 22 [label = "\\e", color = DimGray]
  11 -> 23 [label = "\\e", color = DimGray]
  11 -> 24 [label = "\\e", color = DimGray]
  12 -> 25 [label = "\\e", color = DimGray]
  12 -> 26 [label = "\\e", color = DimGray]
  13 -> 27 [label = "\\e", color = DimGray]
  13 -> 28 [label = "\\e", color = DimGray]
  13 -> 29 [label = "\\e", color = DimGray]
  14 -> F14
  15 -> 30 [label = "\\e", color = DimGray]
  15 -> 31 [label = "\\e", color = DimGray]
  16 -> 32 [label = "\\e", color = DimGray]
  16 -> 33 [label = "\\e", color = DimGray]
  17 -> 34 [label = "\\e", color = DimGray]
  17 -> 35 [label = "\\e", color = DimGray]
  17 -> 36 [label = "\\e", color = DimGray]
  19 -> 37 [label = "\\e"]
  19 -> 38 [label = "\\e", color = DimGray]
  20 -> 39 [label = "\\e", color = DimGray]
  20 -> 40 [label = "\\e", color = DimGray]
  21 -> 41 [label = "\\e", color = DimGray]
  21 -> 42 [label = "\\e", color = DimGray]
  21 -> 43 [label = "\\e", color = DimGray]
  23 -> 44 [label = "\\e", color = DimGray]
  23 -> 45 [label = "\\e", color = DimGray]
  23 -> 46 [label = "\\e", color = DimGray]
  25 -> 47 [label = "\\e", color = DimGray]
  25 -> 48 [label = "\\e", color = DimGray]
  25 -> 49 [label = "\\e", color = DimGray]
  30 -> 50 [label = "\\e", color = DimGray]
  30 -> 51 [label = "\\e", color = DimGray]
  30 -> 52 [label = "\\e", color = DimGray]
  32 -> 53 [label = "\\e", color = DimGray]
  32 -> 54 [label = "\\e", color = DimGray]
  32 -> 55 [label = "\\e", color = DimGray]
  37 -> 56 [label = "\\e"]
  37 -> 57 [label = "\\e", color = DimGray]
  37 -> 58 [label = "\\e", color = DimGray]
  39 -> 59 [label = "\\e", color = DimGray]
  39 -> 60 [label = "\\e", color = DimGray]
  39 -> 61 [label = "\\e", color = DimGray]
  56 -> 62 [label = "b"]
  62 -> 63 [label = "\\e"]
  63 -> 64 [label = "\\e"]
  63 -> 65 [label = "\\e"]
  64 -> 66 [label = "\\e"]
  65 -> 67 [label = "\\e", color = DimGray]
  65 -> 68 [label = "\\e"]
  66 -> 69 [label = "\\e"]
  66 -> 70 [label = "\\e", color = DimGray]
  67 -> 71 [label = "\\e", color = DimGray]
  68 -> 72 [label = "\\e"]
  69 -> 73 [label = "\\e"]
  70 -> 74 [label = "\\e", color = DimGray]
  70 -> 75 [label = "\\e", color = DimGray]
  71 -> 76 [label = "\\e", color = DimGray]
  71 -> 77 [label = "\\e", color = DimGray]
  72 -> 78 [label = "\\e", color = DimGray]
  72 -> 79 [label = "\\e"]
  73 -> 13 [label = "\\e", color = DimGray]
  73 -> 14 [label = "\\e"]
  74 -> 80 [label = "\\e", color = DimGray]
  75 -> 81 [label = "\\e", color = DimGray]
  76 -> 82 [label = "\\e", color = DimGray]
  77 -> 83 [label = "\\e", color = DimGray]
  77 -> 84 [label = "\\e", color = DimGray]
  78 -> 85 [label = "\\e", color = DimGray]
  79 -> 86 [label = "\\e"]
  79 -> 87 [label = "\\e", color = DimGray]
  80 -> 23 [label = "\\e", color = DimGray]
  80 -> 24 [label = "\\e", color = DimGray]
  81 -> 25 [label = "\\e", color = DimGray]
  81 -> 26 [label = "\\e", color = DimGray]
  82 -> 17 [label = "\\e", color = DimGray]
  82 -> 18 [label = "\\e", color = DimGray]
  83 -> 88 [label = "\\e", color = DimGray]
  84 -> 89 [label = "\\e", color = DimGray]
  85 -> 21 [label = "\\e", color = DimGray]
  85 -> 22 [label = "\\e", color = DimGray]
  86 -> 90 [label = "\\e"]
  87 -> 91 [label = "\\e", color = DimGray]
  88 -> 30 [label = "\\e", color = DimGray]
  88 -> 31 [label = "\\e", color = DimGray]
  89 -> 32 [label = "\\e", color = DimGray]
  89 -> 33 [label = "\\e", color = DimGray]
  90 -> 37 [label = "\\e"]
  90 -> 38 [label = "\\e", color = DimGray]
  91 -> 39 [label = "\\e", color = DimGray]
  91 -> 40 [label = "\\e", color = DimGray]
}'''
CHECK_EQ(res, lhs & rhs & third)
CHECK_EQUIV(vcsn.automaton(res),
            std('lal_char(b), b', 'b*'))


# Simple micro-optimization: don't build states which have no possible
# outgoing transitions.
a1 = vcsn.automaton(r'''
context = "lan, b"
$ -> 0
''')

a2 = vcsn.automaton(r'''
context = "lan, b"
$ -> 0
0 -> 1 \e
1 -> $
''')

res = r'''digraph
{
  vcsn_context = "nullableset<letterset<char_letters()>>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "0, (0, !\\e)", shape = box, color = DimGray]
  }
  I0 -> 0 [color = DimGray]
}'''
CHECK_EQ(res, a1 & a2)



# Because of stupid implementation details, we used to _not_ insplit
# the automaton in repeated conjunction, yielding an incorrect result.
a = vcsn.automaton(r'''
context = "lan, q"
$ -> 0
0 -> 0 <1/2>\e, <2>a
0 -> $
''')
CHECK_EQ((a & a).strip(), a & 2)
CHECK_EQ((a & a & a).strip(), a & 3)


# An automaton such that the insplit-based and the rank-based
# algorithm give different results.
a = vcsn.automaton(r'''
context = "lan, q"
$ -> 0
0 -> 0 <1/2>\e, a
0 -> $
''')

insplit_res = r'''digraph
{
  vcsn_context = "nullableset<letterset<char_letters(a)>>, q"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F1
    F2
    F3
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "0, (0, !\\e), (0, !\\e)", shape = box]
    1 [label = "0, (0, \\e), (0, !\\e)", shape = box]
    2 [label = "0, (0, !\\e), (0, \\e)", shape = box]
    3 [label = "0, (0, \\e), (0, \\e)", shape = box]
  }
  I0 -> 0
  0 -> F0
  0 -> 0 [label = "<1/2>\\e, a"]
  0 -> 1 [label = "<1/2>\\e"]
  0 -> 2 [label = "<1/2>\\e"]
  1 -> F1
  1 -> 0 [label = "a"]
  1 -> 1 [label = "<1/2>\\e"]
  1 -> 3 [label = "<1/2>\\e"]
  2 -> F2
  2 -> 0 [label = "a"]
  2 -> 2 [label = "<1/2>\\e"]
  3 -> F3
  3 -> 0 [label = "a"]
  3 -> 3 [label = "<1/2>\\e"]
}'''

CHECK_EQ(insplit_res, a & a & a)




###############################################
## Check mixed epsilon and letters going out ##
###############################################

a1 = vcsn.automaton(r'''
context = "lan_char(abc), b"
  $ -> 0
  0 -> 1 a
  1 -> $
  0 -> 2 \e
  2 -> $
''')

a2 = vcsn.automaton(r'''
context = "lan_char(abc), b"
  $ -> 0
  0 -> 1 \e
  1 -> 2 a
  2 -> $
''')

res = r'''digraph
{
  vcsn_context = "nullableset<letterset<char_letters(abc)>>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F4
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "0, (0, !\\e)", shape = box]
    1 [label = "2, (0, !\\e)", shape = box, color = DimGray]
    2 [label = "0, (1, \\e)", shape = box]
    3 [label = "2, (1, \\e)", shape = box, color = DimGray]
    4 [label = "1, (2, !\\e)", shape = box]
  }
  I0 -> 0
  0 -> 1 [label = "\\e", color = DimGray]
  0 -> 2 [label = "\\e"]
  1 -> 3 [label = "\\e", color = DimGray]
  2 -> 4 [label = "a"]
  4 -> F4
}'''
CHECK_EQ(res, a1 & a2)

## ------------- ##
## Conjunction.  ##
## ------------- ##

# Check that the conjunction structure is transparent: invoke it.
a = std('lal_char(a), b', 'a')
a = a & a & a
CHECK_EQ('1', a('a'))
