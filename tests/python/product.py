#! /usr/bin/env python

import vcsn
from test import *

b = vcsn.context('lal_char(abcd)_b')

## ---------------------- ##
## Existing transitions.  ##
## ---------------------- ##

# See the actual code of product to understand the point of this test
# (which is new_transition vs. add_transition).
a1 = b.ratexp('a*a').derived_term()
a2 = a1 & a1
CHECK_EQ('a*a', str(a2.ratexp()))

## ---------------------- ##
## (a+b)* & (b+c)* = b*.  ##
## ---------------------- ##

lhs = vcsn.context('lal_char(ab)_b').ratexp('(a+b)*').standard()
rhs = vcsn.context('lal_char(bc)_b').ratexp('(b+c)*').standard()
CHECK_EQ('''digraph
{
  vcsn_context = "lal_char(b)_b"
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
         str(lhs & rhs))

## ------------- ##
## ab & cd = 0.  ##
## ------------- ##

lhs = vcsn.context('lal_char(ab)_b').ratexp('ab').standard()
rhs = vcsn.context('lal_char(cd)_b').ratexp('cd').standard()
CHECK_EQ('''digraph
{
  vcsn_context = "lal_char()_b"
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
}''', str(lhs & rhs))



## ---------------------- ##
## (a+b)* & (c+d)* = \e.  ##
## ---------------------- ##

lhs = vcsn.context('lal_char(ab)_b').ratexp('(a+b)*').standard()
rhs = vcsn.context('lal_char(cd)_b').ratexp('(c+d)*').standard()
CHECK_EQ('''digraph
{
  vcsn_context = "lal_char()_b"
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
         str(lhs & rhs))



## ------------ ##
## lal_char_z.  ##
## ------------ ##

# <2>(a*b*a*)
lhs = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abc)_z"
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
  vcsn_context = "lal_char(abc)_z"
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
  vcsn_context = "lal_char(abc)_z"
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

CHECK_EQ(exp, str(lhs & rhs))

## ------------------------------------ ##
## Heterogeneous (and variadic) input.  ##
## ------------------------------------ ##

# check OPERATION RES AUT...
# --------------------------
def check(operation, exp, *args):
    CHECK_EQ(exp, str(vcsn.automaton._product(list(args)).ratexp()))

# RatE and B, in both directions.
a1 = vcsn.context('lal_char(ab)_ratexpset<lal_char(uv)_b>') \
         .ratexp('(<u>a+<v>b)*').standard()
a2 = vcsn.context('lal_char(ab)_b').ratexp('a{+}').standard()
check('product', '<u>a+<u>a<u>a(<u>a)*', a1, a2)
check('product', '<u>a+<u>a<u>a(<u>a)*', a2, a1)

# Z, Q, R.
z = vcsn.context('lal_char(ab)_z').ratexp('(<2>a+<3>b)*')    .derived_term()
q = vcsn.context('lal_char(ab)_q').ratexp('(<1/2>a+<1/3>b)*').derived_term()
r = vcsn.context('lal_char(ab)_r').ratexp('(<.2>a+<.3>b)*')  .derived_term()

check('product', '(a+b)*', z, q)
check('product', '(a+b)*', q, z)
check('product', '(<2>a+<3>b)*', z, q, z)
check('product', '(<1/2>a+<1/3>b)*', z, q, q)

check('product', '(<0.4>a+<0.9>b)*', z, r)
check('product', '(<0.4>a+<0.9>b)*', r, z)

check('product', '(<0.1>a+<0.1>b)*', q, r)
check('product', '(<0.1>a+<0.1>b)*', r, q)


## ----------------- ##
## Non-commutative.  ##
## ----------------- ##

a1 = vcsn.context('lal_char(ab)_ratexpset<lal_char(uv)_b>') \
         .ratexp('<u>a<v>b').standard()
a2 = vcsn.context('lal_char(ab)_ratexpset<lal_char(xy)_b>') \
         .ratexp('<x>a<y>b').standard()

