#! /usr/bin/env python

import vcsn
from test import *

def check(num_sccs, a):
    algos = ["tarjan_iterative", "tarjan_recursive", "kosaraju"]
    for algo in algos:
        ati = a.scc(algo)
        CHECK_EQ(True, ati.is_isomorphic(a))
        CHECK_EQ(num_sccs, ati.num_components())
        CHECK_EQ(num_sccs, ati.condense().info()["number of states"])

a = vcsn.automaton('''digraph {
  vcsn_context = "lal_char(abc), b"
}''')
check(0, a)

a = vcsn.automaton('''digraph {
  vcsn_context = "lal_char(abc), b"
  I0 -> 0
  0 -> F0
}''')
check(1, a)

a = vcsn.automaton('''digraph {
  vcsn_context = "lal_char(abc), b"
  I0 -> 0
  0 -> 1 [label = "a"]
  1 -> F1
}''')
check(2, a)

a = vcsn.automaton('''digraph {
  vcsn_context = "lal_char(abc), b"
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "a"]
  1 -> 2 [label = "b"]
  2 -> 0 [label = "b"]
}''')
check(1, a)

a = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abc), b"
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
check(1, a)

a = vcsn.automaton('''digraph
{
  vcsn_context = "lal_char(abcd), zmin"
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
check(4, a)

ctx = vcsn.context("lal_char(abc), z")
a = ctx.expression("(abc)*{5}").standard()
check(6, a)

# component
a = ctx.expression("(ab)*(bc)*").standard()
ati = a.scc("tarjan_iterative")
CHECK_EQ(ati.component(1), ati.filter([4, 6]))
CHECK_EQ(ati.component(2), ati.filter([1, 3]))
CHECK_EQ(ati.component(3), ati.filter([0]))

try:
    ati.component(0)
except RuntimeError:
    PASS()
else:
    FAIL('num_components egal 0: did not raise an exception')

try:
    ati.component(4)
except RuntimeError:
    PASS()
else:
    FAIL('num_components egal 0: did not raise an exception')
