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
check('lal_char(x)_ratexpset<lal_char(abc)_z>', 'a*<23>bc', 'a*<23>(bc)')

# Check that we don't ignore trailing characters.
XFAIL(lambda: vcsn.context('lal_char(ab)_z').weight('123a*'))
