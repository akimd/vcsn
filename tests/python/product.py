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
CHECK_EQ(vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(b)_b"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F0
    F1
  }
  {
    node [shape = circle]
    0
    1
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "b"]
  1 -> F1
  1 -> 1 [label = "b"]
}
'''),
         lhs & rhs)

## ------------- ##
## ab x cd = 0.  ##
## ------------- ##

lhs = vcsn.context('lal_char(ab)_b').ratexp('ab').standard()
rhs = vcsn.context('lal_char(cd)_b').ratexp('cd').standard()
CHECK_EQ(vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char()_b"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
  }
  {
    node [shape = circle]
    0 [color = DimGray]
  }
  I0 -> 0 [color = DimGray]
}
'''), lhs & rhs)



## ---------------------- ##
## (a+b)* x (c+d)* = \e.  ##
## ---------------------- ##

lhs = vcsn.context('lal_char(ab)_b').ratexp('(a+b)*').standard()
rhs = vcsn.context('lal_char(cd)_b').ratexp('(c+d)*').standard()
CHECK_EQ(vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char()_b"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F0
  }
  {
    node [shape = circle]
    0
  }
  I0 -> 0
  0 -> F0
}
'''), 
         lhs & rhs)



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
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F0
    F1
    F2
  }
  {
    node [shape = circle]
    0
    1
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
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F0
  }
  {
    node [shape = circle]
    0
    1
  }
  I0 -> 0 [label = "<3>"]
  0 -> F0
  0 -> 1 [label = "a"]
  1 -> 0 [label = "<3>b"]
}
''')

exp = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abc)_z"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F0
    F3
  }
  {
    node [shape = circle]
    0
    1
    2 [color = DimGray]
    3
  }
  I0 -> 0 [label = "<6>"]
  0 -> F0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "a", color = DimGray]
  1 -> 3 [label = "<3>b"]
  3 -> F3
  3 -> 2 [label = "a", color = DimGray]
}
''')

CHECK_EQ(exp, lhs&rhs)

## --------------------- ##
## Heterogeneous input.  ##
## --------------------- ##

# check OPERATION RES AUT...
# --------------------------
def check(operation, exp, *args):
    CHECK_EQ(exp, str(reduce(lambda l, r: l & r, args).ratexp()))

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
CHECK_EQ('<uxvy>ab', str((a1&a2).enumerate(4)))


## ------- ##
## n-ary.  ##
## ------- ##

ctx = vcsn.context('lal_char(x)_ratexpset<lal_char(abcd)_b>')
a = dict()
for l in ['a', 'b', 'c', 'd']:
    a[l] = ctx.ratexp("<{}>x".format(l)).standard()
CHECK_EQ('<abcd>x', str((a['a'] & a['b'] & a['c'] & a['d']).enumerate(10)))

PLAN()
