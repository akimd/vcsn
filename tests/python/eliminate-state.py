#! /usr/bin/env python

import vcsn
from test import *

# Eliminate state 0, or 1, or 2.
a = vcsn.automaton(filename = medir + '/lao.gv')
for i in [0, 1, 2]:
    CHECK_EQ(open(medir + '/lao-elim-{}.gv'.format(i)).read().strip(),
             str(a.eliminate_state(i)))
