#! /usr/bin/env python

import vcsn
from test import *

## ---------- ##
## Automata.  ##
## ---------- ##
a = vcsn.Q.expression('<3>abc*(<2>d)*+ce<5>').automaton()
CHECK_EQ(metext('lal,q.gv'), a.partial_identity())
CHECK_EQ(a, a.partial_identity().project(0))
CHECK_EQ(a, a.partial_identity().project(1))


## ------------- ##
## Expressions.  ##
## ------------- ##

c1 = vcsn.Q
c2 = c1 | c1
def check(e, exp, ids='auto'):
    e = c1.expression(e, ids)
    exp = c2.expression(exp, ids)
    e2 = e.partial_identity()
    CHECK_EQ(exp, e2)
    CHECK_EQ(exp.identities(), e2.identities())
    # The partial identity of the automaton is the automaton of the
    # partial identity.
    CHECK_EQ(e.automaton().partial_identity(), e2.automaton())
    # The projection of the partial identity is the original
    # expression.
    for tape in range(e2.context().num_tapes()):
        CHECK_EQ(e, e2.project(tape))

check('a', 'a|a')
check('abc', '(a|a)(b|b)(c|c)')
check('ab*c', '(a|a)(b|b)*(c|c)')
check('[abc]', '(a|a)+(b|b)+(c|c)')
check('<2>a', '<2>(a|a)')
# Make check we preserve the identities.
check('<2>ab<3>', '<2>(a|a)(b|b)<3>', 'associative')
