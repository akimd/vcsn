#! /usr/bin/env python

import vcsn
from test import *

## ---------- ##
## Automata.  ##
## ---------- ##

l4 = vcsn.context('lal_char(abc), b').ladybird(4)
CHECK_EQ('''digraph
{
  vcsn_context = "lao, expressionset<letterset<char_letters(abc)>, b>"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
    3
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "<a>"]
  1 -> 0 [label = "<c>"]
  1 -> 1 [label = "<b+c>"]
  1 -> 2 [label = "<a>"]
  2 -> 0 [label = "<c>"]
  2 -> 2 [label = "<b+c>"]
  2 -> 3 [label = "<a>"]
  3 -> 0 [label = "<a+c>"]
  3 -> 3 [label = "<b+c>"]
}''',
         l4.lift())


## ------------ ##
## Lift(tape).  ##
## ------------ ##

c = vcsn.context('lat<lal_char(abc), lan_char(def), law_char(ghi)>, q')
a = c.expression("'a,d,gh'<2>").standard()
aref = '''digraph
{
  vcsn_context = "lat<letterset<char_letters(abc)>>, expressionset<lat<nullableset<letterset<char_letters(def)>>, wordset<char_letters(ghi)>>, q>"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F1
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
  }
  I0 -> 0
  0 -> 1 [label = "<<2>(d,gh)>(a)"]
  1 -> F1
}'''
CHECK_EQ(aref, a.lift(1, 2))
CHECK_EQ(aref, a.lift([1, 2]))

## ------------------ ##
## lift(expression).  ##
## ------------------ ##

CHECK_EQ('''<abc>\e''',
         vcsn.context('lal_char(abc), b').expression('abc').lift())
CHECK_EQ('''<<2>(abc)>\e''',
         vcsn.context('lal_char(abc), z').expression('<2>abc').lift())
