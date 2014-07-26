#! /usr/bin/env python

import vcsn
from test import *

## ---------- ##
## Automata.  ##
## ---------- ##

l4 = vcsn.context('lal_char(abc)_b').ladybird(4)
CHECK_EQ('''digraph
{
  vcsn_context = "lao_ratexpset<lal_char(abc)_b>"
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
  3 -> 0 [label = "<c+a>"]
  3 -> 3 [label = "<b+c>"]
}''',
         l4.lift())

## ------------- ##
## Expressions.  ##
## ------------- ##

CHECK_EQ('''<abc>\e''',
         vcsn.context('lal_char(abc)_b').ratexp('abc').lift())
CHECK_EQ('''<<2>abc>\e''',
         vcsn.context('lal_char(abc)_z').ratexp('<2>abc').lift())
