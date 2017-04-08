#! /usr/bin/env python

import re
import sys
import vcsn

from test import *

def check(a1, a2, exp):
    # @ is only supported since Python 3.5.
    CHECK_EQ(exp, a1.compose(a2))

## ---------- ##
## Contexts.  ##
## ---------- ##

make = vcsn.context
c1 = make('lat<lan(abc), lan(efg), lan(xyz)>, b')
c2 = make('lat<lan(xyz), lan(EFG), lal(ABC)>, q')
c3 = make('lat<lan(abc), lan(efg), lan(EFG), lal(ABC)>, q')
check(c1, c2, c3)

## -------- ##
## Labels.  ##
## -------- ##

check(c1.label('a|e|x'),
      c2.label('x|E|A'),
      c3.label('a|e|E|A'))


## ---------- ##
## Automata.  ##
## ---------- ##

c1 = vcsn.context("lat<lan_char(abc),lan_char(xyz)>, b")
c2 = vcsn.context("lat<lan_char(xyz),lan_char(def)>, b")

check(c1.expression("a|x").standard(),
      c2.expression("x|d").standard(),
      r'''digraph
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
    0 [label = "0, (0, !\\e)", shape = box]
    1 [label = "1, (1, !\\e)", shape = box]
  }
  I0 -> 0
  0 -> 1 [label = "a|d"]
  1 -> F1
}''')

a = r'''digraph
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
    0 [label = "0, (0, !\\e)", shape = box]
    1 [label = "1, (1, !\\e)", shape = box]
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "a|d"]
  1 -> F1
  1 -> 1 [label = "a|d"]
}'''
check(c1.expression("(a|x)*").standard(),
      c2.expression("(x|d)*").standard(),
      a)

# Check that weights are added, not replaced

cz = vcsn.context('lat<lal_char, lal_char>, z')
aut = cz.expression('[ab]|[ab]').automaton()
b = r'''digraph
{
  vcsn_context = "lat<letterset<char_letters(ab)>, letterset<char_letters(ab)>>, z"
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
  0 -> 1 [label = "<2>[^]"]
  1 -> F1
}'''
check(aut, aut, b)

## ------------------------- ##
## Spontaneous transitions.  ##
## ------------------------- ##

t1 = c1.expression("(a|x)*").thompson()
t2 = c2.expression("(x|d)*").thompson()
CHECK_EQ(vcsn.automaton(a),
         t1.compose(t2).trim().proper())

check(c1.expression("(a|x)*").standard(),
      c2.expression("(y|d)*").standard(),
      r'''digraph
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
    0 [label = "0, (0, !\\e)", shape = box]
  }
  I0 -> 0
  0 -> F0
}''')

## ------------------------ ##
## Heterogeneous contexts.  ##
## ------------------------ ##

c_ratb = vcsn.context("lat<lan_char(abc),lan_char(xyz)>, expressionset<lal_char(mno), b>")
c_q = vcsn.context("lat<lan_char(xyz),lan_char(def)>, q")
check(c_ratb.expression("<o>(a|x)").standard(),
      c_q.expression("<3/2>(x|d)").standard(),
      r'''digraph
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
    0 [label = "0, (0, !\\e)", shape = box]
    1 [label = "1, (1, !\\e)", shape = box]
  }
  I0 -> 0
  0 -> 1 [label = "<<3/2>o>a|d"]
  1 -> F1
}''')

## ------------------------------------------- ##
## Check mixed epsilon and letters going out.  ##
## ------------------------------------------- ##


a1 = vcsn.automaton(r'''digraph
{
  vcsn_context = "lat<lan_char(xyz), lan_char(abc)>, b"
  I0 -> 0
  0 -> 1 [label = "(x, a)"]
  1 -> F1
  0 -> 2 [label = "(y, \\e)"]
  2 -> F2
}''')


a2 = vcsn.automaton(r'''digraph
{
  vcsn_context = "lat<lan_char(abc), lan_char(def)>, b"
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
    0 [label = "0, (0, !\\e)", shape = box]
    1 [label = "2, (0, !\\e)", shape = box, color = DimGray]
    2 [label = "0, (1, \\e)", shape = box]
    3 [label = "2, (1, \\e)", shape = box, color = DimGray]
    4 [label = "1, (2, !\\e)", shape = box]
  }
  I0 -> 0
  0 -> 1 [label = "y|\\e", color = DimGray]
  0 -> 2 [label = "\\e|d"]
  1 -> 3 [label = "\\e|d", color = DimGray]
  2 -> 4 [label = "x|e"]
  4 -> F4
}'''

check(a1, a2, res)


c_r = vcsn.context("lat<lan_char(abc),lan_char(xyz)>, r")
check(c_r.expression("<3.1>(a|x)").standard(),
      c2.expression("x|d").standard(),
      r'''digraph
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
    0 [label = "0, (0, !\\e)", shape = box]
    1 [label = "1, (1, !\\e)", shape = box]
  }
  I0 -> 0
  0 -> 1 [label = "<3.1>a|d"]
  1 -> F1
}''')

## ---------------------- ##
## Fibonacci normalizer.  ##
## ---------------------- ##

check(meaut('left.gv'),
      meaut('right.gv'),
      metext('result.gv'))

## ------------------ ##
## Lazy composition.  ##
## ------------------ ##

CHECK_EQ(metext('result.gv'),
         meaut('left.gv').compose(meaut('right.gv'), lazy=True).accessible())


# Test laziness on strict composition
ctx = vcsn.context("lat<lan<char>, lan<char>>, b")
fr_to_en = ctx.expression("chien|dog + chat|cat").automaton()
en_to_es = ctx.expression("dog|perro + cat|gato").automaton()
fr_to_es_lazy = fr_to_en.compose(en_to_es, lazy=True)
chien = ctx.expression("chien|chien").automaton()
c2 = chien.compose(fr_to_es_lazy)

# FIXME: info resolves part of the laziness (because it calls
# number_of_accessible_states etc. which, stupidly, do not respect
# laziness).  This is to be fixed at some point, but in the meanwhile,
# just be cautious about calling info.
CHECK_EQ(fr_to_es_lazy.info('number of lazy states'), 1)
