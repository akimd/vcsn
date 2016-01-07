#! /usr/bin/env python

import vcsn
from test import *

#
# Simple test, one spontaneous transition
#      a    \e   a
# -> 0 -> 1 -> 2 -> 3 ->
#

aut = vcsn.automaton(r'''
digraph
{
  vcsn_context = "nullableset<letterset<char_letters(a)>>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F3
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
    3
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  1 -> 2 [label = "\\e"]
  2 -> 3 [label = "a"]
  3 -> F3
}''')

lazy = aut.proper(algo='lazy')

CHECK_EQ('''digraph
{
  vcsn_context = "letterset<char_letters(a)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F3
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [style = dashed, color = DimGray]
  }
  I0 -> 0 [color = DimGray]
}''', lazy.format('dot'))

CHECK_EQ('0', str(lazy.eval(r'\e')))
CHECK_EQ('0', str(lazy.eval('a')))
CHECK_EQ('1', str(lazy.eval('aa')))

CHECK_EQ('''digraph
{
  vcsn_context = "letterset<char_letters(a)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F3
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    2
    3
  }
  I0 -> 0
  0 -> 2 [label = "a"]
  2 -> 3 [label = "a"]
  3 -> F3
}''', lazy.format('dot'))

#
# More complex test, generated by thompson
# Expression:  a*(a+b)
#

aut = vcsn.b.expression('a*(a+b)').thompson()
lazy = aut.proper(algo='lazy')

CHECK_EQ('''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    I6
    I8
    F5
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [style = dashed, color = DimGray]
    6 [style = dashed, color = DimGray]
    8 [style = dashed, color = DimGray]
  }
  I0 -> 0 [color = DimGray]
  I6 -> 6 [color = DimGray]
  I8 -> 8 [color = DimGray]
}''', lazy.format('dot'))

CHECK_EQ('1', str(lazy.eval('b')))
CHECK_EQ('1', str(lazy.eval('a')))
CHECK_EQ('0', str(lazy.eval('ba')))
CHECK_EQ('0', str(lazy.eval('aba')))
CHECK_EQ('1', str(lazy.eval('aaaaab')))

CHECK_EQ('''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    I6
    I8
    F5
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    5
    6
    8
  }
  I0 -> 0
  I6 -> 6
  I8 -> 8
  0 -> 0 [label = "a"]
  0 -> 6 [label = "a"]
  0 -> 8 [label = "a"]
  5 -> F5
  6 -> 5 [label = "a"]
  8 -> 5 [label = "b"]
}''', lazy.format('dot'))