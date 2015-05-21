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
    CHECK_EQ(output, w)

check('lal_char(a), b', '0', '0')
check('lal_char(a), b', '1', '1')
check('lal_char(a), z', '123', '123')
check('lal_char(a), f2', '1', '1')
check('lal_char(x), expressionset<lal_char(abc), z>', 'a*<23>bc', 'a*<23>bc')
check('lal_char(x), seriesset<lal_char(abc), z>', 'a*<23>bc', '<23>(a*bc)')


# Check that we don't ignore trailing characters.
XFAIL(lambda: vcsn.context('lal_char(ab), z').weight('123a*'))

# Check +.
c = vcsn.context('lal_char(x), seriesset<lal_char(abc), z>')
CHECK_EQ(c.weight('<5>a+<5>b'),
         c.weight('<2>a+<3>b') + c.weight('<3>a+<2>b'))
