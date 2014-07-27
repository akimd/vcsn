#! /usr/bin/env python

import vcsn
from test import *

## --------- ##
## Weights.  ##
## --------- ##

# check CONTEXT INPUT OUTPUT
# --------------------------
def check(ctx, w, output):
    ctx = vcsn.context(ctx)
    w = ctx.weight(w)
    CHECK_EQ(output, str(w))

check('lal_char(a)_b', '0', '0')
check('lal_char(a)_b', '1', '1')
check('lal_char(a)_z', '123', '123')
check('lal_char(a)_f2', '1', '1')
check('lal_char(x)_ratexpset<lal_char(abc)_z>', 'a*<23>bc', 'a*<23>bc')
check('lal_char(x)_seriesset<lal_char(abc)_z>', 'a*<23>bc', '<23>(a*bc)')


# Check that we don't ignore trailing characters.
XFAIL(lambda: vcsn.context('lal_char(ab)_z').weight('123a*'))

# Check + and *.
c = vcsn.context('lal_char(x)_seriesset<lal_char(abc)_z>')
CHECK_EQ(c.weight('<5>a+<5>b'),
         c.weight('<2>a+<3>b') + c.weight('<3>a+<2>b'))
CHECK_EQ(c.weight('<4>aa+<6>ab+<6>ba+<9>bb'),
         c.weight('<2>a+<3>b') * c.weight('<2>a+<3>b'))
