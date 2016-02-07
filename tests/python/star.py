#! /usr/bin/env python

import vcsn
from test import *

def check_algo(input):
    if isinstance(input, str):
        input = ctx.expression(input, identities='none').automaton()
    gen = input.star("general")
    if not input.is_standard():
        XFAIL(lambda: input.star("standard"))
    std = input.standard().star("standard")
    CHECK_EQUIV(gen, std)
    det = input.standard().star("deterministic")
    CHECK(det.is_deterministic(), det, "is_deterministic")
    CHECK_EQUIV(gen, det)
    CHECK(std.is_standard(), std, "is standard")

def check(input, exp):
    if isinstance(input, str):
        input = vcsn.automaton(input)
    if isinstance(exp, str):
        exp = vcsn.automaton(exp)
    CHECK_EQ(exp, input.star())
    # Cannot call determinize algo on expressionset
    # check_algo(input)

ctx = vcsn.context('lal_char, q')

check_algo('a')
check_algo('ab')
check_algo('a+b')
check_algo('a<2>')

# This used to trigger an assert.
l_br = vcsn.context('lal_char(a), expressionset<lal_char(xy), b>')
check(l_br.expression('<y>a(<x>a)*').automaton('derived_term'),
'''
digraph
{
  vcsn_context = "lal_char(a), expressionset<lal_char(xy), b>"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F0
    F1
  }
  {
    node [shape = circle]
    0
    1
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "<y>a"]
  1 -> F1
  1 -> 1 [label = "<x+y>a"]
}
''')

check('''
digraph
{
  vcsn_context = "lal_char(ab), b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F1
    F2
  }
  {
    node [shape = circle]
    0
    1
    2
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> F1
  2 -> F2
}
''',
'''
digraph
{
  vcsn_context = "lal_char(ab), b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F0
    F1
    F2
  }
  {
    node [shape = circle]
    0
    1
    2
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> F1
  1 -> 1 [label = "a"]
  1 -> 2 [label = "b"]
  2 -> F2
  2 -> 1 [label = "a"]
  2 -> 2 [label = "b"]
}
''')


check('''
digraph
{
  vcsn_context = "lal_char(ab), expressionset<lal_char(xyz), b>"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I
    F0
    F1
  }
  {
    node [shape = circle]
    0
    1
    2
  }
  I -> 0
  0 -> 1 [label = "<x>a"]
  1 -> 2 [label = "<y>b"]
  2 -> F0 [label = "<y>"]
  0 -> F1 [label = "<z>"]
}
''',
'''
digraph
{
  vcsn_context = "lal_char(ab), expressionset<lal_char(xyz), b>"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F0
    F2
  }
  {
    node [shape = circle]
    0
    1
    2
  }
  I0 -> 0
  0 -> F0 [label = "<z*>"]
  0 -> 1 [label = "<z*x>a"]
  1 -> 2 [label = "<y>b"]
  2 -> F2 [label = "<yz*>"]
  2 -> 1 [label = "<yz*x>a"]
}
''')
