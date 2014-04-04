#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context('lal_char(ab)_ratexpset<lal_char(xyz)_b>')

## ---------- ##
## automata.  ##
## ---------- ##

q = vcsn.context('lal_char(ab)_q')
a = vcsn.automaton(r'''
digraph
{
  vcsn_context = "lal_char(ab)_q"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I
    F0
    F1
  }
  {
    node [shape = circle]
    0
    1
    2
  }
  I -> 0
  0 -> F0
  0 -> 1 [label = "a, b"]
  1 -> 2 [label = "b"]
  2 -> 2 [label = "a, b"]
  2 -> F1
}
''')

exp = vcsn.automaton(r'''
digraph
{
  vcsn_context = "lal_char(ab)_q"
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
    2
  }
  I0 -> 0
  0 -> F0 [label = "<3/4>"]
  0 -> 1 [label = "<3/4>a, <3/4>b"]
  1 -> 2 [label = "b"]
  2 -> F2
  2 -> 2 [label = "a, b"]
}
''')
CHECK_EQ(exp, q.weight('3/4') * a)

exp = vcsn.automaton(r'''
digraph
{
  vcsn_context = "lal_char(ab)_q"
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
    2
  }
  I0 -> 0
  0 -> F0 [label = "<3/4>"]
  0 -> 1 [label = "a, b"]
  1 -> 2 [label = "b"]
  2 -> F2 [label = "<3/4>"]
  2 -> 2 [label = "a, b"]
}
''')
CHECK_EQ(exp, a * q.weight('3/4'))


a = vcsn.automaton(r'''
digraph
{
  vcsn_context = "lal_char(abc)_r"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I
    F0
    F1
  }
  {
    node [shape = circle]
    0
    1
    2
    3
  }
  I -> 0
  0 -> F0
  0 -> 1 [label = "a, b"]
  0 -> 3 [label = "c"]
  3 -> 2 [label = "c"]
  1 -> 2 [label = "b"]
  2 -> F1
}
''')


r = vcsn.context('lal_char(ab)_r')
exp = vcsn.automaton(r'''
digraph
{
  vcsn_context = "lal_char(abc)_r"
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
    2
    3
  }
  I0 -> 0
  0 -> F0 [label = "<3.4>"]
  0 -> 1 [label = "<3.4>a, <3.4>b"]
  0 -> 2 [label = "<3.4>c"]
  1 -> 3 [label = "b"]
  2 -> 3 [label = "c"]
  3 -> F3
}
''')
CHECK_EQ(exp, (r.weight('3.4') * a).sort())

exp = vcsn.automaton(r'''
digraph
{
  vcsn_context = "lal_char(abc)_r"
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
    2
    3
  }
  I0 -> 0
  0 -> F0 [label = "<3.4>"]
  0 -> 1 [label = "a, b"]
  0 -> 2 [label = "c"]
  1 -> 3 [label = "b"]
  2 -> 3 [label = "c"]
  3 -> F3 [label = "<3.4>"]
}
''')
CHECK_EQ(exp, (a * r.weight('3.4')).sort())

## -------- ##
## ratexp.  ##
## -------- ##

r = ctx.ratexp('<x>(<y>a)*<z>')
CHECK_EQ(ctx.ratexp('<xx>(<y>a)*<z>'), ctx.weight('x') * r)
CHECK_EQ(ctx.ratexp('<x>(<y>a)*<zz>'), r * ctx.weight('z'))
