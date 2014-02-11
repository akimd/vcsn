#! /usr/bin/env python

import vcsn
from test import *

# Eliminate state 0, or 1, or 2.
a = vcsn.automaton.load(medir + '/lao.gv')
for i in [0, 1, 2]:
    CHECK_EQ(vcsn.automaton.load(medir + '/lao-elim-{}.gv'.format(i)),
             a.eliminate_state(i))

PLAN()
