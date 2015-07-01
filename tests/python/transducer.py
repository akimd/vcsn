#! /usr/bin/env python

import vcsn
from test import *

c = vcsn.context("lat<lan_char(abc), lan_char(xyz)>, z")

e = c.expression('([ab]|x)')
CHECK_EQ('a+b | x', e)
CHECK_EQ('''(a,x).[\e] + (b,x).[\e]''', e.expansion())
CHECK_EQ(r'''digraph
{
  vcsn_context = "lat<nullableset<letterset<char_letters(abc)>>, nullableset<letterset<char_letters(xyz)>>>, z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F1
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "a+b | x", shape = box]
    1 [label = "\\e", shape = box]
  }
  I0 -> 0
  0 -> 1 [label = "(a,x), (b,x)"]
  1 -> F1
}''',
         e.derived_term())
