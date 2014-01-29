#! /usr/bin/env python

# Check the factories (of automata, ratexps, etc.)

import vcsn
from test import *

def XFAIL(fun):
    try:
        fun()
    except RuntimeError:
        PASS()
    else:
        FAIL('did not raise an exception', fun)

## ----------- ##
## divkbaseb.  ##
## ----------- ##

b = vcsn.context('lal_char(0-4)_b')

XFAIL(lambda: b.divkbaseb(0, 2))
XFAIL(lambda: b.divkbaseb(2, 0))
XFAIL(lambda: b.divkbaseb(2, 1))
XFAIL(lambda: b.divkbaseb(2, 10))

## -------- ##
## random.  ##
## -------- ##

# Expect a clique.
c1 = vcsn.context('lal_char(a)_b').random(4, 1, 4, 4)
c2 = vcsn.automaton.load(medir + "/" + 'clique-a-4.gv')
CHECK_EQ(c1, c2)

# Expect the right number of states.
a = vcsn.context('lal_char(a)_b').random(100, .1, 20, 30)
CHECK_EQ(a.info()['type'], 'mutable_automaton<lal_char(a)_b>')
CHECK_EQ(a.info()['number of states'], 100)
CHECK_EQ(a.info()['number of initial states'], 20)
CHECK_EQ(a.info()['number of final states'], 30)

PLAN()
