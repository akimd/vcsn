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
  $ -> 0
  0 -> 1 a
  0 -> 2 b
  1 -> 0 b
  2 -> 1 a
  2 -> 2 b
  1 -> $
'''))

# A spontaneous transition.
check(False, vcsn.automaton('''
  $ -> 0
  0 -> 1 \e
  1 -> $
'''))

# A loop.
check(False, vcsn.automaton('''
  $ -> 0
  0 -> 1 a
  0 -> 0 a
  1 -> 1 b
  1 -> 2 b
  2 -> 2 c
  2 -> $
'''))

check(True, vcsn.automaton('''
  $ -> 0
  0 -> 1 a
  0 -> 2 b
  1 -> 3 a
  2 -> 3 b
  3 -> 3 a
  3 -> 3 b
  3 -> $
'''))

check(True, vcsn.automaton('''
  $ -> 0
  0 -> 1 b
  0 -> 2 a
  1 -> 2 a
  2 -> $
'''))

# No states.
check(True, vcsn.automaton('''
digraph
{
  vcsn_context = "[a]? → 𝔹"
}
'''))

# No initial state.
check(True, vcsn.automaton('''
  0 -> 1 a
'''))

# Two initial states.
check(False, vcsn.automaton('''
  $ -> 0
  $ -> 1
  0 -> 2 a
  1 -> 2 a
  2 -> $
'''))

# An unreachable not deterministic state.
check(False, vcsn.automaton('''
  $ -> 0
  1 -> 2 a
  1 -> 3 a
  2 -> $
'''))



# A word transition.
check(False, vcsn.automaton('''
  context = [...]* → 𝔹
  $ -> 0
  0 -> 1 ab
  1 -> $
'''))

# Wordset, but deterministic.
check(True, vcsn.automaton('''
  context = [...]* → 𝔹
  $ -> 0
  0 -> 1 a, b
  1 -> $
'''))
