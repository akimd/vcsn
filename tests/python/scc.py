#! /usr/bin/env python

import vcsn
from test import *

def check(num_sccs, a):
    for algo in ['dijkstra', 'kosaraju', 'tarjan_iterative', 'tarjan_recursive']:
        scc = a.scc(algo)
        CHECK(scc.is_isomorphic(a))
        CHECK_EQ(num_sccs, scc.num_components())
        CHECK_EQ(num_sccs, scc.condense().info()['number of states'])

a = vcsn.automaton('''digraph {
  vcsn_context = "[abc]? → 𝔹"
}''')
check(0, a)

a = vcsn.automaton('''digraph {
  vcsn_context = "[abc]? → 𝔹"
  I0 -> 0
  0 -> F0
}''')
check(1, a)

a = vcsn.automaton('''digraph {
  vcsn_context = "[abc]? → 𝔹"
  I0 -> 0
  0 -> 1 [label = "a"]
  1 -> F1
}''')
check(2, a)

a = vcsn.automaton('''digraph {
  vcsn_context = "[abc]? → 𝔹"
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
  vcsn_context = "[abc]? → 𝔹"
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
  vcsn_context = "[abcd]? → 𝔹"
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "a"]
  1 -> 3 [label = "b"]
  1 -> 4 [label = "c"]
  2 -> 4 [label = "d"]
  2 -> 5 [label = "b"]
  3 -> 6 [label = "c"]
  4 -> F4
  5 -> 7 [label = "c"]
  6 -> 1 [label = "d"]
  7 -> 2 [label = "d"]
}''')
check(4, a)

ctx = vcsn.context('[abc] -> b')
a = ctx.expression('(abc)*{5}').standard()
check(6, a)

# component
a = ctx.expression('(ab)*(bc)*').standard()
scc = a.scc('tarjan_iterative')
CHECK_EQ(scc.component(0), scc.filter([0]))
CHECK_EQ(scc.component(1), scc.filter([1, 3]))
CHECK_EQ(scc.component(2), scc.filter([4, 6]))
XFAIL(lambda: scc.component(3))


# condense: make sure we don't lose inaccessible parts.
a = vcsn.automaton(
    r'''
    0 -> 0 a
    0 -> 1 b
    1 -> 1 c''',
    'daut')
CHECK_EQ(r'''digraph
{
  vcsn_context = "[abc]? → 𝔹"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "0.0", shape = box, color = DimGray]
    1 [label = "1.1", shape = box, color = DimGray]
  }
  0 -> 0 [label = "a", color = DimGray]
  0 -> 1 [label = "b", color = DimGray]
  1 -> 1 [label = "c", color = DimGray]
}''',
         a.scc())

CHECK_EQ(r'''digraph
{
  vcsn_context = "[abc]? → 𝔹"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "0.0", shape = box, color = DimGray]
    1 [label = "1.1", shape = box, color = DimGray]
  }
  0 -> 1 [label = "b", color = DimGray]
}''',
         a.scc().condense())
