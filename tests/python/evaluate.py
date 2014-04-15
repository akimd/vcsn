#! /usr/bin/env python

import vcsn
from test import *

## check AUTOMATON WORD EXP
## ------------------------
def check(aut, word, exp):
    exp = ctx.weight(exp)
    CHECK_EQ(exp, aut.eval(word))

## ------------ ##
## lal_char_z.  ##
## ------------ ##
ctx = vcsn.context("lal_char(ab)_z")
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
check(simple, 'aabab', '6')
check(simple, 'aabab', '6')
check(simple, 'abab',  '4')
check(simple, 'aaaa',  '8')
check(simple, 'b',     '0')
check(simple, 'a',     '2')

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
check(initial_weight, 'a',      '4')
check(initial_weight, 'abab',   '8')
check(initial_weight, 'aabab', '12')

ctx = vcsn.context("lal_char(abc)_z")
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
check(more_letters, 'caa', '6')

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
check(prod, "aabab", '12')

ctx = vcsn.context("lal_char(abc)_z")
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
check(cmplex, 'a',     '12')
check(cmplex, 'abab',  '24')
check(cmplex, 'aabab', '36')


## --------------- ##
## lal_char_zmin.  ##
## --------------- ##

ctx = vcsn.context('lal_char(abc)_zmin')
a = ctx.ratexp('a').standard()
check(a, '',   'oo')
check(a, 'a',  '0')
check(a, 'aa', 'oo')
check(a, 'b',  'oo')

minab = load('lal_char_zmin/minab.gv')
check(minab, '',          '0')
check(minab, 'b',         '0')
check(minab, 'a',         '0')
check(minab, 'ab',        '1')
check(minab, 'abababbbb', '3')

minblocka = load('lal_char_zmin/minblocka.gv')
check(minblocka, '',          'oo')
check(minblocka, 'b',         'oo')
check(minblocka, 'a',         'oo')
check(minblocka, 'ab',        'oo')
check(minblocka, 'abababbbb', '0')
check(minblocka, 'aabaaba',   '2')

slowgrow = load('lal_char_zmin/slowgrow.gv')
check(slowgrow, '',         'oo')
check(slowgrow, 'b',        'oo')
check(slowgrow, 'a',        'oo')
check(slowgrow, 'ab',       'oo')
check(slowgrow, 'abababb',  '0')
check(slowgrow, 'abbaaa',   '0')
check(slowgrow, 'abbababa', '1')
check(slowgrow, 'baaaab',   '4')
