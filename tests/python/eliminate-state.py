#! /usr/bin/env python

import vcsn
from test import *

# Eliminate state 0, or 1, or 2.
a = vcsn.automaton(filename = medir + '/lao.gv')
for i in [0, 1, 2]:
    print("Eliminating state", i)
    exp = open(medir + '/lao-elim-{}.gv'.format(i)).read().strip()
    CHECK_EQ(exp, a.eliminate_state(i))

XFAIL(lambda: a.eliminate_state(3))

a = vcsn.B.expression('ab*c').standard().lift()
for i in range(4):
    print("elimination:", i)
    exp = open(medir + '/abc-elim-{}.gv'.format(i)).read().strip()
    a = a.eliminate_state(-1)
    CHECK_EQ(exp, a)

# Remove states in an empty automaton.
XFAIL(lambda: a.eliminate_state(0))
XFAIL(lambda: a.eliminate_state(-3))
XFAIL(lambda: a.eliminate_state())
