#! /usr/bin/env python

import vcsn
from test import *

ab = vcsn.context('lal_char(ab)_b').ratexp('(a+b)*').standard()
bc = vcsn.context('lal_char(bc)_b').ratexp('(b+c)*').standard()
CHECK_EQ(vcsn.automaton.load(medir + '/abc.gv'), ab | bc)

## ------------ ##
## lal_char_z.  ##
## ------------ ##

# <2>(a*b*a*)
a = vcsn.automaton('''
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
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> F1
  1 -> 1 [label = "a"]
  2 -> F2
  2 -> 1 [label = "a"]
  2 -> 2 [label = "b"]
}
''')

# (<3>(ab))*
b = vcsn.automaton('''
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
    I3
    F0
    F1
    F2
    F3
  }
  {
    node [shape = circle]
    0
    1
    2
    3
    4
  }
  I0 -> 0 [label = "<2>"]
  I3 -> 3 [label = "<3>"]
  0 -> F0
  0 -> 0 [label = "a"]
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> F1
  1 -> 1 [label = "a"]
  2 -> F2
  2 -> 1 [label = "a"]
  2 -> 2 [label = "b"]
  3 -> F3
  3 -> 4 [label = "a"]
  4 -> 3 [label = "<3>b"]
}
''')
CHECK_EQ(exp, a | b)

# Check union of contexts.
a1 = vcsn.context('lal_char(a)_ratexpset<lal_char(x)_b>').ratexp('<x>a*').standard()
a2 = vcsn.context('lal_char(b)_ratexpset<lal_char(y)_b>').ratexp('<y>b*').standard()
exp = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab)_ratexpset<lal_char(xy)_b>"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    I2
    F0
    F1
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
  I0 -> 0
  I2 -> 2
  0 -> F0 [label = "<x>"]
  0 -> 1 [label = "<x>a"]
  1 -> F1
  1 -> 1 [label = "a"]
  2 -> F2 [label = "<y>"]
  2 -> 3 [label = "<y>b"]
  3 -> F3
  3 -> 3 [label = "b"]
}
''')

CHECK_EQ(exp, a1 | a2)

## --------------------- ##
## Heterogeneous input.  ##
## --------------------- ##

# check RES AUT
# -------------
def check(exp, eff):
    CHECK_EQ(exp, str(eff.ratexp()))

# RatE and B, in both directions.
a1 = vcsn.context('lal_char(a)_ratexpset<lal_char(uv)_b>') \
         .ratexp('<u>a').derived_term()
a2 = vcsn.context('lal_char(b)_b').ratexp('b*').derived_term()
check('<u>a+b*', a1|a2)
check('<u>a+b*', a2|a1)

# Z, Q, R.
z = vcsn.context('lal_char(a)_z').ratexp('<2>a')  .derived_term()
q = vcsn.context('lal_char(b)_q').ratexp('<1/3>b').derived_term()
r = vcsn.context('lal_char(c)_r').ratexp('<.4>c') .derived_term()

check('<2>a+<1/3>b', z|q)
check('<1/3>b+<2>a', q|z)
check('<2>a+<1/3>b+<2>a', z|q|z)
check('<2>a+<1/3>b+<1/3>b', z|q|q)

check('<2>a+<0.4>c', z|r)
check('<0.4>c+<2>a', r|z)

check('<0.333333>b+<0.4>c', q|r)
check('<0.4>c+<0.333333>b', r|q)

PLAN()
