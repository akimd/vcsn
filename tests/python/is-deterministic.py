#! /usr/bin/env python

import vcsn
from test import *

# check_lal EXPECT, INPUT
# -----------------------
# Check that is-deterministic(INPUT) = EXPECT in lal.
def check(expect, i):
    CHECK_EQ(expect, i.is_deterministic())
    CHECK_EQ(expect, i.info('is deterministic'))
    # check codeterministic.
    t = i.transpose()
    deter = t.is_deterministic()
    CHECK_EQ(deter, i.is_codeterministic())
    CHECK_EQ(deter, t.info('is deterministic'))

check(True, vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab), b"
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> 0 [label = "b"]
  2 -> 1 [label = "a"]
  2 -> 2 [label = "b"]
  1 -> F1
}
'''))

# A loop.
check(False, vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abc), b"
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 0 [label = "a"]
  1 -> 1 [label = "b"]
  1 -> 2 [label = "b"]
  2 -> 2 [label = "c"]
  2 -> F2
}
'''))

check(True, vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab), b"
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> 3 [label = "a"]
  2 -> 3 [label = "b"]
  3 -> 3 [label = "a"]
  3 -> 3 [label = "b"]
  3 -> F3
}
'''))

check(True, vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab), b"
  I0 -> 0
  0 -> 1 [label = "b"]
  0 -> 2 [label = "a"]
  1 -> 2 [label = "a"]
  2 -> F2
}
'''))

# No states.
check(True, vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(a), b"
}
'''))

# No initial states.
check(True, vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(a), b"
  0 -> 1 [label = "a"]
}
'''))

# Two initial states.
check(False, vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(a), b"
  I0 -> 0
  I1 -> 1
  0 -> 2 [label = "a"]
  1 -> 2 [label = "a"]
  2 -> F2
}
'''))

# An unreachable not deterministic state.
check(False, vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(a), b"
  I0 -> 0
  1 -> 2 [label = "a"]
  1 -> 3 [label = "a"]
  2 -> F2
}
'''))
