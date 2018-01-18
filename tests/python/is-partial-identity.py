#! /usr/bin/env python

import vcsn
from test import *

## ----- ##
## LAL.  ##
## ----- ##

a = vcsn.automaton('''digraph {
  vcsn_context = "lat<lal(abc), lal>, z"
  I0 -> 0
  0 -> 1 [label = "(a, a)"]
  1 -> 2 [label = "(b, b)"]
  2 -> F2
}''')
CHECK(a.is_partial_identity())

a = vcsn.automaton('''digraph {
  vcsn_context = "lat<lal(abc), lal>, z"
  I0 -> 0
  0 -> 1 [label = "(a, a)"]
  1 -> 2 [label = "(b, c)"]
  2 -> F2
}''')
CHECK(not a.is_partial_identity())



## ----- ##
## LAW.  ##
## ----- ##

a = vcsn.automaton(r'''digraph {
  vcsn_context = "lat<law_char(abc),law_char(abc)>, b"
  I -> 0
  0 -> 1 [label = "(aaa, a)"]
  1 -> 0 [label = "(a, aaa)"]
  0 -> 2 [label = "(bb, \\e)"]
  2 -> 0 [label = "(bb, bbbb)"]
  0 -> F
}''')
CHECK(a.is_partial_identity())

a = vcsn.automaton(r'''digraph {
  vcsn_context = "lat<law_char(abc),law_char(abcd)>, b"
  I -> 0
  0 -> 1 [label = "(aaa, a)"]
  1 -> 0 [label = "(a, aaa)"]
  0 -> 2 [label = "(bb, \\e)"]
  2 -> 0 [label = "(bb, bbb)"]
  0 -> F
}''')
CHECK(not a.is_partial_identity())
