#! /usr/bin/env python

import vcsn
from test import *

b = vcsn.context('[abcd] -> b')

## ---------------------- ##
## Existing transitions.  ##
## ---------------------- ##

# See the actual code of product to understand the point of this test
# (which is new_transition vs. add_transition).
a1 = vcsn.Z.expression('a*b').derived_term()
CHECK_EQ('⟨2⟩((⟨2⟩a)*ba*b)', str(a1.shuffle(a1).expression()))


## ------------------------ ##
## (ab)* )( (<-1>ab)* = \e. ##
## ------------------------ ##
# TAFKIT manual, Figure 3.13, left [as of 2013-10-10]

abs = vcsn.automaton('''
digraph
{
  vcsn_context = "[ab]? → ℤ"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F0
    F2
  }
  {
    node [shape = circle]
    0 [label = "0, 0", shape = box, style = rounded]
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
  vcsn_context = "[ab]? → ℤ"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F0
    F2
  }
  {
    node [shape = circle]
    0 [label = "0, 0", shape = box, style = rounded]
    1
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "<-1>a"]
  1 -> 0 [label = "<1>b"]
}
''')

CHECK_EQ('''digraph
{
  vcsn_context = "[ab]? → ℤ"
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
    1 [label = "1, 0", shape = box]
    2 [label = "0, 1", shape = box]
    3 [label = "1, 1", shape = box]
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
}''', str(abs.shuffle(mabs)))


## ---------------------- ##
## (a)* )( (<-1>a)* = \e. ##
## ---------------------- ##
# TAFKIT manual, Figure 3.13, right [as of 2013-10-10]

pas = vcsn.context("[a] -> z").expression('a*').derived_term()
mas = vcsn.context("[a] -> z").expression('(<-1>a)*').derived_term()
CHECK_EQ('''digraph
{
  vcsn_context = "[a]? → ℤ"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "a*, (<-1>a)*", shape = box]
  }
  I0 -> 0
  0 -> F0
}''', str(pas.shuffle(mas)))


## ---------- ##
## ABC )( ab. ##
## ---------- ##

ab = vcsn.context('[ab] -> z').expression('ab').standard()
ABC = vcsn.context('[ABC] -> z').expression('ABC').standard()
ABCab = ABC.shuffle(ab)
CHECK_EQ('''digraph
{
  vcsn_context = "[ABCab]? → ℤ"
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
    3 [label = "3, 0", shape = box]
    4 [label = "1, 1", shape = box]
    5 [label = "0, 3", shape = box]
    6 [label = "4, 0", shape = box]
    7 [label = "3, 1", shape = box]
    8 [label = "1, 3", shape = box]
    9 [label = "4, 1", shape = box]
    10 [label = "3, 3", shape = box]
    11 [label = "4, 3", shape = box]
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
}''',  str(ABCab))

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
abABC''', ABCab.shortest(len = 10).format('list'))


## --------------------- ##
## Heterogeneous input.  ##
## --------------------- ##

# RatE and B, in both directions.
a1 = vcsn.context('[ab] -> seriesset<[uv] -> q>') \
         .expression('(<u>a+<v>b)*').standard()
a2 = vcsn.context('[ab] -> b').expression('a').standard()
CHECK_EQ('a + <<2>u>aa + <v>ab + <v>ba',
         a1.shuffle(a2).shortest(4).format('text'))
CHECK_EQ('a + <<2>u>aa + <v>ab + <v>ba',
         a2.shuffle(a1).shortest(4).format('text'))


## ----------------- ##
## Non-commutative.  ##
## ----------------- ##

a1 = vcsn.context('[ab] -> seriesset<[uv] -> q>') \
         .expression('<u>a<v>b').standard()
a2 = vcsn.context('[ab] -> seriesset<[xy] -> q>') \
         .expression('<x>a<y>b').standard()
CHECK_EQ('''<uxvy+uxyv+xuvy+xuyv>aabb
<uvxy+xyuv>abab''', a1.shuffle(a2).shortest(len = 4).format('list'))


## ---------- ##
## Variadic.  ##
## ---------- ##

ctx = vcsn.context('[x] -> seriesset<[abcd] -> q>')
a = dict()
for l in ['a', 'b', 'c', 'd']:
    a[l] = ctx.expression("<{}>x".format(l)).standard()

CHECK_EQ('<abcd+abdc+acbd+acdb+adbc+adcb+bacd+badc+bcad+bcda+bdac+bdca+cabd+cadb+cbad+cbda+cdab+cdba+dabc+dacb+dbac+dbca+dcab+dcba>xxxx',
         a['a'].shuffle(a['b'], a['c'], a['d'])
         .strip()
         .shortest(len = 10)
         .format('list'))
