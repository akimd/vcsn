#! /usr/bin/env python

import re
import vcsn
from test import *

def check(a1, a2, exp):
    res = a1.compose(a2)
    CHECK_EQ(exp, res)

#################
## Composition ##
#################

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

a = vcsn.automaton("""digraph
{
  vcsn_context = "lat<lan<lal_char(abc)>,lan<lal_char(def)>>_b"
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
  0 -> 1 [label = "(a, d)"]
  1 -> F1
  1 -> 1 [label = "(a, d)"]
}
""")
check(c1.ratexp("'(a, x)'*").standard(), c2.ratexp("'(x, d)'*").standard(), a)

#########################
## Epsilon-transitions ##
#########################

CHECK_EQ(str(c1.ratexp("'(a, x)'*").thompson().compose(
             c2.ratexp("'(x, d)'*").thompson()).trim().proper()),
             re.compile("lan<(lal_char\(.*?\))>").sub("\\1", str(a)))

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

############################
## Heterogeneous contexts ##
############################

c_ratb = vcsn.context("lat<lan_char(abc),lan_char(xyz)>_ratexpset<lal_char(mno)_b>")
c_q = vcsn.context("lat<lan_char(xyz),lan_char(def)>_q")
check(c_ratb.ratexp("<o>'(a, x)'").standard(),
      c_q.ratexp("<3/2>'(x, d)'").standard(),
      vcsn.automaton("""digraph
{
  vcsn_context = "lat<lan<lal_char(abc)>,lan<lal_char(def)>>_ratexpset<lal_char(mno)_q>"
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
  0 -> 1 [label = "<<3/2>o>(a, d)"]
  1 -> F1
}
"""))

##########################
## Fibonacci normalizer ##
##########################

check(vcsn.automaton.load(medir + "/left.gv"),
      vcsn.automaton.load(medir + "/right.gv"),
      vcsn.automaton.load(medir + "/result.gv"))

c_r = vcsn.context("lat<lan_char(abc),lan_char(xyz)>_r")
check(c_r.ratexp("<3.1>'(a, x)'").standard(), c2.ratexp("'(x, d)'").standard(),
        vcsn.automaton("""digraph
{
  vcsn_context = "lat<lan<lal_char(abc)>,lan<lal_char(def)>>_r"
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
  0 -> 1 [label = "<3.1>(a, d)"]
  1 -> F1
}
"""))
