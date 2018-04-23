#! /usr/bin/env python

import vcsn
from test import *

# check(input, expected-output)
# -----------------------------
def check(aut, exp):
    if isinstance(aut, str):
        aut = vcsn.automaton(aut)
    CHECK(not aut.is_complete())
    exp = vcsn.automaton(exp)
    CHECK_EQ(exp, aut.complete())
    CHECK(exp.is_complete())
    # Idempotence.
    CHECK_EQ(exp, exp.complete())


check('''
context = [abcd]? → 𝔹
$ 0
0 1 a
0 2 b
1 2 c
2 $
''', '''
context = [abcd]? → 𝔹
$ -> 0
0 -> 1 a
0 -> 2 b
0 -> 3 c, d
1 -> 2 c
1 -> 3 a, b, d
2 -> $
2 -> 3 a, b, c, d
3 -> 3 a, b, c, d
''')

# An automaton with an open context.
check(vcsn.B.expression('a').standard(), '''
context = [a]? → 𝔹
  $ -> 0
  0 -> 1 a
  1 -> $
  1 -> 2 a
  2 -> 2 a
''')

# An automaton without initial state.
check('''
context = [a]? → 𝔹
0 -> 0 a
0 -> $
''', '''
  0 -> $
  0 -> 0 a
  1 -> 1 a
  $ -> 1
''')
