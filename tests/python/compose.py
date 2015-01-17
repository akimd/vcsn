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

c1 = vcsn.context("lat<lan<lal_char(abc)>,lan<lal_char(xyz)>>, b")
c2 = vcsn.context("lat<lan<lal_char(xyz)>,lan<lal_char(def)>>, b")

check(c1.expression("'(a, x)'").standard(), c2.expression("'(x, d)'").standard(),
      '''digraph
{
  vcsn_context = "lat<nullableset<letterset<char_letters(abc)>>, nullableset<letterset<char_letters(def)>>>, b"
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
  }
  I0 -> 0
  0 -> 1 [label = "(a,d)"]
  1 -> F1
}''')

a = '''digraph
{
  vcsn_context = "lat<nullableset<letterset<char_letters(abc)>>, nullableset<letterset<char_letters(def)>>>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F1
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "0, 0", shape = box]
    1 [label = "1, 1", shape = box]
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "(a,d)"]
  1 -> F1
  1 -> 1 [label = "(a,d)"]
}'''
check(c1.expression("'(a, x)'*").standard(), c2.expression("'(x, d)'*").standard(), a)

#########################
## Epsilon-transitions ##
#########################

t1 = c1.expression("'(a, x)'*").thompson()
t2 = c2.expression("'(x, d)'*").thompson()
CHECK_EQ(vcsn.automaton(a),
         t1.compose(t2).trim().proper())

check(c1.expression("'(a, x)'*").standard(), c2.expression("'(y, d)'*").standard(),
      '''digraph
{
  vcsn_context = "lat<nullableset<letterset<char_letters(abc)>>, nullableset<letterset<char_letters(def)>>>, b"
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
  }
  I0 -> 0
  0 -> F0
}''')

############################
## Heterogeneous contexts ##
############################

c_ratb = vcsn.context("lat<lan_char(abc),lan_char(xyz)>, expressionset<lal_char(mno), b>")
c_q = vcsn.context("lat<lan_char(xyz),lan_char(def)>, q")
check(c_ratb.expression("<o>'(a, x)'").standard(),
      c_q.expression("<3/2>'(x, d)'").standard(),
      '''digraph
{
  vcsn_context = "lat<nullableset<letterset<char_letters(abc)>>, nullableset<letterset<char_letters(def)>>>, expressionset<letterset<char_letters(mno)>, q>"
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
  }
  I0 -> 0
  0 -> 1 [label = "<<3/2>o>(a,d)"]
  1 -> F1
}''')

###############################################
## Check mixed epsilon and letters going out ##
###############################################


a1 = vcsn.automaton(r'''digraph
{
  vcsn_context = "lat<lan<lal_char(xyz)>, lan<lal_char(abc)>>, b"
  I0 -> 0
  0 -> 1 [label = "(x, a)"]
  1 -> F1
  0 -> 2 [label = "(y, \\e)"]
  2 -> F2
}''')


a2 = vcsn.automaton(r'''digraph
{
  vcsn_context = "lat<lan<lal_char(abc)>, lan<lal_char(def)>>, b"
  I0 -> 0
  0 -> 1 [label = "(\\e, d)"]
  1 -> 2 [label = "(a, e)"]
  2 -> F2
}''')

res = r'''digraph
{
  vcsn_context = "lat<nullableset<letterset<char_letters(xyz)>>, nullableset<letterset<char_letters(def)>>>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F4
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "0, 0", shape = box]
    1 [label = "2, 0", shape = box, color = DimGray]
    2 [label = "0, 1", shape = box]
    3 [label = "2, 1", shape = box, color = DimGray]
    4 [label = "1, 2", shape = box]
  }
  I0 -> 0
  0 -> 1 [label = "(y,\\e)", color = DimGray]
  0 -> 2 [label = "(\\e,d)"]
  1 -> 3 [label = "(\\e,d)", color = DimGray]
  2 -> 4 [label = "(x,e)"]
  4 -> F4
}'''

check(a1, a2, res)


c_r = vcsn.context("lat<lan_char(abc),lan_char(xyz)>, r")
check(c_r.expression("<3.1>'(a, x)'").standard(), c2.expression("'(x, d)'").standard(),
      '''digraph
{
  vcsn_context = "lat<nullableset<letterset<char_letters(abc)>>, nullableset<letterset<char_letters(def)>>>, r"
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
  }
  I0 -> 0
  0 -> 1 [label = "<3.1>(a,d)"]
  1 -> F1
}''')

##########################
## Fibonacci normalizer ##
##########################

check(vcsn.automaton(filename = medir + "/left.gv"),
      vcsn.automaton(filename = medir + "/right.gv"),
      open(medir + "/result.gv").read().strip())
