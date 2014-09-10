#! /usr/bin/env python

import vcsn
from test import *

a = vcsn.automaton('''digraph {
  vcsn_context = "lat<lal_char(abc), lal_char(xyz)>_z"
  I0 -> 0
  0 -> 1 [label = "<2>(a, x)"]
  1 -> 2 [label = "<3>(b, y)"]
  2 -> F2
}''')
CHECK_EQ(True, a.is_functional())

a = vcsn.automaton(r'''digraph {
  vcsn_context = "lat<lal_char(abc),lal_char(xyz)>_b"
  I0 -> 0
  0 -> 1 [label = "(a, x)"]
  0 -> 2 [label = "(a, x)"]
  1 -> 3 [label = "(b, y)"]
  2 -> 3 [label = "(b, y)"]
  3 -> F3
}''')
CHECK_EQ(True, a.is_functional())

a = vcsn.automaton('''digraph {
  vcsn_context = "lat<lal_char(abc),lal_char(xyz)>_b"
  I0 -> 0
  0 -> 1 [label = "(a, x)"]
  0 -> 2 [label = "(a, y)"]
  1 -> 3 [label = "(b, y)"]
  2 -> 3 [label = "(b, y)"]
  3 -> F3
}''')
CHECK_EQ(False, a.is_functional())

a = vcsn.automaton(r'''digraph {
  vcsn_context = "lat<lan<lal_char(abc)>,lan<lal_char(xyz)>>_b"
  I0 -> 0
  0 -> 1 [label = "(a, \\e)"]
  0 -> 2 [label = "(a, x)"]
  1 -> 3 [label = "(b, x)"]
  2 -> 3 [label = "(b, \\e)"]
  3 -> F3
}''')
CHECK_EQ(True, a.is_functional())

a = vcsn.automaton(r'''digraph {
  vcsn_context = "lat<lan_char(xy),lan_char(ab)>_b"
  I0 -> 0
  0 -> 1 [label = "(x, a)"]
  0 -> 2 [label = "(x, a)"]
  1 -> 3 [label = "(x, a)"]
  2 -> 3 [label = "(x, \\e)"]
  3 -> 4 [label = "(y, b)"]
  4 -> F4
}''')
CHECK_EQ(False, a.is_functional())

a = vcsn.automaton('''digraph {
  vcsn_context = "lat<lal_char(ab), lal_char(xy)>_z"
  I0 -> 0
  0 -> 1 [label = "<2>(a, x), <3>(b, y)"]
  1 -> F1
}''')
CHECK_EQ(True, a.is_functional())
