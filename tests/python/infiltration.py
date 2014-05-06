#! /usr/bin/env python

import vcsn
from test import *

b = vcsn.context('lal_char(abcd)_b')

medir = srcdir + '/tests/python/product.dir'

## ---------------------- ##
## Existing transitions.  ##
## ---------------------- ##

# See the actual code of product to understand the point of this test
# (which is new_transition vs. add_transition).
a1 = b.ratexp('a*a').derived_term()
# FIXME: ABORT
### CHECK_EQ('a*(a+(a+a)a*a+(a+a)a*a)', str(a1.infiltration(a1).ratexp()))

## -------------------- ##
## Hand crafted tests.  ##
## -------------------- ##

# a infiltration a
a = vcsn.automaton.load(medir + "/a.gv")
CHECK_EQ(vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(a)_z"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F1
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
  0 -> 2 [label = "a"]
  0 -> 3 [label = "a"]
  1 -> F1
  2 -> 1 [label = "a"]
  3 -> 1 [label = "a"]
}
'''),  a.infiltration(a))

# abc infiltration abc
abc = vcsn.automaton.load(medir + "/abc.gv")
CHECK_EQ(vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abc)_z"
  rankdir = LR
  {
    node [shape = point, width = 0]
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
    10
    11
    12
    13
    14
    15
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "a"]
  0 -> 3 [label = "a"]
  1 -> 4 [label = "b"]
  1 -> 5 [label = "b"]
  1 -> 6 [label = "b"]
  2 -> 1 [label = "a"]
  2 -> 7 [label = "b"]
  3 -> 1 [label = "a"]
  3 -> 8 [label = "b"]
  4 -> 9 [label = "c"]
  4 -> 10 [label = "c"]
  4 -> 11 [label = "c"]
  5 -> 4 [label = "b"]
  5 -> 12 [label = "c"]
  6 -> 4 [label = "b"]
  6 -> 13 [label = "c"]
  7 -> 5 [label = "a"]
  7 -> 14 [label = "c"]
  8 -> 6 [label = "a"]
  8 -> 15 [label = "c"]
  9 -> F9
  10 -> 9 [label = "c"]
  11 -> 9 [label = "c"]
  12 -> 10 [label = "b"]
  13 -> 11 [label = "b"]
  14 -> 12 [label = "a"]
  15 -> 13 [label = "a"]
}
'''), abc.infiltration(abc))

# abc infiltration xy
xy = vcsn.automaton.load(medir + "/xy.gv")
CHECK_EQ(vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abcxy)_z"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F11
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
  0 -> 1 [label = "a"]
  0 -> 2 [label = "x"]
  1 -> 3 [label = "b"]
  1 -> 4 [label = "x"]
  2 -> 4 [label = "a"]
  2 -> 5 [label = "y"]
  3 -> 6 [label = "c"]
  3 -> 7 [label = "x"]
  4 -> 7 [label = "b"]
  4 -> 8 [label = "y"]
  5 -> 8 [label = "a"]
  6 -> 9 [label = "x"]
  7 -> 9 [label = "c"]
  7 -> 10 [label = "y"]
  8 -> 10 [label = "b"]
  9 -> 11 [label = "y"]
  10 -> 11 [label = "c"]
  11 -> F11
}
'''), abc.infiltration(xy))


## --------------------- ##
## Heterogeneous input.  ##
## --------------------- ##

# RatE and B, in both directions.
ab = vcsn.context('lal_char(ab)_ratexpset<lal_char(uv)_b>') \
    .ratexp('(<u>a+<v>b)*').standard()
a = vcsn.context('lal_char(ab)_b').ratexp('a').standard()
# FIXME: ABORT
### CHECK_EQ('<u+\e>a + <uu+uu+u+u>aa + <uv+v>ab + <vu+v>ba',
###     str(ab.infiltration(a).shortest(4)))
### CHECK_EQ('<u+\e>a + <uu+u+uu+u>aa + <uv+v>ab + <vu+v>ba',
###     str(a.infiltration(ab).shortest(4)))


## ----------------- ##
## Non-commutative.  ##
## ----------------- ##

uavb = vcsn.context('lal_char(ab)_ratexpset<lal_char(uv)_b>') \
    .ratexp('<u>a<v>b').standard()
xayb = vcsn.context('lal_char(ab)_ratexpset<lal_char(xy)_b>') \
    .ratexp('<x>a<y>b').standard()
CHECK_EQ('<uxvy>ab + <uxvy+xuvy>aab + <uxvy+uxyv>abb + <uxvy+uxyv+xuvy+xuyv>aabb + <uvxy+xyuv>abab',
    str(uavb.infiltration(xayb).enumerate(4)))


## ------- ##
## n-ary.  ##
## ------- ##

ctx = vcsn.context('lal_char(x)_ratexpset<lal_char(abcd)_b>')
a = dict()
for l in ['a', 'b', 'c', 'd']:
    a[l] = ctx.ratexp("<{}>x".format(l)).standard()
CHECK_EQ('''<abcd>x
<acdb+bcda+abdc+adbc+bdac+cdab+abcd+acbd+bcad+abcd+abcd+bacd+cabd+dabc>xx
<adbc+adcb+bdac+bdca+cdab+cdba+acbd+acdb+bcad+bcda+abcd+abdc+abdc+acdb+abcd+abcd+acbd+adbc+badc+bcda+bacd+bacd+bcad+bdac+cadb+cbda+cabd+cabd+cbad+cdab+dacb+dbca+dabc+dabc+dbac+dcab>xxx
<abcd+abdc+acbd+acdb+adbc+adcb+bacd+badc+bcad+bcda+bdac+bdca+cabd+cadb+cbad+cbda+cdab+cdba+dabc+dacb+dbac+dbca+dcab+dcba>xxxx''',
    a['a']
    .infiltration(a['b'])
    .infiltration(a['c'])
    .infiltration(a['d'])
    .enumerate(10)
    .format('list'))
