#! /usr/bin/env python

import vcsn
from test import *

def push_weights_check(i, o, words):
  i1 = i.push_weights()
  CHECK_EQ(o, i1)
  for w in words:
    CHECK_EQ(o.eval(w), i1.eval(w))

i = vcsn.automaton('''digraph {
  vcsn_context = "lal_char(abc), q"
  I0 -> 0
  0 -> 1 [label = "<2>a"]
  1 -> 2 [label = "<3>b"]
  2 -> 3 [label = "<5>c"]
  3 -> 4 [label = "<2>b"]
  1 -> 3 [label = "<8>c"]
  4 -> F4
}''')
o = vcsn.automaton('''digraph {
  vcsn_context = "lal_char(abc), q"
  I0 -> 0 [label = "<92>"]
  0 -> 1 [label = "a"]
  1 -> 2 [label = "<15/23>b"]
  1 -> 3 [label = "<8/23>c"]
  2 -> 3 [label = "c"]
  3 -> 4 [label = "b"]
  4 -> F4
}''')
words = ["abcb", "ac", "abc", "bc"]
push_weights_check(i, o, words)

i = vcsn.automaton('''digraph {
  vcsn_context = "lal_char(a-f), zmin"
  I0 -> 0
  0 -> 1 [label = "<0>a, <1>b, <4>c"]
  0 -> 2 [label = "<0>d,<1>e"]
  1 -> 3 [label = "<0>e, <1>f"]
  2 -> 3 [label = "<10>e, <11>f"]
  3 -> F3
}''')
o = vcsn.automaton('''digraph {
  vcsn_context = "lal_char(a-f), zmin"
  I0 -> 0
  0 -> 1 [label = "<0>a, <1>b, <4>c"]
  0 -> 2 [label = "<10>d, <11>e"]
  1 -> 3 [label = "<0>e, <1>f"]
  2 -> 3 [label = "<0>e, <1>f"]
  3 -> F3
}''')
words = ["a", "af", "bde", "bc", "ca"]
push_weights_check(i, o, words)

i = vcsn.automaton('''digraph {
  vcsn_context = "lal_char(abc), q"
  I0 -> 0
  0 -> 1 [label = "<5>a"]
  1 -> 2 [label = "<2>b"]
  1 -> 3 [label = "<3>b"]
  2 -> F2 [label = "<5>"]
  2 -> 3 [label = "<7>c"]
  3 -> F3 [label = "<2>"]
}''')
o = vcsn.automaton('''digraph {
  vcsn_context = "lal_char(abc), q"
  I0 -> 0 [label = "<220>"]
  0 -> 1 [label = "a"]
  1 -> 2 [label = "<19/22>b"]
  1 -> 3 [label = "<3/22>b"]
  2 -> F2 [label = "<5/19>"]
  2 -> 3 [label = "<14/19>c"]
  3 -> F3
}''')
words = ["ab", "abc", "ac", "cb"]
push_weights_check(i, o, words)

i = vcsn.automaton('''digraph {
  vcsn_context = "lal_char(abc), zmin"
  I0 -> 0
  0 -> 1 [label = "<5>a"]
  1 -> 2 [label = "<2>b"]
  1 -> 3 [label = "<3>b"]
  2 -> F2 [label = "<5>"]
  2 -> 3 [label = "<7>c"]
  3 -> F3 [label = "<2>"]
}''')
o = vcsn.automaton('''digraph {
  vcsn_context = "lal_char(abc), zmin"
  I0 -> 0 [label = "<10>"]
  0 -> 1 [label = "<0>a"]
  1 -> 2 [label = "<2>b"]
  1 -> 3 [label = "<0>b"]
  2 -> F2 [label = "<0>"]
  2 -> 3 [label = "<4>c"]
  3 -> F3 [label = "<0>"]
}''')
words = ["ab", "abc", "ac", "cb"]
push_weights_check(i, o, words)
