#! /usr/bin/env python

import vcsn
from test import *

## ---------- ##
## automata.  ##
## ---------- ##

# Standard automata.
q = vcsn.context('lal(ab), q')
# This automaton is standard.
r = q.expression(r'\e+[ab]b[ab]*', 'associative')

# Derived-term commutes with kmul.
CHECK_EQ(('3/4' * r).automaton(),
         '3/4' * (r.automaton()))

CHECK_EQ((r * '3/4').automaton(),
         r.automaton() * '3/4')

# Standard commutes with kmul. They should be equal, but "standard"
# leaves holes in the state numbers.
CHECK_ISOMORPHIC(('3/4' * r).standard(),
                 '3/4' * r.standard())

CHECK_ISOMORPHIC((r * '3/4').standard(),
                 r.standard() * '3/4')

# Check the case of multiplication by 0: generate the standard
# automaton for the empty language.
CHECK_EQ((0 * r).standard(),
         0 * (r.standard()))

CHECK_EQ((r * 0).standard(),
         r.standard() * 0)

# Non-standard automata.  This time, it does not commute.
ab = q.expression('ab').automaton()
a = ab.add(ab, "general")
CHECK_EQUIV(q.expression('<3/4>ab+<3/4>ab').derived_term(),
            '3/4' * a)
CHECK_EQUIV(q.expression('(ab)<3/4>+(ab)<3/4>').derived_term(),
            a * '3/4')

# Check the case of multiplication by 0: generate the empty automaton.
CHECK_EQ(q.expression(r'\z').automaton(),
         0 * a)
CHECK_EQ(q.expression(r'\z').automaton(),
         a * 0)

## ------------ ##
## expression.  ##
## ------------ ##

ctx = vcsn.context('lal(ab), expressionset<lal(xyz), q>')
r = ctx.expression('<x>(<y>a)*<z>')
CHECK_EQ(ctx.expression('<xx>(<y>a)*<z>'), 'x' * r)
CHECK_EQ(ctx.expression('<x>(<y>a)*<zz>'), r * 'z')
