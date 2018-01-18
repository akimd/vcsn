#! /usr/bin/env python

import vcsn
from test import *

def check(i, o):
  i1 = i.push_weights()
  CHECK_EQ(o, i1)
  # Make sure the expected result is consistant.
  CHECK_EQUIV(o, i1)

# q
i = vcsn.automaton('''digraph {
  vcsn_context = "lal(abc), q"
  I0 -> 0
  0 -> 1 [label = "<2>a"]
  1 -> 2 [label = "<3>b"]
  2 -> 3 [label = "<5>c"]
  3 -> 4 [label = "<2>b"]
  1 -> 3 [label = "<8>c"]
  4 -> F4
}''')
o = vcsn.automaton('''digraph {
  vcsn_context = "lal(abc), q"
  I0 -> 0 [label = "<92>"]
  0 -> 1 [label = "a"]
  1 -> 2 [label = "<15/23>b"]
  1 -> 3 [label = "<8/23>c"]
  2 -> 3 [label = "c"]
  3 -> 4 [label = "b"]
  4 -> F4
}''')
check(i, o)

# zmin.
i = vcsn.automaton('''digraph {
  vcsn_context = "lal(a-f), zmin"
  I0 -> 0
  0 -> 1 [label = "<0>a, <1>b, <4>c"]
  0 -> 2 [label = "<0>d,<1>e"]
  1 -> 3 [label = "<0>e, <1>f"]
  2 -> 3 [label = "<10>e, <11>f"]
  3 -> F3
}''')
o = vcsn.automaton('''digraph {
  vcsn_context = "lal(a-f), zmin"
  I0 -> 0
  0 -> 1 [label = "<0>a, <1>b, <4>c"]
  0 -> 2 [label = "<10>d, <11>e"]
  1 -> 3 [label = "<0>e, <1>f"]
  2 -> 3 [label = "<0>e, <1>f"]
  3 -> F3
}''')
check(i, o)

# q
i = vcsn.automaton('''digraph {
  vcsn_context = "lal(abc), q"
  I0 -> 0
  0 -> 1 [label = "<5>a"]
  1 -> 2 [label = "<2>b"]
  1 -> 3 [label = "<3>b"]
  2 -> F2 [label = "<5>"]
  2 -> 3 [label = "<7>c"]
  3 -> F3 [label = "<2>"]
}''')
o = vcsn.automaton('''digraph {
  vcsn_context = "lal(abc), q"
  I0 -> 0 [label = "<220>"]
  0 -> 1 [label = "a"]
  1 -> 2 [label = "<19/22>b"]
  1 -> 3 [label = "<3/22>b"]
  2 -> F2 [label = "<5/19>"]
  2 -> 3 [label = "<14/19>c"]
  3 -> F3
}''')
check(i, o)

# zmin
i = vcsn.automaton('''digraph {
  vcsn_context = "lal(abc), zmin"
  I0 -> 0
  0 -> 1 [label = "<5>a"]
  1 -> 2 [label = "<2>b"]
  1 -> 3 [label = "<3>b"]
  2 -> F2 [label = "<5>"]
  2 -> 3 [label = "<7>c"]
  3 -> F3 [label = "<2>"]
}''')
o = vcsn.automaton('''digraph {
  vcsn_context = "lal(abc), zmin"
  I0 -> 0 [label = "<10>"]
  0 -> 1 [label = "<0>a"]
  1 -> 2 [label = "<2>b"]
  1 -> 3 [label = "<0>b"]
  2 -> F2 [label = "<0>"]
  2 -> 3 [label = "<4>c"]
  3 -> F3 [label = "<0>"]
}''')
check(i, o)

# Exercize a bug because of which once a node had a distance, no
# better distance would ever be considered.  This resulted in an
# incorrect weight_pushing.
#
# This automaton has two routes from 0 to 4: a single-transition
# costing 10, and a four-transition one costing 0.  Yet we use the
# long one.
i = vcsn.automaton('''
context = "lal(a), zmin"
$ -> 0
0 -> 1 <0>a
1 -> 2 <0>a
2 -> 3 <0>a
3 -> 4 <0>a
4 -> 5 <2>a
0 -> 4 <10>a
5 -> $
''')

o = vcsn.automaton('''
context = "lal(a), zmin"
$ -> 0 <2>
0 -> 1 <0>a
1 -> 2 <0>a
2 -> 3 <0>a
3 -> 4 <0>a
4 -> 5 <0>a
5 -> $ <0>
0 -> 4 <10>a
''')
check(i, o)
