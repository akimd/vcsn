#! /usr/bin/env python

import vcsn
from test import *

## ---------- ##
## Automata.  ##
## ---------- ##

# check EXPECT INPUT
# ------------------
# Check that is-normalized(INPUT) = EXPECT.
def check(expect, i):
    CHECK_EQ(expect, i.is_normalized())
    # Check normalize
    CHECK_EQ(i.standard().costandard(), i.normalize())

# A simple, normalized, non-Thompson, automaton.
check(True, vcsn.automaton('''
digraph
{
  vcsn_context = "lal(ab), z"
  I0 -> 0
  0 -> 1 [label = "a,b"]
  1 -> F1
}
'''))

# No initials
check(False, vcsn.automaton('''
digraph
{
  vcsn_context = "lal(ab), z"
  0 -> 1 [label = "a,b"]
  1 -> F
}
'''))

# Two initials.
check(False, vcsn.automaton('''
digraph
{
  vcsn_context = "lal(ab), z"
  I -> 0
  0 -> 1 [label = "a,b"]
  I -> 1
  1 -> F
}
'''))

# No finals
check(False, vcsn.automaton('''
digraph
{
  vcsn_context = "lal(ab), z"
  I -> 0
  0 -> 1 [label = "a,b"]
}
'''))

# Two finals.
check(False, vcsn.automaton('''
digraph
{
  vcsn_context = "lal(ab), z"
  I -> 0
  0 -> F
  0 -> 1 [label = "a,b"]
  1 -> F
}
'''))

# Non-one initial weight.
check(False, vcsn.automaton('''
digraph
{
  vcsn_context = "lal(ab), z"
  I0 -> 0 [label = "<2>"]
  0 -> 1 [label = "a,b"]
  1 -> F1
}
'''))

# Non-one final weight.
check(False, vcsn.automaton('''
digraph
{
  vcsn_context = "lal(ab), z"
  I0 -> 0
  0 -> 1 [label = "a,b"]
  1 -> F1 [label = "<2>"]
}
'''))

# Arriving to initial state.
check(False, vcsn.automaton('''
digraph
{
  vcsn_context = "lal(ab), z"
  I -> 0
  0 -> 1 [label = "a,b"]
  1 -> 2 [label = "a"]
  1 -> 0 [label = "b"]
  2 -> F
}
'''))

# Leaving a final state.
check(False, vcsn.automaton('''
digraph
{
  vcsn_context = "lal(ab), z"
  I -> 0
  0 -> 1 [label = "a,b"]
  1 -> 2 [label = "a"]
  2 -> 1 [label = "b"]
  2 -> F
}
'''))

# Regression: we used to crash on this.
a = vcsn.automaton('''context = lal, z
$ -> 0
0 -> 1 <-1>a
0 -> 2 <1>a
1 -> $
2 -> $''', 'daut')
CHECK_EQ(False,
         a.transpose().determinize().transpose().is_normalized())

## ------------ ##
## Expansions.  ##
## ------------ ##
c = vcsn.context('lal, q')
e = c.expression('a*')
x = e.expansion()
CHECK(x.is_normal())
x = x.denormalize()
CHECK(not x.is_normal())
x = x.normalize()
CHECK(x.is_normal())

e = c.expression('(ab{\}ab +<-1>b*)')
x = e.expansion()
CHECK(not x.is_normal())
