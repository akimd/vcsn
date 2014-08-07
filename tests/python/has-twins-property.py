#! /usr/bin/env python

import vcsn
from test import *

aut1 = vcsn.automaton(r'''
digraph
{
  vcsn_context = "lal_char(abcd)_zmin"

  I0 -> 0
  0 -> 1 [label = "<1>a"]
  0 -> 2 [label = "<2>a"]
  1 -> 1 [label = "<3>b"]
  1 -> 3 [label = "<5>c"]
  2 -> 2 [label = "<3>b"]
  2 -> 3 [label = "<6>d"]
  3 -> F3 [label = "<0>"]
}
''')

aut2 = vcsn.automaton(r'''
digraph
{
  vcsn_context = "lal_char(abcd)_zmin"

  I0 -> 0
  0 -> 1 [label = "<1>a"]
  0 -> 2 [label = "<2>a"]
  1 -> 1 [label = "<3>b"]
  1 -> 3 [label = "<5>c"]
  2 -> 2 [label = "<4>b"]
  2 -> 3 [label = "<6>d"]
  3 -> F3 [label = "<0>"]
}
''')


oaut1 = '''digraph
{
  vcsn_context = "lal_char(abcd)_zmin"
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
  I0 -> 0 [label = "<0>"]
  0 -> 1 [label = "<-1>a"]
  0 -> 2 [label = "<-2>a"]
  1 -> 1 [label = "<-3>b"]
  1 -> 3 [label = "<-5>c"]
  2 -> 2 [label = "<-3>b"]
  2 -> 3 [label = "<-6>d"]
  3 -> F3 [label = "<0>"]
}'''

aut3 = vcsn.automaton('''digraph
{
  vcsn_context = "lal_char(abcd)_zmin"
  I0 -> 0
  0 -> 1 [label = "<2>a"]
  0 -> 2 [label = "<3>a"]
  1 -> 3 [label = "<3>b"]
  1 -> 4 [label = "<2>c"]
  2 -> 4 [label = "<5>d"]
  2 -> 5 [label = "<3>b"]
  3 -> 6 [label = "<6>c"]
  4 -> F4
  5 -> 7 [label = "<6>c"]
  6 -> 1 [label = "<9>d"]
  7 -> 2 [label = "<9>d"]
}''')

aut4 = vcsn.automaton('''digraph
{
  vcsn_context = "lal_char(abcd)_zmin"
  I0 -> 0
  0 -> 1 [label = "<2>a"]
  0 -> 2 [label = "<3>a"]
  1 -> 3 [label = "<3>b"]
  1 -> 4 [label = "<2>c"]
  2 -> 4 [label = "<5>d"]
  2 -> 5 [label = "<3>b"]
  3 -> 6 [label = "<6>c"]
  4 -> F4
  5 -> 7 [label = "<6>c"]
  6 -> 1 [label = "<9>d"]
  7 -> 2 [label = "<13>d"]
}''')

aut5 = vcsn.automaton('''digraph
{
  vcsn_context = "lal_char(abcd)_q"
  I0 -> 0
  0 -> 1 [label = "<2/3>a"]
  0 -> 2 [label = "<3/5>a"]
  1 -> 3 [label = "<4>b"]
  1 -> 4 [label = "<2>c"]
  2 -> 4 [label = "<7>d"]
  2 -> 5 [label = "<2>b"]
  3 -> 1 [label = "<6>d"]
  4 -> F4
  5 -> 2 [label = "<12>d"]
}''')

aut6 = vcsn.automaton('''digraph
{
  vcsn_context = "lal_char(abcd)_q"
  I0 -> 0
  0 -> 1 [label = "<2/3>a"]
  0 -> 2 [label = "<3/5>a"]
  1 -> 3 [label = "<4>b"]
  1 -> 4 [label = "<2>c"]
  2 -> 4 [label = "<7>d"]
  2 -> 5 [label = "<2>b"]
  3 -> 1 [label = "<6>d"]
  4 -> F4
  5 -> 2 [label = "<17>d"]
}''')

def inverse_check(i, o):
  CHECK_EQ(o, i.inverse())

inverse_check(aut1, oaut1)

def has_twins_property_check():
  CHECK_EQ(True, aut1.has_twins_property())
  CHECK_EQ(False, aut2.has_twins_property())
  CHECK_EQ(True, aut3.has_twins_property())
  CHECK_EQ(False, aut4.has_twins_property())
  CHECK_EQ(True, aut5.has_twins_property())
  CHECK_EQ(False, aut6.has_twins_property())

has_twins_property_check()

aut1 = vcsn.automaton('''digraph {
  vcsn_context = "lal_char(abc)_b"
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "a"]
  1 -> 0 [label = "b"]
  1 -> 3 [label = "b"]
  2 -> 1 [label = "c"]
  2 -> 2 [label = "b"]
  3 -> F3
  3 -> 1 [label = "c"]
}''')

aut2 = vcsn.automaton('''digraph {
  vcsn_context = "lal_char(abc)_b"
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "a"]
  1 -> 0 [label = "b"]
  1 -> 3 [label = "b"]
  2 -> 1 [label = "c"]
  2 -> 2 [label = "b"]
  3 -> F3
  3 -> 1 [label = "b"]
}''')

aut3 = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abc)_b"
  I0 -> 0
  0 -> 1 [label = "c"]
  1 -> 2 [label = "a"]
  2 -> 3 [label = "b"]
  2 -> 4 [label = "b"]
  3 -> 5 [label = "c"]
  3 -> 6 [label = "c"]
  4 -> 7 [label = "c"]
  5 -> 2 [label = "a"]
  6 -> 0 [label = "b"]
  7 -> F7
  7 -> 2 [label = "a"]
}''')

r1 = "((abc)*){5}abc" + format(aut1.ratexp())
aut4 = vcsn.context("lal_char(abc)_b").ratexp(r1).derived_term()

r2 = "((abc)*){5}abc" + format(aut2.ratexp())
aut5 = vcsn.context("lal_char(abc)_b").ratexp(r2).derived_term()


def check_is_cycle_unambiguous():
  CHECK_EQ(True, aut1.is_cycle_unambiguous())
  CHECK_EQ(False, aut2.is_cycle_unambiguous())
  CHECK_EQ(True, aut3.is_cycle_unambiguous())
  CHECK_EQ(True, aut4.is_cycle_unambiguous())
  CHECK_EQ(False, aut5.is_cycle_unambiguous())

  CHECK_EQ(True, vcsn.context("lal_char(abc)_b").
           ladybird(5).is_cycle_unambiguous())
  CHECK_EQ(False, vcsn.context("lal_char(abc)_b").
           de_bruijn(5).is_cycle_unambiguous())
  CHECK_EQ(True, vcsn.context("lal_char(abc)_b").
           ladybird(20).is_cycle_unambiguous())
  CHECK_EQ(False, vcsn.context("lal_char(abc)_b").
           de_bruijn(20).is_cycle_unambiguous())


check_is_cycle_unambiguous()
