#! /usr/bin/env python

import vcsn
from test import *

## ------------ ##
## lal_char_z.  ##
## ------------ ##

simple = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab)_z"
  rankdir = LR
  node [shape = circle]
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F1
  }
  0 -> 0 [label = "a, b"]
  0 -> 1 [label = "<2>a"]
  1 -> 1 [label = "a, b"]
  1 -> F1
  I0 -> 0
}
''')

CHECK_EQ(int(simple.eval("aabab").format("text")), 6)
CHECK_EQ(int(simple.eval("abab").format("text")), 4)
CHECK_EQ(int(simple.eval("aaaa").format("text")), 8)
CHECK_EQ(int(simple.eval("b").format("text")), 0)
CHECK_EQ(int(simple.eval("a").format("text")), 2)

initial_weight = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab)_z"
  rankdir = LR
  node [shape = circle]
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F1
  }
  0 -> 0 [label = "a, b"]
  0 -> 1 [label = "<2>a"]
  1 -> 1 [label = "a, b"]
  1 -> F1
  I0 -> 0 [label = "<2>"]
}
''')

CHECK_EQ(int(initial_weight.eval("a").format("text")), 4)
CHECK_EQ(int(initial_weight.eval("abab").format("text")), 8)
CHECK_EQ(int(initial_weight.eval("aabab").format("text")), 12)

more_letters = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abc)_z"
  rankdir = LR
  node [shape = circle]
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F1
  }
  0 -> 0 [label = "a, b"]
  0 -> 1 [label = "<2>a"]
  0 -> 2 [label = "c"]
  1 -> 1 [label = "a, b"]
  1 -> F1
  2 -> 0 [label = "a"]
  2 -> 1 [label = "a"]
  I0 -> 0 [label = "<2>"]
}
''')

CHECK_EQ(int(more_letters.eval("caa").format("text")), 6)

prod = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abc)_z"
  rankdir = LR
  node [shape = circle]
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F1
  }
  0 -> 0 [label = "a, b"]
  0 -> 1 [label = "<2>a"]
  0 -> 2 [label = "c"]
  1 -> 1 [label = "a, b"]
  1 -> F1
  2 -> 0 [label = "a"]
  2 -> 1 [label = "a"]
  I0 -> 0 [label = "<2>"]
}
''')

CHECK_EQ(int(prod.eval("aabab").format("text")), 12)

cmplex = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abc)_z"
  rankdir = LR
  node [shape = circle]
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F1
  }
  0 -> 0 [label = "a, b"]
  0 -> 1 [label = "<2>a"]
  0 -> 2 [label = "c"]
  1 -> 1 [label = "a, b"]
  1 -> F1 [label = "<3>"]
  2 -> 0 [label = "a"]
  2 -> 1 [label = "a"]
  I0 -> 0 [label = "<2>"]
}
''')

CHECK_EQ(int(cmplex.eval("a").format("text")), 12)
CHECK_EQ(int(cmplex.eval("abab").format("text")), 24)
CHECK_EQ(int(cmplex.eval("aabab").format("text")), 36)


## --------------- ##
## lal_char_zmin.  ##
## --------------- ##

ctx = vcsn.context('lal_char(abcd)_b')
a = ctx.ratexp('(?@lal_char(abc)_zmin)a').standard()

CHECK_EQ(a.eval('').format("text"), 'oo')
CHECK_EQ(int(a.eval('a').format("text")), 0)
CHECK_EQ(a.eval('aa').format("text"), 'oo')
CHECK_EQ(a.eval('b').format("text"), 'oo')

CHECK_EQ(int(load('lal_char_zmin/minab.gv').eval('').format("text")), 0)
CHECK_EQ(int(load('lal_char_zmin/minab.gv').eval('b').format("text")), 0)
CHECK_EQ(int(load('lal_char_zmin/minab.gv').eval('a').format("text")), 0)
CHECK_EQ(int(load('lal_char_zmin/minab.gv').eval('ab').format("text")), 1)
CHECK_EQ(int(load('lal_char_zmin/minab.gv').eval('abababbbb').format("text")), 3)

CHECK_EQ(load('lal_char_zmin/minblocka.gv').eval('').format("text"), 'oo')
CHECK_EQ(load('lal_char_zmin/minblocka.gv').eval('b').format("text"), 'oo')
CHECK_EQ(load('lal_char_zmin/minblocka.gv').eval('a').format("text"), 'oo')
CHECK_EQ(load('lal_char_zmin/minblocka.gv').eval('ab').format("text"), 'oo')
CHECK_EQ(int(load('lal_char_zmin/minblocka.gv').eval('abababbbb').format("text")), 0)
CHECK_EQ(int(load('lal_char_zmin/minblocka.gv').eval('aabaaba').format("text")), 2)

CHECK_EQ(load('lal_char_zmin/slowgrow.gv').eval('').format("text"), 'oo')
CHECK_EQ(load('lal_char_zmin/slowgrow.gv').eval('b').format("text"), 'oo')
CHECK_EQ(load('lal_char_zmin/slowgrow.gv').eval('a').format("text"), 'oo')
CHECK_EQ(load('lal_char_zmin/slowgrow.gv').eval('ab').format("text"), 'oo')
CHECK_EQ(int(load('lal_char_zmin/slowgrow.gv').eval('abababb').format("text")), 0)
CHECK_EQ(int(load('lal_char_zmin/slowgrow.gv').eval('abbaaa').format("text")), 0)
CHECK_EQ(int(load('lal_char_zmin/slowgrow.gv').eval('abbababa').format("text")), 1)
CHECK_EQ(int(load('lal_char_zmin/slowgrow.gv').eval('baaaab').format("text")), 4)

