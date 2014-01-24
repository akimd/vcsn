#! /usr/bin/env python

# Check the factories (of automata, ratexps, etc.)

import vcsn
from test import *

b = vcsn.context('lal_char(a)_b')

## -------- ##
## random.  ##
## -------- ##

# Expect a clique.
c1 = b.random(4, 1, 4, 4)
c2 = vcsn.automaton.load(medir + "/" + 'clique-a-4.gv')
CHECK_EQ(c1, c2)

# Expect the right number of states.
a = b.random(100, .1, 20, 30)
CHECK_EQ(a.info()['type'], 'mutable_automaton<lal_char(a)_b>')
CHECK_EQ(a.info()['number of states'], 100)
CHECK_EQ(a.info()['number of initial states'], 20)
CHECK_EQ(a.info()['number of final states'], 30)

PLAN()
