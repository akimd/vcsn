#! /usr/bin/env python

import vcsn
from test import *

c = vcsn.context('law_char, z')
p = c.polynomial('<2>\e+<3>a+<4>b+<5>abc+<6>abcd+<7>abdc')
a = p.trie()
CHECK_EQ(r'''digraph
{
  vcsn_context = "letterset<char_letters(abcd)>, z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F1
    F2
    F4
    F5
    F7
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
    3
    4
    5
    6
    7
  }
  I0 -> 0
  0 -> F0 [label = "<2>"]
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> F1 [label = "<3>"]
  1 -> 3 [label = "b"]
  2 -> F2 [label = "<4>"]
  3 -> 4 [label = "c"]
  3 -> 6 [label = "d"]
  4 -> F4 [label = "<5>"]
  4 -> 5 [label = "d"]
  5 -> F5 [label = "<6>"]
  6 -> 7 [label = "c"]
  7 -> F7 [label = "<7>"]
}''', a)
