#! /usr/bin/env python

import vcsn
from test import *

## ----- ##
## LAL.  ##
## ----- ##

a = vcsn.automaton('''digraph {
  vcsn_context = "[abc]? × [xyz]? → ℤ"
  I0 -> 0
  0 -> 1 [label = "<2>(a, x)"]
  1 -> 2 [label = "<3>(b, y)"]
  2 -> F2
}''')
CHECK(a.is_functional())

a = vcsn.automaton(r'''digraph {
  vcsn_context = "[abc]? × [xyz]? → 𝔹"
  I0 -> 0
  0 -> 1 [label = "(a, x)"]
  0 -> 2 [label = "(a, x)"]
  1 -> 3 [label = "(b, y)"]
  2 -> 3 [label = "(b, y)"]
  3 -> F3
}''')
CHECK(a.is_functional())

a = vcsn.automaton('''digraph {
  vcsn_context = "[abc]? × [xyz]? → 𝔹"
  I0 -> 0
  0 -> 1 [label = "(a, x)"]
  0 -> 2 [label = "(a, y)"]
  1 -> 3 [label = "(b, y)"]
  2 -> 3 [label = "(b, y)"]
  3 -> F3
}''')
CHECK(not a.is_functional())

a = vcsn.automaton('''digraph {
  vcsn_context = "[abc]? × [xyz]? → 𝔹"
  I0 -> 0
  0 -> 1 [label = "(a, x)"]
  1 -> 2 [label = "(b, y)"]
  2 -> F
  0 -> 3 [label = "(a, y)"]
  3 -> 4 [label = "(b, y)"]
  4 -> F
}''')
CHECK(not a.is_functional())

a = vcsn.automaton('''digraph {
  vcsn_context = "[abc]? × [xyz]? → 𝔹"
  I0 -> 0
  0 -> 1 [label = "(a, x)"]
  1 -> 2 [label = "(b, y)"]
  2 -> F
  0 -> 3 [label = "(a, x)"]
  3 -> 4 [label = "(b, y)"]
  4 -> F
}''')
CHECK(a.is_functional())

a = vcsn.automaton('''digraph {
  vcsn_context = "[ab]? × [xy]? → ℤ"
  I0 -> 0
  0 -> 1 [label = "<2>(a, x), <3>(b, y)"]
  1 -> F1
}''')
CHECK(a.is_functional())

## ----- ##
## LAN.  ##
## ----- ##

a = vcsn.automaton(r'''digraph {
  vcsn_context = "[abc]? × [xyz]? → 𝔹"
  I0 -> 0
  0 -> 1 [label = "(a, \\e)"]
  0 -> 2 [label = "(a, x)"]
  1 -> 3 [label = "(b, x)"]
  2 -> 3 [label = "(b, \\e)"]
  3 -> F3
}''')
CHECK(a.is_functional())

a = vcsn.automaton(r'''digraph {
  vcsn_context = "[xy]? × [ab]? → 𝔹"
  I0 -> 0
  0 -> 1 [label = "(x, a)"]
  0 -> 2 [label = "(x, a)"]
  1 -> 3 [label = "(x, a)"]
  2 -> 3 [label = "(x, \\e)"]
  3 -> 4 [label = "(y, b)"]
  4 -> F4
}''')
CHECK(not a.is_functional())

# states non coaccessible
a = vcsn.automaton(r'''digraph {
  vcsn_context = "[abc]? × [xyz]? → 𝔹"
  I0 -> 0
  0 -> 1 [label = "(a, x), (b, y)"]
  1 -> F1
  0 -> 2 [label = "(b, y)"]
  0 -> 3 [label = "(b, y)"]
  2 -> 4 [label = "(c, x)"]
  3 -> 4 [label = "(c, z)"]
}''')
CHECK(a.is_functional())

## ----- ##
## LAW.  ##
## ----- ##

a = vcsn.automaton(r'''digraph {
  vcsn_context = "[abc]* × [xyz]* → 𝔹"
  I0 -> 0
  0 -> 1 [label = "(aaa, x)"]
  1 -> 2 [label = "(a, x)"]
  2 -> F
  0 -> 1 [label = "(aa, \\e)"]
  2 -> 3 [label = "(aa, xxx)"]
  3 -> F
}''')
CHECK(a.is_functional())

a = vcsn.automaton(r'''digraph {
  vcsn_context = "[abc]* × [xyz]* → 𝔹"
  I0 -> 0
  0 -> 1 [label = "(aaa, xx)"]
  1 -> 2 [label = "(a, x)"]
  2 -> F
  0 -> 1 [label = "(aa, x)"]
  2 -> 3 [label = "(aa, xx)"]
  3 -> F
}''')
CHECK(a.is_functional())
