#! /usr/bin/env python

import vcsn
from test import *

# Eliminate state 0, or 1, or 2.
a = vcsn.automaton(filename = medir + '/lao.gv')
for i in [0, 1, 2]:
    print("Eliminating state", i)
    exp = open(medir + '/lao-elim-{}.gv'.format(i)).read().strip()
    CHECK_EQ(exp, a.eliminate_state(i))
