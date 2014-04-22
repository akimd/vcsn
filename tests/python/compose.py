#! /usr/bin/env python

import vcsn
from test import *

def check(a1, a2, exp):
    res = a1.compose(a2)
    CHECK_EQ(res, exp)


c1 = vcsn.context("lat<lan<lal_char(abc)>,lan<lal_char(xyz)>>_b")
c2 = vcsn.context("lat<lan<lal_char(xyz)>,lan<lal_char(def)>>_b")

check(c1.ratexp("'(a, x)'").standard(), c2.ratexp("'(x, d)'").standard(),
        vcsn.automaton("""digraph
{
  vcsn_context = "lat<lan<lal_char(abc)>,lan<lal_char(def)>>_b"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F1
  }
  {
    node [shape = circle]
    0
    1
  }
  I0 -> 0
  0 -> 1 [label = "(a, d)"]
  1 -> F1
}
"""))

check(c1.ratexp("'(a, x)'*").standard(), c2.ratexp("'(x, d)'*").standard(),
        vcsn.automaton("""digraph
{
  vcsn_context = "lat<lan<lal_char(abc)>,lan<lal_char(def)>>_b"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F1
  }
  {
    node [shape = circle]
    0
    1
  }
  I0 -> 0
  0 -> 1 [label = "(a, d)"]
  1 -> F1
  1 -> 1 [label = "(a, d)"]
}
"""))

check(c1.ratexp("'(a, x)'*").standard(), c2.ratexp("'(y, d)'*").standard(),
        vcsn.automaton("""digraph
{
  vcsn_context = "lat<lan<lal_char(abc)>,lan<lal_char(def)>>_b"
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
}"""))
