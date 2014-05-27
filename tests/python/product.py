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
  {
    node [shape = point, width = 0]
    I0
    F0
    F1
  }
  {
    node [shape = circle]
    0 [label = "0, 0", shape = box, style = rounded]
    1 [label = "3, 1", shape = box, style = rounded]
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
  {
    node [shape = point, width = 0]
    I0
  }
  {
    node [shape = circle]
    0 [label = "0, 0", shape = box, style = rounded] [color = DimGray]
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
  {
    node [shape = point, width = 0]
    I0
    F0
  }
  {
    node [shape = circle]
    0 [label = "0, 0", shape = box, style = rounded]
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
  {
    node [shape = point, width = 0]
    I0
    F0
    F3
  }
  {
    node [shape = circle]
    0 [label = "0, 0", shape = box, style = rounded]
    1 [label = "0, 1", shape = box, style = rounded]
    2 [label = "2, 1", shape = box, style = rounded] [color = DimGray]
    3 [label = "1, 0", shape = box, style = rounded]
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
  {
    node [shape = point, width = 0]
    I0
  }
  {
    node [shape = circle]
    0 [label = "0", shape = box, style = rounded] [color = DimGray]
    1 [label = "1", shape = box, style = rounded] [color = DimGray]
    2 [label = "2", shape = box, style = rounded] [color = DimGray]
  }
  I0 -> 0 [color = DimGray]
  0 -> 1 [label = "a", color = DimGray]
  1 -> 2 [label = "a", color = DimGray]
}''', str(vcsn.automaton._product([vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab)_b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
  }
  {
    node [shape = circle]
    0 [label = "0", shape = box, style = rounded] [color = DimGray]
    1 [label = "1", shape = box, style = rounded] [color = DimGray]
    2 [label = "2", shape = box, style = rounded] [color = DimGray]
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

def check_equivalent(a1, a2):
    if a1.is_equivalent(a2):
        PASS()
    else:
        FAIL(a1 + " not equivalent to " + a2);

lhs = vcsn.context('lan_char(ab)_b').ratexp('(a+b)*').thompson()
rhs = vcsn.context('lan_char(bc)_b').ratexp('(b+c)*').thompson()
res = r'''digraph
{
  vcsn_context = "lan<lal_char(b)>_b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F10
  }
  {
    node [shape = circle]
    0 [label = "6, 6", shape = box, style = rounded]
    1 [label = "0, 6", shape = box, style = rounded]
    2 [label = "7, 6", shape = box, style = rounded]
    3 [label = "6, 0", shape = box, style = rounded] [color = DimGray]
    4 [label = "6, 7", shape = box, style = rounded] [color = DimGray]
    5 [label = "2, 6", shape = box, style = rounded] [color = DimGray]
    6 [label = "4, 6", shape = box, style = rounded]
    7 [label = "0, 0", shape = box, style = rounded] [color = DimGray]
    8 [label = "0, 7", shape = box, style = rounded] [color = DimGray]
    9 [label = "7, 0", shape = box, style = rounded] [color = DimGray]
    10 [label = "7, 7", shape = box, style = rounded]
    11 [label = "6, 2", shape = box, style = rounded] [color = DimGray]
    12 [label = "6, 4", shape = box, style = rounded] [color = DimGray]
    13 [label = "2, 0", shape = box, style = rounded] [color = DimGray]
    14 [label = "2, 7", shape = box, style = rounded] [color = DimGray]
    15 [label = "4, 0", shape = box, style = rounded]
    16 [label = "4, 7", shape = box, style = rounded] [color = DimGray]
    17 [label = "0, 2", shape = box, style = rounded] [color = DimGray]
    18 [label = "0, 4", shape = box, style = rounded] [color = DimGray]
    19 [label = "7, 2", shape = box, style = rounded] [color = DimGray]
    20 [label = "7, 4", shape = box, style = rounded] [color = DimGray]
    21 [label = "2, 2", shape = box, style = rounded] [color = DimGray]
    22 [label = "2, 4", shape = box, style = rounded] [color = DimGray]
    23 [label = "4, 2", shape = box, style = rounded]
    24 [label = "4, 4", shape = box, style = rounded] [color = DimGray]
    25 [label = "5, 3", shape = box, style = rounded]
    26 [label = "1, 3", shape = box, style = rounded]
    27 [label = "5, 1", shape = box, style = rounded] [color = DimGray]
    28 [label = "7, 3", shape = box, style = rounded]
    29 [label = "0, 3", shape = box, style = rounded]
    30 [label = "1, 1", shape = box, style = rounded] [color = DimGray]
    31 [label = "5, 7", shape = box, style = rounded] [color = DimGray]
    32 [label = "5, 0", shape = box, style = rounded] [color = DimGray]
    33 [label = "7, 1", shape = box, style = rounded]
    34 [label = "2, 3", shape = box, style = rounded] [color = DimGray]
    35 [label = "4, 3", shape = box, style = rounded]
    36 [label = "0, 1", shape = box, style = rounded] [color = DimGray]
    37 [label = "1, 7", shape = box, style = rounded] [color = DimGray]
    38 [label = "1, 0", shape = box, style = rounded] [color = DimGray]
    39 [label = "5, 2", shape = box, style = rounded] [color = DimGray]
    40 [label = "5, 4", shape = box, style = rounded] [color = DimGray]
    41 [label = "2, 1", shape = box, style = rounded] [color = DimGray]
    42 [label = "4, 1", shape = box, style = rounded]
    43 [label = "1, 2", shape = box, style = rounded] [color = DimGray]
    44 [label = "1, 4", shape = box, style = rounded] [color = DimGray]
  }
  I0 -> 0
  0 -> 1 [label = "\\e"]
  0 -> 2 [label = "\\e"]
  0 -> 3 [label = "\\e", color = DimGray]
  0 -> 4 [label = "\\e", color = DimGray]
  1 -> 5 [label = "\\e", color = DimGray]
  1 -> 6 [label = "\\e"]
  1 -> 7 [label = "\\e", color = DimGray]
  1 -> 8 [label = "\\e", color = DimGray]
  2 -> 9 [label = "\\e", color = DimGray]
  2 -> 10 [label = "\\e"]
  3 -> 11 [label = "\\e", color = DimGray]
  3 -> 12 [label = "\\e", color = DimGray]
  5 -> 13 [label = "\\e", color = DimGray]
  5 -> 14 [label = "\\e", color = DimGray]
  6 -> 15 [label = "\\e"]
  6 -> 16 [label = "\\e", color = DimGray]
  7 -> 17 [label = "\\e", color = DimGray]
  7 -> 18 [label = "\\e", color = DimGray]
  9 -> 19 [label = "\\e", color = DimGray]
  9 -> 20 [label = "\\e", color = DimGray]
  10 -> F10
  13 -> 21 [label = "\\e", color = DimGray]
  13 -> 22 [label = "\\e", color = DimGray]
  15 -> 23 [label = "\\e"]
  15 -> 24 [label = "\\e", color = DimGray]
  23 -> 25 [label = "b"]
  25 -> 26 [label = "\\e"]
  25 -> 27 [label = "\\e", color = DimGray]
  26 -> 28 [label = "\\e"]
  26 -> 29 [label = "\\e"]
  26 -> 30 [label = "\\e", color = DimGray]
  27 -> 31 [label = "\\e", color = DimGray]
  27 -> 32 [label = "\\e", color = DimGray]
  28 -> 33 [label = "\\e"]
  29 -> 34 [label = "\\e", color = DimGray]
  29 -> 35 [label = "\\e"]
  29 -> 36 [label = "\\e", color = DimGray]
  30 -> 37 [label = "\\e", color = DimGray]
  30 -> 38 [label = "\\e", color = DimGray]
  32 -> 39 [label = "\\e", color = DimGray]
  32 -> 40 [label = "\\e", color = DimGray]
  33 -> 9 [label = "\\e", color = DimGray]
  33 -> 10 [label = "\\e"]
  34 -> 41 [label = "\\e", color = DimGray]
  35 -> 42 [label = "\\e"]
  36 -> 7 [label = "\\e", color = DimGray]
  36 -> 8 [label = "\\e", color = DimGray]
  38 -> 43 [label = "\\e", color = DimGray]
  38 -> 44 [label = "\\e", color = DimGray]
  41 -> 13 [label = "\\e", color = DimGray]
  41 -> 14 [label = "\\e", color = DimGray]
  42 -> 15 [label = "\\e"]
  42 -> 16 [label = "\\e", color = DimGray]
}'''
CHECK_EQ(res, str(lhs & rhs))
check_equivalent(vcsn.automaton(res).proper(),
                 vcsn.context("lal_char(b)_b").ratexp("b*").standard())

third = vcsn.context('lan_char(bcd)_b').ratexp('(b+c+d)*').thompson()
res = r'''digraph
{
  vcsn_context = "lan<lal_char(b)>_b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F48
  }
  {
    node [shape = circle]
    0 [label = "6, 6, 8", shape = box, style = rounded]
    1 [label = "0, 6, 8", shape = box, style = rounded]
    2 [label = "7, 6, 8", shape = box, style = rounded]
    3 [label = "6, 0, 8", shape = box, style = rounded] [color = DimGray]
    4 [label = "6, 7, 8", shape = box, style = rounded] [color = DimGray]
    5 [label = "6, 6, 0", shape = box, style = rounded] [color = DimGray]
    6 [label = "6, 6, 9", shape = box, style = rounded] [color = DimGray]
    7 [label = "2, 6, 8", shape = box, style = rounded] [color = DimGray]
    8 [label = "4, 6, 8", shape = box, style = rounded]
    9 [label = "0, 0, 8", shape = box, style = rounded] [color = DimGray]
    10 [label = "0, 7, 8", shape = box, style = rounded] [color = DimGray]
    11 [label = "0, 6, 0", shape = box, style = rounded] [color = DimGray]
    12 [label = "0, 6, 9", shape = box, style = rounded] [color = DimGray]
    13 [label = "7, 0, 8", shape = box, style = rounded] [color = DimGray]
    14 [label = "7, 7, 8", shape = box, style = rounded]
    15 [label = "7, 6, 0", shape = box, style = rounded] [color = DimGray]
    16 [label = "7, 6, 9", shape = box, style = rounded] [color = DimGray]
    17 [label = "6, 2, 8", shape = box, style = rounded] [color = DimGray]
    18 [label = "6, 4, 8", shape = box, style = rounded] [color = DimGray]
    19 [label = "6, 0, 0", shape = box, style = rounded] [color = DimGray]
    20 [label = "6, 0, 9", shape = box, style = rounded] [color = DimGray]
    21 [label = "6, 7, 0", shape = box, style = rounded] [color = DimGray]
    22 [label = "6, 7, 9", shape = box, style = rounded] [color = DimGray]
    23 [label = "6, 6, 2", shape = box, style = rounded] [color = DimGray]
    24 [label = "6, 6, 4", shape = box, style = rounded] [color = DimGray]
    25 [label = "6, 6, 6", shape = box, style = rounded] [color = DimGray]
    26 [label = "2, 0, 8", shape = box, style = rounded] [color = DimGray]
    27 [label = "2, 7, 8", shape = box, style = rounded] [color = DimGray]
    28 [label = "2, 6, 0", shape = box, style = rounded] [color = DimGray]
    29 [label = "2, 6, 9", shape = box, style = rounded] [color = DimGray]
    30 [label = "4, 0, 8", shape = box, style = rounded]
    31 [label = "4, 7, 8", shape = box, style = rounded] [color = DimGray]
    32 [label = "4, 6, 0", shape = box, style = rounded] [color = DimGray]
    33 [label = "4, 6, 9", shape = box, style = rounded] [color = DimGray]
    34 [label = "0, 2, 8", shape = box, style = rounded] [color = DimGray]
    35 [label = "0, 4, 8", shape = box, style = rounded] [color = DimGray]
    36 [label = "0, 0, 0", shape = box, style = rounded] [color = DimGray]
    37 [label = "0, 0, 9", shape = box, style = rounded] [color = DimGray]
    38 [label = "0, 7, 0", shape = box, style = rounded] [color = DimGray]
    39 [label = "0, 7, 9", shape = box, style = rounded] [color = DimGray]
    40 [label = "0, 6, 2", shape = box, style = rounded] [color = DimGray]
    41 [label = "0, 6, 4", shape = box, style = rounded] [color = DimGray]
    42 [label = "0, 6, 6", shape = box, style = rounded] [color = DimGray]
    43 [label = "7, 2, 8", shape = box, style = rounded] [color = DimGray]
    44 [label = "7, 4, 8", shape = box, style = rounded] [color = DimGray]
    45 [label = "7, 0, 0", shape = box, style = rounded] [color = DimGray]
    46 [label = "7, 0, 9", shape = box, style = rounded] [color = DimGray]
    47 [label = "7, 7, 0", shape = box, style = rounded] [color = DimGray]
    48 [label = "7, 7, 9", shape = box, style = rounded]
    49 [label = "7, 6, 2", shape = box, style = rounded] [color = DimGray]
    50 [label = "7, 6, 4", shape = box, style = rounded] [color = DimGray]
    51 [label = "7, 6, 6", shape = box, style = rounded] [color = DimGray]
    52 [label = "6, 2, 0", shape = box, style = rounded] [color = DimGray]
    53 [label = "6, 2, 9", shape = box, style = rounded] [color = DimGray]
    54 [label = "6, 4, 0", shape = box, style = rounded] [color = DimGray]
    55 [label = "6, 4, 9", shape = box, style = rounded] [color = DimGray]
    56 [label = "6, 0, 2", shape = box, style = rounded] [color = DimGray]
    57 [label = "6, 0, 4", shape = box, style = rounded] [color = DimGray]
    58 [label = "6, 0, 6", shape = box, style = rounded] [color = DimGray]
    59 [label = "6, 7, 2", shape = box, style = rounded] [color = DimGray]
    60 [label = "6, 7, 4", shape = box, style = rounded] [color = DimGray]
    61 [label = "6, 7, 6", shape = box, style = rounded] [color = DimGray]
    62 [label = "2, 2, 8", shape = box, style = rounded] [color = DimGray]
    63 [label = "2, 4, 8", shape = box, style = rounded] [color = DimGray]
    64 [label = "2, 0, 0", shape = box, style = rounded] [color = DimGray]
    65 [label = "2, 0, 9", shape = box, style = rounded] [color = DimGray]
    66 [label = "2, 7, 0", shape = box, style = rounded] [color = DimGray]
    67 [label = "2, 7, 9", shape = box, style = rounded] [color = DimGray]
    68 [label = "2, 6, 2", shape = box, style = rounded] [color = DimGray]
    69 [label = "2, 6, 4", shape = box, style = rounded] [color = DimGray]
    70 [label = "2, 6, 6", shape = box, style = rounded] [color = DimGray]
    71 [label = "4, 2, 8", shape = box, style = rounded]
    72 [label = "4, 4, 8", shape = box, style = rounded] [color = DimGray]
    73 [label = "4, 0, 0", shape = box, style = rounded] [color = DimGray]
    74 [label = "4, 0, 9", shape = box, style = rounded] [color = DimGray]
    75 [label = "4, 7, 0", shape = box, style = rounded] [color = DimGray]
    76 [label = "4, 7, 9", shape = box, style = rounded] [color = DimGray]
    77 [label = "4, 6, 2", shape = box, style = rounded] [color = DimGray]
    78 [label = "4, 6, 4", shape = box, style = rounded] [color = DimGray]
    79 [label = "4, 6, 6", shape = box, style = rounded] [color = DimGray]
    80 [label = "0, 2, 0", shape = box, style = rounded] [color = DimGray]
    81 [label = "0, 2, 9", shape = box, style = rounded] [color = DimGray]
    82 [label = "0, 4, 0", shape = box, style = rounded] [color = DimGray]
    83 [label = "0, 4, 9", shape = box, style = rounded] [color = DimGray]
    84 [label = "0, 0, 2", shape = box, style = rounded] [color = DimGray]
    85 [label = "0, 0, 4", shape = box, style = rounded] [color = DimGray]
    86 [label = "0, 0, 6", shape = box, style = rounded] [color = DimGray]
    87 [label = "0, 7, 2", shape = box, style = rounded] [color = DimGray]
    88 [label = "0, 7, 4", shape = box, style = rounded] [color = DimGray]
    89 [label = "0, 7, 6", shape = box, style = rounded] [color = DimGray]
    90 [label = "7, 2, 0", shape = box, style = rounded] [color = DimGray]
    91 [label = "7, 2, 9", shape = box, style = rounded] [color = DimGray]
    92 [label = "7, 4, 0", shape = box, style = rounded] [color = DimGray]
    93 [label = "7, 4, 9", shape = box, style = rounded] [color = DimGray]
    94 [label = "7, 0, 2", shape = box, style = rounded] [color = DimGray]
    95 [label = "7, 0, 4", shape = box, style = rounded] [color = DimGray]
    96 [label = "7, 0, 6", shape = box, style = rounded] [color = DimGray]
    97 [label = "7, 7, 2", shape = box, style = rounded] [color = DimGray]
    98 [label = "7, 7, 4", shape = box, style = rounded] [color = DimGray]
    99 [label = "7, 7, 6", shape = box, style = rounded] [color = DimGray]
    100 [label = "6, 2, 2", shape = box, style = rounded] [color = DimGray]
    101 [label = "6, 2, 4", shape = box, style = rounded] [color = DimGray]
    102 [label = "6, 2, 6", shape = box, style = rounded] [color = DimGray]
    103 [label = "6, 4, 2", shape = box, style = rounded] [color = DimGray]
    104 [label = "6, 4, 4", shape = box, style = rounded] [color = DimGray]
    105 [label = "6, 4, 6", shape = box, style = rounded] [color = DimGray]
    106 [label = "2, 2, 0", shape = box, style = rounded] [color = DimGray]
    107 [label = "2, 2, 9", shape = box, style = rounded] [color = DimGray]
    108 [label = "2, 4, 0", shape = box, style = rounded] [color = DimGray]
    109 [label = "2, 4, 9", shape = box, style = rounded] [color = DimGray]
    110 [label = "2, 0, 2", shape = box, style = rounded] [color = DimGray]
    111 [label = "2, 0, 4", shape = box, style = rounded] [color = DimGray]
    112 [label = "2, 0, 6", shape = box, style = rounded] [color = DimGray]
    113 [label = "2, 7, 2", shape = box, style = rounded] [color = DimGray]
    114 [label = "2, 7, 4", shape = box, style = rounded] [color = DimGray]
    115 [label = "2, 7, 6", shape = box, style = rounded] [color = DimGray]
    116 [label = "4, 2, 0", shape = box, style = rounded]
    117 [label = "4, 2, 9", shape = box, style = rounded] [color = DimGray]
    118 [label = "4, 4, 0", shape = box, style = rounded] [color = DimGray]
    119 [label = "4, 4, 9", shape = box, style = rounded] [color = DimGray]
    120 [label = "4, 0, 2", shape = box, style = rounded] [color = DimGray]
    121 [label = "4, 0, 4", shape = box, style = rounded] [color = DimGray]
    122 [label = "4, 0, 6", shape = box, style = rounded] [color = DimGray]
    123 [label = "4, 7, 2", shape = box, style = rounded] [color = DimGray]
    124 [label = "4, 7, 4", shape = box, style = rounded] [color = DimGray]
    125 [label = "4, 7, 6", shape = box, style = rounded] [color = DimGray]
    126 [label = "0, 2, 2", shape = box, style = rounded] [color = DimGray]
    127 [label = "0, 2, 4", shape = box, style = rounded] [color = DimGray]
    128 [label = "0, 2, 6", shape = box, style = rounded] [color = DimGray]
    129 [label = "0, 4, 2", shape = box, style = rounded] [color = DimGray]
    130 [label = "0, 4, 4", shape = box, style = rounded] [color = DimGray]
    131 [label = "0, 4, 6", shape = box, style = rounded] [color = DimGray]
    132 [label = "7, 2, 2", shape = box, style = rounded] [color = DimGray]
    133 [label = "7, 2, 4", shape = box, style = rounded] [color = DimGray]
    134 [label = "7, 2, 6", shape = box, style = rounded] [color = DimGray]
    135 [label = "7, 4, 2", shape = box, style = rounded] [color = DimGray]
    136 [label = "7, 4, 4", shape = box, style = rounded] [color = DimGray]
    137 [label = "7, 4, 6", shape = box, style = rounded] [color = DimGray]
    138 [label = "2, 2, 2", shape = box, style = rounded] [color = DimGray]
    139 [label = "2, 2, 4", shape = box, style = rounded] [color = DimGray]
    140 [label = "2, 2, 6", shape = box, style = rounded] [color = DimGray]
    141 [label = "2, 4, 2", shape = box, style = rounded] [color = DimGray]
    142 [label = "2, 4, 4", shape = box, style = rounded] [color = DimGray]
    143 [label = "2, 4, 6", shape = box, style = rounded] [color = DimGray]
    144 [label = "4, 2, 2", shape = box, style = rounded]
    145 [label = "4, 2, 4", shape = box, style = rounded] [color = DimGray]
    146 [label = "4, 2, 6", shape = box, style = rounded] [color = DimGray]
    147 [label = "4, 4, 2", shape = box, style = rounded] [color = DimGray]
    148 [label = "4, 4, 4", shape = box, style = rounded] [color = DimGray]
    149 [label = "4, 4, 6", shape = box, style = rounded] [color = DimGray]
    150 [label = "5, 3, 3", shape = box, style = rounded]
    151 [label = "1, 3, 3", shape = box, style = rounded]
    152 [label = "5, 1, 3", shape = box, style = rounded] [color = DimGray]
    153 [label = "5, 3, 1", shape = box, style = rounded] [color = DimGray]
    154 [label = "7, 3, 3", shape = box, style = rounded]
    155 [label = "0, 3, 3", shape = box, style = rounded]
    156 [label = "1, 1, 3", shape = box, style = rounded] [color = DimGray]
    157 [label = "1, 3, 1", shape = box, style = rounded] [color = DimGray]
    158 [label = "5, 7, 3", shape = box, style = rounded] [color = DimGray]
    159 [label = "5, 0, 3", shape = box, style = rounded] [color = DimGray]
    160 [label = "5, 1, 1", shape = box, style = rounded] [color = DimGray]
    161 [label = "5, 3, 9", shape = box, style = rounded] [color = DimGray]
    162 [label = "5, 3, 0", shape = box, style = rounded] [color = DimGray]
    163 [label = "7, 1, 3", shape = box, style = rounded]
    164 [label = "7, 3, 1", shape = box, style = rounded] [color = DimGray]
    165 [label = "2, 3, 3", shape = box, style = rounded] [color = DimGray]
    166 [label = "4, 3, 3", shape = box, style = rounded]
    167 [label = "0, 1, 3", shape = box, style = rounded] [color = DimGray]
    168 [label = "0, 3, 1", shape = box, style = rounded] [color = DimGray]
    169 [label = "1, 7, 3", shape = box, style = rounded] [color = DimGray]
    170 [label = "1, 0, 3", shape = box, style = rounded] [color = DimGray]
    171 [label = "1, 1, 1", shape = box, style = rounded] [color = DimGray]
    172 [label = "1, 3, 9", shape = box, style = rounded] [color = DimGray]
    173 [label = "1, 3, 0", shape = box, style = rounded] [color = DimGray]
    174 [label = "5, 7, 1", shape = box, style = rounded] [color = DimGray]
    175 [label = "5, 2, 3", shape = box, style = rounded] [color = DimGray]
    176 [label = "5, 4, 3", shape = box, style = rounded] [color = DimGray]
    177 [label = "5, 0, 1", shape = box, style = rounded] [color = DimGray]
    178 [label = "5, 1, 9", shape = box, style = rounded] [color = DimGray]
    179 [label = "5, 1, 0", shape = box, style = rounded] [color = DimGray]
    180 [label = "5, 3, 2", shape = box, style = rounded] [color = DimGray]
    181 [label = "5, 3, 4", shape = box, style = rounded] [color = DimGray]
    182 [label = "5, 3, 6", shape = box, style = rounded] [color = DimGray]
    183 [label = "7, 7, 3", shape = box, style = rounded]
    184 [label = "7, 0, 3", shape = box, style = rounded] [color = DimGray]
    185 [label = "7, 1, 1", shape = box, style = rounded] [color = DimGray]
    186 [label = "7, 3, 9", shape = box, style = rounded] [color = DimGray]
    187 [label = "7, 3, 0", shape = box, style = rounded] [color = DimGray]
    188 [label = "2, 1, 3", shape = box, style = rounded] [color = DimGray]
    189 [label = "2, 3, 1", shape = box, style = rounded] [color = DimGray]
    190 [label = "4, 1, 3", shape = box, style = rounded]
    191 [label = "4, 3, 1", shape = box, style = rounded] [color = DimGray]
    192 [label = "0, 7, 3", shape = box, style = rounded] [color = DimGray]
    193 [label = "0, 0, 3", shape = box, style = rounded] [color = DimGray]
    194 [label = "0, 1, 1", shape = box, style = rounded] [color = DimGray]
    195 [label = "0, 3, 9", shape = box, style = rounded] [color = DimGray]
    196 [label = "0, 3, 0", shape = box, style = rounded] [color = DimGray]
    197 [label = "1, 7, 1", shape = box, style = rounded] [color = DimGray]
    198 [label = "1, 2, 3", shape = box, style = rounded] [color = DimGray]
    199 [label = "1, 4, 3", shape = box, style = rounded] [color = DimGray]
    200 [label = "1, 0, 1", shape = box, style = rounded] [color = DimGray]
    201 [label = "1, 1, 9", shape = box, style = rounded] [color = DimGray]
    202 [label = "1, 1, 0", shape = box, style = rounded] [color = DimGray]
    203 [label = "1, 3, 2", shape = box, style = rounded] [color = DimGray]
    204 [label = "1, 3, 4", shape = box, style = rounded] [color = DimGray]
    205 [label = "1, 3, 6", shape = box, style = rounded] [color = DimGray]
    206 [label = "5, 7, 9", shape = box, style = rounded] [color = DimGray]
    207 [label = "5, 7, 0", shape = box, style = rounded] [color = DimGray]
    208 [label = "5, 2, 1", shape = box, style = rounded] [color = DimGray]
    209 [label = "5, 4, 1", shape = box, style = rounded] [color = DimGray]
    210 [label = "5, 0, 9", shape = box, style = rounded] [color = DimGray]
    211 [label = "5, 0, 0", shape = box, style = rounded] [color = DimGray]
    212 [label = "5, 1, 2", shape = box, style = rounded] [color = DimGray]
    213 [label = "5, 1, 4", shape = box, style = rounded] [color = DimGray]
    214 [label = "5, 1, 6", shape = box, style = rounded] [color = DimGray]
    215 [label = "7, 7, 1", shape = box, style = rounded]
    216 [label = "7, 2, 3", shape = box, style = rounded] [color = DimGray]
    217 [label = "7, 4, 3", shape = box, style = rounded] [color = DimGray]
    218 [label = "7, 0, 1", shape = box, style = rounded] [color = DimGray]
    219 [label = "7, 1, 9", shape = box, style = rounded] [color = DimGray]
    220 [label = "7, 1, 0", shape = box, style = rounded] [color = DimGray]
    221 [label = "7, 3, 2", shape = box, style = rounded] [color = DimGray]
    222 [label = "7, 3, 4", shape = box, style = rounded] [color = DimGray]
    223 [label = "7, 3, 6", shape = box, style = rounded] [color = DimGray]
    224 [label = "2, 7, 3", shape = box, style = rounded] [color = DimGray]
    225 [label = "2, 0, 3", shape = box, style = rounded] [color = DimGray]
    226 [label = "2, 1, 1", shape = box, style = rounded] [color = DimGray]
    227 [label = "2, 3, 9", shape = box, style = rounded] [color = DimGray]
    228 [label = "2, 3, 0", shape = box, style = rounded] [color = DimGray]
    229 [label = "4, 7, 3", shape = box, style = rounded] [color = DimGray]
    230 [label = "4, 0, 3", shape = box, style = rounded]
    231 [label = "4, 1, 1", shape = box, style = rounded] [color = DimGray]
    232 [label = "4, 3, 9", shape = box, style = rounded] [color = DimGray]
    233 [label = "4, 3, 0", shape = box, style = rounded] [color = DimGray]
    234 [label = "0, 7, 1", shape = box, style = rounded] [color = DimGray]
    235 [label = "0, 2, 3", shape = box, style = rounded] [color = DimGray]
    236 [label = "0, 4, 3", shape = box, style = rounded] [color = DimGray]
    237 [label = "0, 0, 1", shape = box, style = rounded] [color = DimGray]
    238 [label = "0, 1, 9", shape = box, style = rounded] [color = DimGray]
    239 [label = "0, 1, 0", shape = box, style = rounded] [color = DimGray]
    240 [label = "0, 3, 2", shape = box, style = rounded] [color = DimGray]
    241 [label = "0, 3, 4", shape = box, style = rounded] [color = DimGray]
    242 [label = "0, 3, 6", shape = box, style = rounded] [color = DimGray]
    243 [label = "1, 7, 9", shape = box, style = rounded] [color = DimGray]
    244 [label = "1, 7, 0", shape = box, style = rounded] [color = DimGray]
    245 [label = "1, 2, 1", shape = box, style = rounded] [color = DimGray]
    246 [label = "1, 4, 1", shape = box, style = rounded] [color = DimGray]
    247 [label = "1, 0, 9", shape = box, style = rounded] [color = DimGray]
    248 [label = "1, 0, 0", shape = box, style = rounded] [color = DimGray]
    249 [label = "1, 1, 2", shape = box, style = rounded] [color = DimGray]
    250 [label = "1, 1, 4", shape = box, style = rounded] [color = DimGray]
    251 [label = "1, 1, 6", shape = box, style = rounded] [color = DimGray]
    252 [label = "5, 7, 2", shape = box, style = rounded] [color = DimGray]
    253 [label = "5, 7, 4", shape = box, style = rounded] [color = DimGray]
    254 [label = "5, 7, 6", shape = box, style = rounded] [color = DimGray]
    255 [label = "5, 2, 9", shape = box, style = rounded] [color = DimGray]
    256 [label = "5, 2, 0", shape = box, style = rounded] [color = DimGray]
    257 [label = "5, 4, 9", shape = box, style = rounded] [color = DimGray]
    258 [label = "5, 4, 0", shape = box, style = rounded] [color = DimGray]
    259 [label = "5, 0, 2", shape = box, style = rounded] [color = DimGray]
    260 [label = "5, 0, 4", shape = box, style = rounded] [color = DimGray]
    261 [label = "5, 0, 6", shape = box, style = rounded] [color = DimGray]
    262 [label = "7, 2, 1", shape = box, style = rounded] [color = DimGray]
    263 [label = "7, 4, 1", shape = box, style = rounded] [color = DimGray]
    264 [label = "7, 1, 2", shape = box, style = rounded] [color = DimGray]
    265 [label = "7, 1, 4", shape = box, style = rounded] [color = DimGray]
    266 [label = "7, 1, 6", shape = box, style = rounded] [color = DimGray]
    267 [label = "2, 7, 1", shape = box, style = rounded] [color = DimGray]
    268 [label = "2, 2, 3", shape = box, style = rounded] [color = DimGray]
    269 [label = "2, 4, 3", shape = box, style = rounded] [color = DimGray]
    270 [label = "2, 0, 1", shape = box, style = rounded] [color = DimGray]
    271 [label = "2, 1, 9", shape = box, style = rounded] [color = DimGray]
    272 [label = "2, 1, 0", shape = box, style = rounded] [color = DimGray]
    273 [label = "2, 3, 2", shape = box, style = rounded] [color = DimGray]
    274 [label = "2, 3, 4", shape = box, style = rounded] [color = DimGray]
    275 [label = "2, 3, 6", shape = box, style = rounded] [color = DimGray]
    276 [label = "4, 7, 1", shape = box, style = rounded] [color = DimGray]
    277 [label = "4, 2, 3", shape = box, style = rounded]
    278 [label = "4, 4, 3", shape = box, style = rounded] [color = DimGray]
    279 [label = "4, 0, 1", shape = box, style = rounded] [color = DimGray]
    280 [label = "4, 1, 9", shape = box, style = rounded] [color = DimGray]
    281 [label = "4, 1, 0", shape = box, style = rounded] [color = DimGray]
    282 [label = "4, 3, 2", shape = box, style = rounded] [color = DimGray]
    283 [label = "4, 3, 4", shape = box, style = rounded] [color = DimGray]
    284 [label = "4, 3, 6", shape = box, style = rounded] [color = DimGray]
    285 [label = "0, 2, 1", shape = box, style = rounded] [color = DimGray]
    286 [label = "0, 4, 1", shape = box, style = rounded] [color = DimGray]
    287 [label = "0, 1, 2", shape = box, style = rounded] [color = DimGray]
    288 [label = "0, 1, 4", shape = box, style = rounded] [color = DimGray]
    289 [label = "0, 1, 6", shape = box, style = rounded] [color = DimGray]
    290 [label = "1, 7, 2", shape = box, style = rounded] [color = DimGray]
    291 [label = "1, 7, 4", shape = box, style = rounded] [color = DimGray]
    292 [label = "1, 7, 6", shape = box, style = rounded] [color = DimGray]
    293 [label = "1, 2, 9", shape = box, style = rounded] [color = DimGray]
    294 [label = "1, 2, 0", shape = box, style = rounded] [color = DimGray]
    295 [label = "1, 4, 9", shape = box, style = rounded] [color = DimGray]
    296 [label = "1, 4, 0", shape = box, style = rounded] [color = DimGray]
    297 [label = "1, 0, 2", shape = box, style = rounded] [color = DimGray]
    298 [label = "1, 0, 4", shape = box, style = rounded] [color = DimGray]
    299 [label = "1, 0, 6", shape = box, style = rounded] [color = DimGray]
    300 [label = "5, 2, 2", shape = box, style = rounded] [color = DimGray]
    301 [label = "5, 2, 4", shape = box, style = rounded] [color = DimGray]
    302 [label = "5, 2, 6", shape = box, style = rounded] [color = DimGray]
    303 [label = "5, 4, 2", shape = box, style = rounded] [color = DimGray]
    304 [label = "5, 4, 4", shape = box, style = rounded] [color = DimGray]
    305 [label = "5, 4, 6", shape = box, style = rounded] [color = DimGray]
    306 [label = "2, 2, 1", shape = box, style = rounded] [color = DimGray]
    307 [label = "2, 4, 1", shape = box, style = rounded] [color = DimGray]
    308 [label = "2, 1, 2", shape = box, style = rounded] [color = DimGray]
    309 [label = "2, 1, 4", shape = box, style = rounded] [color = DimGray]
    310 [label = "2, 1, 6", shape = box, style = rounded] [color = DimGray]
    311 [label = "4, 2, 1", shape = box, style = rounded]
    312 [label = "4, 4, 1", shape = box, style = rounded] [color = DimGray]
    313 [label = "4, 1, 2", shape = box, style = rounded] [color = DimGray]
    314 [label = "4, 1, 4", shape = box, style = rounded] [color = DimGray]
    315 [label = "4, 1, 6", shape = box, style = rounded] [color = DimGray]
    316 [label = "1, 2, 2", shape = box, style = rounded] [color = DimGray]
    317 [label = "1, 2, 4", shape = box, style = rounded] [color = DimGray]
    318 [label = "1, 2, 6", shape = box, style = rounded] [color = DimGray]
    319 [label = "1, 4, 2", shape = box, style = rounded] [color = DimGray]
    320 [label = "1, 4, 4", shape = box, style = rounded] [color = DimGray]
    321 [label = "1, 4, 6", shape = box, style = rounded] [color = DimGray]
  }
  I0 -> 0
  0 -> 1 [label = "\\e"]
  0 -> 2 [label = "\\e"]
  0 -> 3 [label = "\\e", color = DimGray]
  0 -> 4 [label = "\\e", color = DimGray]
  0 -> 5 [label = "\\e", color = DimGray]
  0 -> 6 [label = "\\e", color = DimGray]
  1 -> 7 [label = "\\e", color = DimGray]
  1 -> 8 [label = "\\e"]
  1 -> 9 [label = "\\e", color = DimGray]
  1 -> 10 [label = "\\e", color = DimGray]
  1 -> 11 [label = "\\e", color = DimGray]
  1 -> 12 [label = "\\e", color = DimGray]
  2 -> 13 [label = "\\e", color = DimGray]
  2 -> 14 [label = "\\e"]
  2 -> 15 [label = "\\e", color = DimGray]
  2 -> 16 [label = "\\e", color = DimGray]
  3 -> 17 [label = "\\e", color = DimGray]
  3 -> 18 [label = "\\e", color = DimGray]
  3 -> 19 [label = "\\e", color = DimGray]
  3 -> 20 [label = "\\e", color = DimGray]
  4 -> 21 [label = "\\e", color = DimGray]
  4 -> 22 [label = "\\e", color = DimGray]
  5 -> 23 [label = "\\e", color = DimGray]
  5 -> 24 [label = "\\e", color = DimGray]
  5 -> 25 [label = "\\e", color = DimGray]
  7 -> 26 [label = "\\e", color = DimGray]
  7 -> 27 [label = "\\e", color = DimGray]
  7 -> 28 [label = "\\e", color = DimGray]
  7 -> 29 [label = "\\e", color = DimGray]
  8 -> 30 [label = "\\e"]
  8 -> 31 [label = "\\e", color = DimGray]
  8 -> 32 [label = "\\e", color = DimGray]
  8 -> 33 [label = "\\e", color = DimGray]
  9 -> 34 [label = "\\e", color = DimGray]
  9 -> 35 [label = "\\e", color = DimGray]
  9 -> 36 [label = "\\e", color = DimGray]
  9 -> 37 [label = "\\e", color = DimGray]
  10 -> 38 [label = "\\e", color = DimGray]
  10 -> 39 [label = "\\e", color = DimGray]
  11 -> 40 [label = "\\e", color = DimGray]
  11 -> 41 [label = "\\e", color = DimGray]
  11 -> 42 [label = "\\e", color = DimGray]
  13 -> 43 [label = "\\e", color = DimGray]
  13 -> 44 [label = "\\e", color = DimGray]
  13 -> 45 [label = "\\e", color = DimGray]
  13 -> 46 [label = "\\e", color = DimGray]
  14 -> 47 [label = "\\e", color = DimGray]
  14 -> 48 [label = "\\e"]
  15 -> 49 [label = "\\e", color = DimGray]
  15 -> 50 [label = "\\e", color = DimGray]
  15 -> 51 [label = "\\e", color = DimGray]
  17 -> 52 [label = "\\e", color = DimGray]
  17 -> 53 [label = "\\e", color = DimGray]
  18 -> 54 [label = "\\e", color = DimGray]
  18 -> 55 [label = "\\e", color = DimGray]
  19 -> 56 [label = "\\e", color = DimGray]
  19 -> 57 [label = "\\e", color = DimGray]
  19 -> 58 [label = "\\e", color = DimGray]
  21 -> 59 [label = "\\e", color = DimGray]
  21 -> 60 [label = "\\e", color = DimGray]
  21 -> 61 [label = "\\e", color = DimGray]
  26 -> 62 [label = "\\e", color = DimGray]
  26 -> 63 [label = "\\e", color = DimGray]
  26 -> 64 [label = "\\e", color = DimGray]
  26 -> 65 [label = "\\e", color = DimGray]
  27 -> 66 [label = "\\e", color = DimGray]
  27 -> 67 [label = "\\e", color = DimGray]
  28 -> 68 [label = "\\e", color = DimGray]
  28 -> 69 [label = "\\e", color = DimGray]
  28 -> 70 [label = "\\e", color = DimGray]
  30 -> 71 [label = "\\e"]
  30 -> 72 [label = "\\e", color = DimGray]
  30 -> 73 [label = "\\e", color = DimGray]
  30 -> 74 [label = "\\e", color = DimGray]
  31 -> 75 [label = "\\e", color = DimGray]
  31 -> 76 [label = "\\e", color = DimGray]
  32 -> 77 [label = "\\e", color = DimGray]
  32 -> 78 [label = "\\e", color = DimGray]
  32 -> 79 [label = "\\e", color = DimGray]
  34 -> 80 [label = "\\e", color = DimGray]
  34 -> 81 [label = "\\e", color = DimGray]
  35 -> 82 [label = "\\e", color = DimGray]
  35 -> 83 [label = "\\e", color = DimGray]
  36 -> 84 [label = "\\e", color = DimGray]
  36 -> 85 [label = "\\e", color = DimGray]
  36 -> 86 [label = "\\e", color = DimGray]
  38 -> 87 [label = "\\e", color = DimGray]
  38 -> 88 [label = "\\e", color = DimGray]
  38 -> 89 [label = "\\e", color = DimGray]
  43 -> 90 [label = "\\e", color = DimGray]
  43 -> 91 [label = "\\e", color = DimGray]
  44 -> 92 [label = "\\e", color = DimGray]
  44 -> 93 [label = "\\e", color = DimGray]
  45 -> 94 [label = "\\e", color = DimGray]
  45 -> 95 [label = "\\e", color = DimGray]
  45 -> 96 [label = "\\e", color = DimGray]
  47 -> 97 [label = "\\e", color = DimGray]
  47 -> 98 [label = "\\e", color = DimGray]
  47 -> 99 [label = "\\e", color = DimGray]
  48 -> F48
  52 -> 100 [label = "\\e", color = DimGray]
  52 -> 101 [label = "\\e", color = DimGray]
  52 -> 102 [label = "\\e", color = DimGray]
  54 -> 103 [label = "\\e", color = DimGray]
  54 -> 104 [label = "\\e", color = DimGray]
  54 -> 105 [label = "\\e", color = DimGray]
  62 -> 106 [label = "\\e", color = DimGray]
  62 -> 107 [label = "\\e", color = DimGray]
  63 -> 108 [label = "\\e", color = DimGray]
  63 -> 109 [label = "\\e", color = DimGray]
  64 -> 110 [label = "\\e", color = DimGray]
  64 -> 111 [label = "\\e", color = DimGray]
  64 -> 112 [label = "\\e", color = DimGray]
  66 -> 113 [label = "\\e", color = DimGray]
  66 -> 114 [label = "\\e", color = DimGray]
  66 -> 115 [label = "\\e", color = DimGray]
  71 -> 116 [label = "\\e"]
  71 -> 117 [label = "\\e", color = DimGray]
  72 -> 118 [label = "\\e", color = DimGray]
  72 -> 119 [label = "\\e", color = DimGray]
  73 -> 120 [label = "\\e", color = DimGray]
  73 -> 121 [label = "\\e", color = DimGray]
  73 -> 122 [label = "\\e", color = DimGray]
  75 -> 123 [label = "\\e", color = DimGray]
  75 -> 124 [label = "\\e", color = DimGray]
  75 -> 125 [label = "\\e", color = DimGray]
  80 -> 126 [label = "\\e", color = DimGray]
  80 -> 127 [label = "\\e", color = DimGray]
  80 -> 128 [label = "\\e", color = DimGray]
  82 -> 129 [label = "\\e", color = DimGray]
  82 -> 130 [label = "\\e", color = DimGray]
  82 -> 131 [label = "\\e", color = DimGray]
  90 -> 132 [label = "\\e", color = DimGray]
  90 -> 133 [label = "\\e", color = DimGray]
  90 -> 134 [label = "\\e", color = DimGray]
  92 -> 135 [label = "\\e", color = DimGray]
  92 -> 136 [label = "\\e", color = DimGray]
  92 -> 137 [label = "\\e", color = DimGray]
  106 -> 138 [label = "\\e", color = DimGray]
  106 -> 139 [label = "\\e", color = DimGray]
  106 -> 140 [label = "\\e", color = DimGray]
  108 -> 141 [label = "\\e", color = DimGray]
  108 -> 142 [label = "\\e", color = DimGray]
  108 -> 143 [label = "\\e", color = DimGray]
  116 -> 144 [label = "\\e"]
  116 -> 145 [label = "\\e", color = DimGray]
  116 -> 146 [label = "\\e", color = DimGray]
  118 -> 147 [label = "\\e", color = DimGray]
  118 -> 148 [label = "\\e", color = DimGray]
  118 -> 149 [label = "\\e", color = DimGray]
  144 -> 150 [label = "b"]
  150 -> 151 [label = "\\e"]
  150 -> 152 [label = "\\e", color = DimGray]
  150 -> 153 [label = "\\e", color = DimGray]
  151 -> 154 [label = "\\e"]
  151 -> 155 [label = "\\e"]
  151 -> 156 [label = "\\e", color = DimGray]
  151 -> 157 [label = "\\e", color = DimGray]
  152 -> 158 [label = "\\e", color = DimGray]
  152 -> 159 [label = "\\e", color = DimGray]
  152 -> 160 [label = "\\e", color = DimGray]
  153 -> 161 [label = "\\e", color = DimGray]
  153 -> 162 [label = "\\e", color = DimGray]
  154 -> 163 [label = "\\e"]
  154 -> 164 [label = "\\e", color = DimGray]
  155 -> 165 [label = "\\e", color = DimGray]
  155 -> 166 [label = "\\e"]
  155 -> 167 [label = "\\e", color = DimGray]
  155 -> 168 [label = "\\e", color = DimGray]
  156 -> 169 [label = "\\e", color = DimGray]
  156 -> 170 [label = "\\e", color = DimGray]
  156 -> 171 [label = "\\e", color = DimGray]
  157 -> 172 [label = "\\e", color = DimGray]
  157 -> 173 [label = "\\e", color = DimGray]
  158 -> 174 [label = "\\e", color = DimGray]
  159 -> 175 [label = "\\e", color = DimGray]
  159 -> 176 [label = "\\e", color = DimGray]
  159 -> 177 [label = "\\e", color = DimGray]
  160 -> 178 [label = "\\e", color = DimGray]
  160 -> 179 [label = "\\e", color = DimGray]
  162 -> 180 [label = "\\e", color = DimGray]
  162 -> 181 [label = "\\e", color = DimGray]
  162 -> 182 [label = "\\e", color = DimGray]
  163 -> 183 [label = "\\e"]
  163 -> 184 [label = "\\e", color = DimGray]
  163 -> 185 [label = "\\e", color = DimGray]
  164 -> 186 [label = "\\e", color = DimGray]
  164 -> 187 [label = "\\e", color = DimGray]
  165 -> 188 [label = "\\e", color = DimGray]
  165 -> 189 [label = "\\e", color = DimGray]
  166 -> 190 [label = "\\e"]
  166 -> 191 [label = "\\e", color = DimGray]
  167 -> 192 [label = "\\e", color = DimGray]
  167 -> 193 [label = "\\e", color = DimGray]
  167 -> 194 [label = "\\e", color = DimGray]
  168 -> 195 [label = "\\e", color = DimGray]
  168 -> 196 [label = "\\e", color = DimGray]
  169 -> 197 [label = "\\e", color = DimGray]
  170 -> 198 [label = "\\e", color = DimGray]
  170 -> 199 [label = "\\e", color = DimGray]
  170 -> 200 [label = "\\e", color = DimGray]
  171 -> 201 [label = "\\e", color = DimGray]
  171 -> 202 [label = "\\e", color = DimGray]
  173 -> 203 [label = "\\e", color = DimGray]
  173 -> 204 [label = "\\e", color = DimGray]
  173 -> 205 [label = "\\e", color = DimGray]
  174 -> 206 [label = "\\e", color = DimGray]
  174 -> 207 [label = "\\e", color = DimGray]
  175 -> 208 [label = "\\e", color = DimGray]
  176 -> 209 [label = "\\e", color = DimGray]
  177 -> 210 [label = "\\e", color = DimGray]
  177 -> 211 [label = "\\e", color = DimGray]
  179 -> 212 [label = "\\e", color = DimGray]
  179 -> 213 [label = "\\e", color = DimGray]
  179 -> 214 [label = "\\e", color = DimGray]
  183 -> 215 [label = "\\e"]
  184 -> 216 [label = "\\e", color = DimGray]
  184 -> 217 [label = "\\e", color = DimGray]
  184 -> 218 [label = "\\e", color = DimGray]
  185 -> 219 [label = "\\e", color = DimGray]
  185 -> 220 [label = "\\e", color = DimGray]
  187 -> 221 [label = "\\e", color = DimGray]
  187 -> 222 [label = "\\e", color = DimGray]
  187 -> 223 [label = "\\e", color = DimGray]
  188 -> 224 [label = "\\e", color = DimGray]
  188 -> 225 [label = "\\e", color = DimGray]
  188 -> 226 [label = "\\e", color = DimGray]
  189 -> 227 [label = "\\e", color = DimGray]
  189 -> 228 [label = "\\e", color = DimGray]
  190 -> 229 [label = "\\e", color = DimGray]
  190 -> 230 [label = "\\e"]
  190 -> 231 [label = "\\e", color = DimGray]
  191 -> 232 [label = "\\e", color = DimGray]
  191 -> 233 [label = "\\e", color = DimGray]
  192 -> 234 [label = "\\e", color = DimGray]
  193 -> 235 [label = "\\e", color = DimGray]
  193 -> 236 [label = "\\e", color = DimGray]
  193 -> 237 [label = "\\e", color = DimGray]
  194 -> 238 [label = "\\e", color = DimGray]
  194 -> 239 [label = "\\e", color = DimGray]
  196 -> 240 [label = "\\e", color = DimGray]
  196 -> 241 [label = "\\e", color = DimGray]
  196 -> 242 [label = "\\e", color = DimGray]
  197 -> 243 [label = "\\e", color = DimGray]
  197 -> 244 [label = "\\e", color = DimGray]
  198 -> 245 [label = "\\e", color = DimGray]
  199 -> 246 [label = "\\e", color = DimGray]
  200 -> 247 [label = "\\e", color = DimGray]
  200 -> 248 [label = "\\e", color = DimGray]
  202 -> 249 [label = "\\e", color = DimGray]
  202 -> 250 [label = "\\e", color = DimGray]
  202 -> 251 [label = "\\e", color = DimGray]
  207 -> 252 [label = "\\e", color = DimGray]
  207 -> 253 [label = "\\e", color = DimGray]
  207 -> 254 [label = "\\e", color = DimGray]
  208 -> 255 [label = "\\e", color = DimGray]
  208 -> 256 [label = "\\e", color = DimGray]
  209 -> 257 [label = "\\e", color = DimGray]
  209 -> 258 [label = "\\e", color = DimGray]
  211 -> 259 [label = "\\e", color = DimGray]
  211 -> 260 [label = "\\e", color = DimGray]
  211 -> 261 [label = "\\e", color = DimGray]
  215 -> 47 [label = "\\e", color = DimGray]
  215 -> 48 [label = "\\e"]
  216 -> 262 [label = "\\e", color = DimGray]
  217 -> 263 [label = "\\e", color = DimGray]
  218 -> 45 [label = "\\e", color = DimGray]
  218 -> 46 [label = "\\e", color = DimGray]
  220 -> 264 [label = "\\e", color = DimGray]
  220 -> 265 [label = "\\e", color = DimGray]
  220 -> 266 [label = "\\e", color = DimGray]
  224 -> 267 [label = "\\e", color = DimGray]
  225 -> 268 [label = "\\e", color = DimGray]
  225 -> 269 [label = "\\e", color = DimGray]
  225 -> 270 [label = "\\e", color = DimGray]
  226 -> 271 [label = "\\e", color = DimGray]
  226 -> 272 [label = "\\e", color = DimGray]
  228 -> 273 [label = "\\e", color = DimGray]
  228 -> 274 [label = "\\e", color = DimGray]
  228 -> 275 [label = "\\e", color = DimGray]
  229 -> 276 [label = "\\e", color = DimGray]
  230 -> 277 [label = "\\e"]
  230 -> 278 [label = "\\e", color = DimGray]
  230 -> 279 [label = "\\e", color = DimGray]
  231 -> 280 [label = "\\e", color = DimGray]
  231 -> 281 [label = "\\e", color = DimGray]
  233 -> 282 [label = "\\e", color = DimGray]
  233 -> 283 [label = "\\e", color = DimGray]
  233 -> 284 [label = "\\e", color = DimGray]
  234 -> 38 [label = "\\e", color = DimGray]
  234 -> 39 [label = "\\e", color = DimGray]
  235 -> 285 [label = "\\e", color = DimGray]
  236 -> 286 [label = "\\e", color = DimGray]
  237 -> 36 [label = "\\e", color = DimGray]
  237 -> 37 [label = "\\e", color = DimGray]
  239 -> 287 [label = "\\e", color = DimGray]
  239 -> 288 [label = "\\e", color = DimGray]
  239 -> 289 [label = "\\e", color = DimGray]
  244 -> 290 [label = "\\e", color = DimGray]
  244 -> 291 [label = "\\e", color = DimGray]
  244 -> 292 [label = "\\e", color = DimGray]
  245 -> 293 [label = "\\e", color = DimGray]
  245 -> 294 [label = "\\e", color = DimGray]
  246 -> 295 [label = "\\e", color = DimGray]
  246 -> 296 [label = "\\e", color = DimGray]
  248 -> 297 [label = "\\e", color = DimGray]
  248 -> 298 [label = "\\e", color = DimGray]
  248 -> 299 [label = "\\e", color = DimGray]
  256 -> 300 [label = "\\e", color = DimGray]
  256 -> 301 [label = "\\e", color = DimGray]
  256 -> 302 [label = "\\e", color = DimGray]
  258 -> 303 [label = "\\e", color = DimGray]
  258 -> 304 [label = "\\e", color = DimGray]
  258 -> 305 [label = "\\e", color = DimGray]
  262 -> 90 [label = "\\e", color = DimGray]
  262 -> 91 [label = "\\e", color = DimGray]
  263 -> 92 [label = "\\e", color = DimGray]
  263 -> 93 [label = "\\e", color = DimGray]
  267 -> 66 [label = "\\e", color = DimGray]
  267 -> 67 [label = "\\e", color = DimGray]
  268 -> 306 [label = "\\e", color = DimGray]
  269 -> 307 [label = "\\e", color = DimGray]
  270 -> 64 [label = "\\e", color = DimGray]
  270 -> 65 [label = "\\e", color = DimGray]
  272 -> 308 [label = "\\e", color = DimGray]
  272 -> 309 [label = "\\e", color = DimGray]
  272 -> 310 [label = "\\e", color = DimGray]
  276 -> 75 [label = "\\e", color = DimGray]
  276 -> 76 [label = "\\e", color = DimGray]
  277 -> 311 [label = "\\e"]
  278 -> 312 [label = "\\e", color = DimGray]
  279 -> 73 [label = "\\e", color = DimGray]
  279 -> 74 [label = "\\e", color = DimGray]
  281 -> 313 [label = "\\e", color = DimGray]
  281 -> 314 [label = "\\e", color = DimGray]
  281 -> 315 [label = "\\e", color = DimGray]
  285 -> 80 [label = "\\e", color = DimGray]
  285 -> 81 [label = "\\e", color = DimGray]
  286 -> 82 [label = "\\e", color = DimGray]
  286 -> 83 [label = "\\e", color = DimGray]
  294 -> 316 [label = "\\e", color = DimGray]
  294 -> 317 [label = "\\e", color = DimGray]
  294 -> 318 [label = "\\e", color = DimGray]
  296 -> 319 [label = "\\e", color = DimGray]
  296 -> 320 [label = "\\e", color = DimGray]
  296 -> 321 [label = "\\e", color = DimGray]
  306 -> 106 [label = "\\e", color = DimGray]
  306 -> 107 [label = "\\e", color = DimGray]
  307 -> 108 [label = "\\e", color = DimGray]
  307 -> 109 [label = "\\e", color = DimGray]
  311 -> 116 [label = "\\e"]
  311 -> 117 [label = "\\e", color = DimGray]
  312 -> 118 [label = "\\e", color = DimGray]
  312 -> 119 [label = "\\e", color = DimGray]
}'''
CHECK_EQ(res, str(lhs & rhs & third))
check_equivalent(vcsn.automaton(res).proper(),
                 vcsn.context("lal_char(b)_b").ratexp("b*").standard())
