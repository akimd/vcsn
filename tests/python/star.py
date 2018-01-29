#! /usr/bin/env python

import vcsn
from test import *


def check_algo(input):
    if isinstance(input, str):
        input = ctx.expression(input, identities='trivial')
    if isinstance(input, vcsn.expression):
        input = input.automaton()

    print("general")
    gen = input.star("general")

    print("standard")
    if not input.is_standard():
        XFAIL(lambda: input.star("standard"))
        std = input.standard().star("standard")
    else:
        std = input.star("standard")
    CHECK(std.is_standard(), std, "is standard")
    CHECK_EQUIV(gen, std)

    # Avoid determinizations that may not terminate.
    if 'expressionset' not in input.context().format('sname'):
        print("deterministic")
        det = input.star("deterministic")
        CHECK(det.is_deterministic(), det, "is_deterministic")
        CHECK_EQUIV(gen, det)


def check(input, exp):
    if isinstance(input, str):
        input = vcsn.automaton(input)
    if isinstance(exp, str):
        exp = vcsn.automaton(exp)
    CHECK_EQ(exp, input.star())
    check_algo(input)


ctx = vcsn.context('lal, q')

check_algo('a')
check_algo('ab')
check_algo('a+b')
check_algo(ctx.expression('a<2>', 'none'))
check_algo('<1/2>a*+<1/3>b*')

# This used to trigger an assert.
l_br = vcsn.context('lal(a), expressionset<lal(xy), b>')
check(l_br.expression('<y>a(<x>a)*').automaton('derived_term'),
'''
digraph
{
  vcsn_context = "[a]? → RatE[[xy]? → 𝔹]"
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
context = [ab]? → 𝔹
$ -> 0
0 -> 1 a
0 -> 2 b
1 -> $
2 -> $
''',
'''
digraph
{
  vcsn_context = "[ab]? → 𝔹"
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
  vcsn_context = "[ab]? → RatE[[xyz]? → 𝔹]"
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
  vcsn_context = "[ab]? → RatE[[xyz]? → 𝔹]"
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