def check_enumerate(exp, aut):
    CHECK_EQ(exp, str(aut.strip().enumerate(4)))

check_enumerate('<uxvy>ab', a1 & a2)
check_enumerate('\z', a1.transpose() & a2)
check_enumerate('\z', a1 & a2.transpose())
check_enumerate('<vyux>ba', a1.transpose() & a2.transpose())


## ---------- ##
## variadic.  ##
## ---------- ##

# unary case: return only the accessible part.
CHECK_EQ('''digraph
{
  vcsn_context = "lal_char(ab)_b"
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
}''', str(vcsn.automaton._product([vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab)_b"
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
}
''')])))

# four arguments.
ctx = vcsn.context('lal_char(x)_ratexpset<lal_char(abcd)_b>')
a = dict()
for l in ['a', 'b', 'c', 'd']:
    a[l] = ctx.ratexp("<{}>x".format(l)).standard()
check_enumerate('<abcd>x', a['a'] & a['b'] & a['c'] & a['d'])


## ----------------- ##
## ratexp & ratexp.  ##
## ----------------- ##

# Add stars (<u>a*, not <u>a) to avoid that the trivial identities
# (a&b -> \z) fire and yield a global \z.
br = vcsn.context('lal_char(a)_ratexpset<lal_char(uv)_b>') \
         .ratexp('<u>a*')
z = vcsn.context('lal_char(b)_z').ratexp('<2>b*')
q = vcsn.context('lal_char(c)_q').ratexp('<1/3>c*')
r = vcsn.context('lal_char(d)_r').ratexp('<.4>d*')
CHECK_EQ('<u>a*&<<2>\e>b*&<<0.333333>\e>c*&<<0.4>\e>d*', str(br & z & q & r))

## ----------------- ##
## nullable labels.  ##
## ----------------- ##

lhs = vcsn.context('lan_char(ab)_b').ratexp('(a+b)*').thompson()
rhs = vcsn.context('lan_char(bc)_b').ratexp('(b+c)*').thompson()
res = r'''digraph
{
  vcsn_context = "lan<lal_char(b)>_b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F6
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "6, 6", shape = box]
    1 [label = "0, 6", shape = box]
    2 [label = "7, 6", shape = box]
    3 [label = "2, 6", shape = box, color = DimGray]
    4 [label = "4, 6", shape = box]
    5 [label = "7, 0", shape = box, color = DimGray]
    6 [label = "7, 7", shape = box]
    7 [label = "2, 0", shape = box, color = DimGray]
    8 [label = "2, 7", shape = box, color = DimGray]
    9 [label = "4, 0", shape = box]
    10 [label = "4, 7", shape = box, color = DimGray]
    11 [label = "7, 2", shape = box, color = DimGray]
    12 [label = "7, 4", shape = box, color = DimGray]
    13 [label = "2, 2", shape = box, color = DimGray]
    14 [label = "2, 4", shape = box, color = DimGray]
    15 [label = "4, 2", shape = box]
    16 [label = "4, 4", shape = box, color = DimGray]
    17 [label = "5, 3", shape = box]
    18 [label = "1, 3", shape = box]
    19 [label = "7, 3", shape = box]
    20 [label = "0, 3", shape = box]
    21 [label = "7, 1", shape = box]
    22 [label = "2, 3", shape = box, color = DimGray]
    23 [label = "4, 3", shape = box]
    24 [label = "2, 1", shape = box, color = DimGray]
    25 [label = "4, 1", shape = box]
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
CHECK_EQ(res, str(lhs & rhs))
CHECK_EQUIV(vcsn.automaton(res),
            vcsn.context("lal_char(b)_b").ratexp("b*").standard())

third = vcsn.context('lan_char(bcd)_b').ratexp('(b+c+d)*').thompson()
res = r'''digraph
{
  vcsn_context = "lan<lal_char(b)>_b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F14
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "6, 6, 8", shape = box]
    1 [label = "0, 6, 8", shape = box]
    2 [label = "7, 6, 8", shape = box]
    3 [label = "2, 6, 8", shape = box, color = DimGray]
    4 [label = "4, 6, 8", shape = box]
    5 [label = "7, 0, 8", shape = box, color = DimGray]
    6 [label = "7, 7, 8", shape = box]
    7 [label = "2, 0, 8", shape = box, color = DimGray]
    8 [label = "2, 7, 8", shape = box, color = DimGray]
    9 [label = "4, 0, 8", shape = box]
    10 [label = "4, 7, 8", shape = box, color = DimGray]
    11 [label = "7, 2, 8", shape = box, color = DimGray]
    12 [label = "7, 4, 8", shape = box, color = DimGray]
    13 [label = "7, 7, 0", shape = box, color = DimGray]
    14 [label = "7, 7, 9", shape = box]
    15 [label = "2, 2, 8", shape = box, color = DimGray]
    16 [label = "2, 4, 8", shape = box, color = DimGray]
    17 [label = "2, 7, 0", shape = box, color = DimGray]
    18 [label = "2, 7, 9", shape = box, color = DimGray]
    19 [label = "4, 2, 8", shape = box]
    20 [label = "4, 4, 8", shape = box, color = DimGray]
    21 [label = "4, 7, 0", shape = box, color = DimGray]
    22 [label = "4, 7, 9", shape = box, color = DimGray]
    23 [label = "7, 2, 0", shape = box, color = DimGray]
    24 [label = "7, 2, 9", shape = box, color = DimGray]
    25 [label = "7, 4, 0", shape = box, color = DimGray]
    26 [label = "7, 4, 9", shape = box, color = DimGray]
    27 [label = "7, 7, 2", shape = box, color = DimGray]
    28 [label = "7, 7, 4", shape = box, color = DimGray]
    29 [label = "7, 7, 6", shape = box, color = DimGray]
    30 [label = "2, 2, 0", shape = box, color = DimGray]
    31 [label = "2, 2, 9", shape = box, color = DimGray]
    32 [label = "2, 4, 0", shape = box, color = DimGray]
    33 [label = "2, 4, 9", shape = box, color = DimGray]
    34 [label = "2, 7, 2", shape = box, color = DimGray]
    35 [label = "2, 7, 4", shape = box, color = DimGray]
    36 [label = "2, 7, 6", shape = box, color = DimGray]
    37 [label = "4, 2, 0", shape = box]
    38 [label = "4, 2, 9", shape = box, color = DimGray]
    39 [label = "4, 4, 0", shape = box, color = DimGray]
    40 [label = "4, 4, 9", shape = box, color = DimGray]
    41 [label = "4, 7, 2", shape = box, color = DimGray]
    42 [label = "4, 7, 4", shape = box, color = DimGray]
    43 [label = "4, 7, 6", shape = box, color = DimGray]
    44 [label = "7, 2, 2", shape = box, color = DimGray]
    45 [label = "7, 2, 4", shape = box, color = DimGray]
    46 [label = "7, 2, 6", shape = box, color = DimGray]
    47 [label = "7, 4, 2", shape = box, color = DimGray]
    48 [label = "7, 4, 4", shape = box, color = DimGray]
    49 [label = "7, 4, 6", shape = box, color = DimGray]
    50 [label = "2, 2, 2", shape = box, color = DimGray]
    51 [label = "2, 2, 4", shape = box, color = DimGray]
    52 [label = "2, 2, 6", shape = box, color = DimGray]
    53 [label = "2, 4, 2", shape = box, color = DimGray]
    54 [label = "2, 4, 4", shape = box, color = DimGray]
    55 [label = "2, 4, 6", shape = box, color = DimGray]
    56 [label = "4, 2, 2", shape = box]
    57 [label = "4, 2, 4", shape = box, color = DimGray]
    58 [label = "4, 2, 6", shape = box, color = DimGray]
    59 [label = "4, 4, 2", shape = box, color = DimGray]
    60 [label = "4, 4, 4", shape = box, color = DimGray]
    61 [label = "4, 4, 6", shape = box, color = DimGray]
    62 [label = "5, 3, 3", shape = box]
    63 [label = "1, 3, 3", shape = box]
    64 [label = "7, 3, 3", shape = box]
    65 [label = "0, 3, 3", shape = box]
    66 [label = "7, 1, 3", shape = box]
    67 [label = "2, 3, 3", shape = box, color = DimGray]
    68 [label = "4, 3, 3", shape = box]
    69 [label = "7, 7, 3", shape = box]
    70 [label = "7, 0, 3", shape = box, color = DimGray]
    71 [label = "2, 1, 3", shape = box, color = DimGray]
    72 [label = "4, 1, 3", shape = box]
    73 [label = "7, 7, 1", shape = box]
    74 [label = "7, 2, 3", shape = box, color = DimGray]
    75 [label = "7, 4, 3", shape = box, color = DimGray]
    76 [label = "2, 7, 3", shape = box, color = DimGray]
    77 [label = "2, 0, 3", shape = box, color = DimGray]
    78 [label = "4, 7, 3", shape = box, color = DimGray]
    79 [label = "4, 0, 3", shape = box]
    80 [label = "7, 2, 1", shape = box, color = DimGray]
    81 [label = "7, 4, 1", shape = box, color = DimGray]
    82 [label = "2, 7, 1", shape = box, color = DimGray]
    83 [label = "2, 2, 3", shape = box, color = DimGray]
    84 [label = "2, 4, 3", shape = box, color = DimGray]
    85 [label = "4, 7, 1", shape = box, color = DimGray]
    86 [label = "4, 2, 3", shape = box]
    87 [label = "4, 4, 3", shape = box, color = DimGray]
    88 [label = "2, 2, 1", shape = box, color = DimGray]
    89 [label = "2, 4, 1", shape = box, color = DimGray]
    90 [label = "4, 2, 1", shape = box]
    91 [label = "4, 4, 1", shape = box, color = DimGray]
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
CHECK_EQ(res, str(lhs & rhs & third))
CHECK_EQUIV(vcsn.automaton(res),
            vcsn.context("lal_char(b)_b").ratexp("b*").standard())

###############################################
## Check mixed epsilon and letters going out ##
###############################################


a1 = vcsn.automaton(r'''digraph
{
  vcsn_context = "lan<lal_char(abc)>_b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F1
    F5
  }
  {
    node [shape = circle]
    0
    1
    5
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  1 -> F1
  0 -> 5 [label = "\\e"]
  5 -> F5
}''')


a2 = vcsn.automaton(r'''digraph
{
  vcsn_context = "lan<lal_char(abc)>_b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F1
    F5
  }
  {
    node [shape = circle]
    0
    5
  }
  I0 -> 0
  0 -> 5 [label = "\\e"]
  5 -> 6 [label = "a"]
  6 -> F6
}''')

res = r'''digraph
{
  vcsn_context = "lan<lal_char(abc)>_b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F4
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "0, 0", shape = box]
    1 [label = "2, 0", shape = box, color = DimGray]
    2 [label = "0, 1", shape = box]
    3 [label = "2, 1", shape = box, color = DimGray]
    4 [label = "1, 2", shape = box]
  }
  I0 -> 0
  0 -> 1 [label = "\\e", color = DimGray]
  0 -> 2 [label = "\\e"]
  1 -> 3 [label = "\\e", color = DimGray]
  2 -> 4 [label = "a"]
  4 -> F4
}'''

CHECK_EQ(res, str(vcsn.automaton._product([a1, a2])))

## ---------------------- ##
## Conjunction            ##
## ---------------------- ##

#Show that Conjunction is callable trough wrapper
#The call is perfectly transparent

b = vcsn.context('lal_char(a)_b')
a = b.ratexp('a').standard()
a = a & a & a
CHECK_EQ(str(a('a')), '1')
