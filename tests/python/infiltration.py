#! /usr/bin/env python

import vcsn
from test import *

b = vcsn.context('lal_char(abcd), b')

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
a = vcsn.automaton(filename = medir + "/a.gv")
CHECK_EQ('''digraph
{
  vcsn_context = "lal_char(a), z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F1
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "0, 0", shape = box]
    1 [label = "1, 1", shape = box]
    2 [label = "1, 0", shape = box]
    3 [label = "0, 1", shape = box]
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "a"]
  0 -> 3 [label = "a"]
  1 -> F1
  2 -> 1 [label = "a"]
  3 -> 1 [label = "a"]
}''',  a.infiltration(a))

# abc infiltration abc
abc = vcsn.automaton(filename = medir + "/abc.gv")
CHECK_EQ('''digraph
{
  vcsn_context = "lal_char(abc), z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F9
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "0, 0", shape = box]
    1 [label = "1, 1", shape = box]
    2 [label = "1, 0", shape = box]
    3 [label = "0, 1", shape = box]
    4 [label = "2, 2", shape = box]
    5 [label = "2, 1", shape = box]
    6 [label = "1, 2", shape = box]
    7 [label = "2, 0", shape = box]
    8 [label = "0, 2", shape = box]
    9 [label = "3, 3", shape = box]
    10 [label = "3, 2", shape = box]
    11 [label = "2, 3", shape = box]
    12 [label = "3, 1", shape = box]
    13 [label = "1, 3", shape = box]
    14 [label = "3, 0", shape = box]
    15 [label = "0, 3", shape = box]
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
}''', abc.infiltration(abc))

# abc infiltration xy
xy = vcsn.automaton(filename = medir + "/xy.gv")
CHECK_EQ('''digraph
{
  vcsn_context = "lal_char(abcxy), z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F11
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "0, 0", shape = box]
    1 [label = "1, 0", shape = box]
    2 [label = "0, 1", shape = box]
    3 [label = "2, 0", shape = box]
    4 [label = "1, 1", shape = box]
    5 [label = "0, 2", shape = box]
    6 [label = "3, 0", shape = box]
    7 [label = "2, 1", shape = box]
    8 [label = "1, 2", shape = box]
    9 [label = "3, 1", shape = box]
    10 [label = "2, 2", shape = box]
    11 [label = "3, 2", shape = box]
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
}''', abc.infiltration(xy))


## --------------------- ##
## Heterogeneous input.  ##
## --------------------- ##

# RatE and B, in both directions.
ab = vcsn.context('lal_char(ab), ratexpset<lal_char(uv), b>') \
    .ratexp('(<u>a+<v>b)*').standard()
a = vcsn.context('lal_char(ab), b').ratexp('a').standard()
# FIXME: ABORT
### CHECK_EQ('<u+\e>a + <uu+uu+u+u>aa + <uv+v>ab + <vu+v>ba',
###     str(ab.infiltration(a).shortest(4)))
### CHECK_EQ('<u+\e>a + <uu+u+uu+u>aa + <uv+v>ab + <vu+v>ba',
###     str(a.infiltration(ab).shortest(4)))


## ----------------- ##
## Non-commutative.  ##
## ----------------- ##

uavb = vcsn.context('lal_char(ab), ratexpset<lal_char(uv), b>') \
    .ratexp('<u>a<v>b').standard()
xayb = vcsn.context('lal_char(ab), ratexpset<lal_char(xy), b>') \
    .ratexp('<x>a<y>b').standard()
CHECK_EQ('<uxvy>ab + <uxvy+xuvy>aab + <uxvy+uxyv>abb + <uxvy+uxyv+xuvy+xuyv>aabb + <uvxy+xyuv>abab',
    str(uavb.infiltration(xayb).enumerate(4)))


## --------------------------------------------- ##
## n-ary: not yet implemented for infiltration.  ##
## --------------------------------------------- ##

ctx = vcsn.context('lal_char(x), seriesset<lal_char(abcd), b>')
a = dict()
for l in ['a', 'b', 'c', 'd']:
    a[l] = ctx.ratexp("<{}>x".format(l)).standard()

CHECK_EQ('''<abcd>x
<abcd+abdc+acbd+acdb+adbc+bacd+bcad+bcda+bdac+cabd+cdab+dabc>xx
<abcd+abdc+acbd+acdb+adbc+adcb+bacd+badc+bcad+bcda+bdac+bdca+cabd+cadb+cbad+cbda+cdab+cdba+dabc+dacb+dbac+dbca+dcab>xxx
<abcd+abdc+acbd+acdb+adbc+adcb+bacd+badc+bcad+bcda+bdac+bdca+cabd+cadb+cbad+cbda+cdab+cdba+dabc+dacb+dbac+dbca+dcab+dcba>xxxx''',
         a['a']
         .infiltration(a['b']).strip()
         .infiltration(a['c']).strip()
         .infiltration(a['d']).strip()
         .enumerate(10)
         .format('list'))
