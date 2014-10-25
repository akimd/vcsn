#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context('lal_char(ab), ratexpset<lal_char(xyz), b>')

## ---------- ##
## automata.  ##
## ---------- ##

# Standard automata.
q = vcsn.context('lal_char(ab), q')
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
                 '3/4' * r.standard())

CHECK_ISOMORPHIC((r * '3/4').standard(),
                 r.standard() * '3/4')

# Non-standard automata.  This time, it does not commute.
a = q.ratexp('ab').derived_term() | q.ratexp('ab').derived_term()
CHECK_EQUIV(q.ratexp('<3/4>ab+<3/4>ab').derived_term(),
            '3/4' * a)
CHECK_EQUIV(q.ratexp('(ab)<3/4>+(ab)<3/4>').derived_term(),
            a * '3/4')

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
