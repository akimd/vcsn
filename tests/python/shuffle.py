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
CHECK_EQ('a*(aa*a+aa*a)', str(a1.shuffle(a1).ratexp()))


## ------------------------ ##
## (ab)* )( (<-1>ab)* = \e. ##
## ------------------------ ##
# TAFKIT manual, Figure 3.13, left [as of 2013-10-10]

abs = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab)_z"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F0
    F2
  }
  {
    node [shape = circle]
    0
    1
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "a"]
  1 -> 0 [label = "b"]
}''')

mabs = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab)_z"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F0
    F2
  }
  {
    node [shape = circle]
    0
    1
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "<-1>a"]
  1 -> 0 [label = "<1>b"]
}
''')

CHECK_EQ(vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab)_z"
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
    2
    3
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "<-1>a"]
  1 -> 0 [label = "b"]
  1 -> 3 [label = "<-1>a"]
  2 -> 0 [label = "b"]
  2 -> 3 [label = "a"]
  3 -> 1 [label = "b"]
  3 -> 2 [label = "b"]
}
'''), abs.shuffle(mabs))


## ---------------------- ##
## (a)* )( (<-1>a)* = \e. ##
## ---------------------- ##
# TAFKIT manual, Figure 3.13, right [as of 2013-10-10]

pas = vcsn.context("lal_char(a)_z").ratexp('a*').derived_term()
mas = vcsn.context("lal_char(a)_z").ratexp('(<-1>a)*').derived_term()
CHECK_EQ(vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(a)_z"
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
'''), pas.shuffle(mas))


## ---------- ##
## ABC )( ab. ##
## ---------- ##

ab = vcsn.context('lal_char(ab)_z').ratexp('ab').standard()
ABC = vcsn.context('lal_char(ABC)_z').ratexp('ABC').standard()
ABCab = ABC.shuffle(ab)
CHECK_EQ(vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ABCab)_z"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
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
  0 -> 1 [label = "A"]
  0 -> 2 [label = "a"]
  1 -> 3 [label = "B"]
  1 -> 4 [label = "a"]
  2 -> 4 [label = "A"]
  2 -> 5 [label = "b"]
  3 -> 6 [label = "C"]
  3 -> 7 [label = "a"]
  4 -> 7 [label = "B"]
  4 -> 8 [label = "b"]
  5 -> 8 [label = "A"]
  6 -> 9 [label = "a"]
  7 -> 9 [label = "C"]
  7 -> 10 [label = "b"]
  8 -> 10 [label = "B"]
  9 -> 11 [label = "b"]
  10 -> 11 [label = "C"]
  11 -> F11
}
'''),  ABCab)

# Of course we'll have 5 letters.
CHECK_EQ('''ABCab
ABaCb
ABabC
AaBCb
AaBbC
AabBC
aABCb
aABbC
aAbBC
abABC''', ABCab.enumerate(10).format('list'))


## --------------------- ##
## Heterogeneous input.  ##
## --------------------- ##

# RatE and B, in both directions.
a1 = vcsn.context('lal_char(ab)_ratexpset<lal_char(uv)_b>') \
         .ratexp('(<u>a+<v>b)*').standard()
a2 = vcsn.context('lal_char(ab)_b').ratexp('a').standard()
CHECK_EQ('a + <u+u>aa + <v>ab + <v>ba',
         a1.shuffle(a2).shortest(4).format('text'))
CHECK_EQ('a + <u+u>aa + <v>ab + <v>ba',
         a2.shuffle(a1).shortest(4).format('text'))


## ----------------- ##
## Non-commutative.  ##
## ----------------- ##

a1 = vcsn.context('lal_char(ab)_ratexpset<lal_char(uv)_b>') \
         .ratexp('<u>a<v>b').standard()
a2 = vcsn.context('lal_char(ab)_ratexpset<lal_char(xy)_b>') \
         .ratexp('<x>a<y>b').standard()
CHECK_EQ('''<uxvy+uxyv+xuvy+xuyv>aabb
<uvxy+xyuv>abab''', a1.shuffle(a2).enumerate(4).format('list'))


## ------- ##
## n-ary.  ##
## ------- ##

ctx = vcsn.context('lal_char(x)_ratexpset<lal_char(abcd)_b>')
a = dict()
for l in ['a', 'b', 'c', 'd']:
    a[l] = ctx.ratexp("<{}>x".format(l)).standard()
CHECK_EQ('<abcd+abdc+acbd+acdb+adbc+adcb+bacd+badc+bcad+bcda+bdac+bdca+cabd+cadb+cbad+cbda+cdab+cdba+dabc+dacb+dbac+dbca+dcab+dcba>xxxx',
    a['a']
    .shuffle(a['b'])
    .shuffle(a['c'])
    .shuffle(a['d'])
    .enumerate(10)
    .format('list'))
