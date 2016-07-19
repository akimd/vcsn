#! /usr/bin/env python

import vcsn
from test import *

## ---------- ##
## Automata.  ##
## ---------- ##
a = vcsn.Q.expression('<3>abc*(<2>d)*+ce<5>').automaton()
CHECK_EQ(metext('lal,q.gv'), a.partial_identity())


