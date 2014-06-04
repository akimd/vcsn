#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context('lal_char(ab)_ratexpset<lal_char(xyz)_b>')

## ---------- ##
## automata.  ##
## ---------- ##

# Standard automata.
q = vcsn.context('lal_char(ab)_q')
# This automaton is standard.
r = q.ratexp('\e+[ab]b[ab]*')

# Derived-term commutes with kmul.
CHECK_EQ(('3/4' * r).derived_term().strip(),
         '3/4' * (r.derived_term().strip()))

CHECK_EQ((r * '3/4').derived_term().strip(),
         r.derived_term().strip() * '3/4')

# Standard commutes with kmul. They should be equal, but "standard"
# leaves holes in the state numbers.
CHECK_ISOMORPHIC(('3/4' * r).standard(),
                 '3/4' * (r.standard()))

CHECK_ISOMORPHIC((r * '3/4').standard(),
                 r.standard() * '3/4')

# Non-standard automata.  This time, it does not commute,
# unfortunately we don't have a CHECK_EQUIV, because we don't have a
# are_equivalent that works for non-deterministic automata.
a = q.ratexp('ab').derived_term() | q.ratexp('ab').derived_term()
CHECK_EQ(q.ratexp('<3/4>ab+<3/4>ab'),
         ('3/4' * a).ratexp())
CHECK_EQ(q.ratexp('(ab)<3/4>+(ab)<3/4>'),
         (a * '3/4').ratexp())

# Check the case of multiplication by 0.
CHECK_EQ(q.ratexp('\z').standard(),
         0 * a)
CHECK_EQ(q.ratexp('\z').standard(),
         a * 0)

## -------- ##
## ratexp.  ##
## -------- ##

r = ctx.ratexp('<x>(<y>a)*<z>')
CHECK_EQ(ctx.ratexp('<xx>(<y>a)*<z>'), 'x' * r)
CHECK_EQ(ctx.ratexp('<x>(<y>a)*<zz>'), r * 'z')
