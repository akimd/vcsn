#! /usr/bin/env python

import vcsn
from test import *

## -------- ##
## Labels.  ##
## -------- ##

# check CONTEXT INPUT OUTPUT
# --------------------------
def check(ctx, w, output):
    ctx = vcsn.context(ctx)
    w = ctx.label(w)
    CHECK_EQ(output, str(w))

check('lal_char(a)_b', 'a', 'a')
check('law_char(a)_b', 'a', 'a')
check('law_char(ab)_b', 'ababab', 'ababab')


# Check that we don't ignore trailing characters.
XFAIL(lambda: vcsn.context('law_char(ab)_b').label('ab*'))
