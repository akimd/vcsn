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
check('lal_char(x), expressionset<lal_char(abc), q>', 'a*<23>bc', '<23>(a*bc)')


# Check that we don't ignore trailing characters.
XFAIL(lambda: vcsn.context('lal_char(ab), z').weight('123a*'))


c = vcsn.context('lal_char(x), q')
w = lambda s: c.weight(s)
# Check +.
CHECK_EQ(w('5'), w('2') + w('3'))

# Check *.
CHECK_EQ(w('6'), w('2') * w('3'))

# Check **.
CHECK_EQ(w('1'),      w('1/2') ** 0)
CHECK_EQ(w('1/2'),    w('1/2') ** 1)
CHECK_EQ(w('1/4'),    w('1/2') ** 2)
CHECK_EQ(w('1/1024'), w('1/2') ** 10)
CHECK_EQ(w('7/16'),   w('1/2') ** (2, 4))

CHECK_EQ(w('2'),      w('1/2') ** -1)
CHECK_EQ(w('1/2'),    w('1/2') ** (2, -1))
