#! /usr/bin/env python

import vcsn
from test import *

# Eliminate state 0, or 1, or 2.
a = meaut('lao.gv')
for i in [0, 1, 2]:
    print("Eliminating state", i)
    exp = metext('lao-elim-{}.gv'.format(i))
    CHECK_EQ(exp, a.eliminate_state(i))

XFAIL(lambda: a.eliminate_state(3))

a = vcsn.B.expression('ab*c').standard().lift()
for i in range(4):
    print("elimination:", i)
    exp = metext('abc-elim-{}.gv'.format(i))
    a = a.eliminate_state(-1)
    CHECK_EQ(exp, a)

# Remove states in an empty automaton.
XFAIL(lambda: a.eliminate_state(0))
XFAIL(lambda: a.eliminate_state(-3))
XFAIL(lambda: a.eliminate_state())

# Regression: at some point, because we made no difference between
# special and one in oneset, we could have a transition to post that
# was labeled by one, and another labeled by special.  That's two
# transitions (<\e>\e, and <aa*>$) instead of a unique one
# (<\e+aa*>$), which is visible in the dot output.
a = vcsn.context('lal, b').expression('aa*').standard().lift()
CHECK_EQ(r'''context = lao, expressionset<letterset<char_letters(a)>, b>
$ -> 0
0 -> 1 <a>
1 -> $ <\e+aa*>''',
         a.eliminate_state(2).format('daut'))
