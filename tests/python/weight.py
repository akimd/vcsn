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


## ---------------- ##
## Random Weights.  ##
## ---------------- ##

def check(ctx, w, params):
    w = ctx.weight(w)
    output = ctx.random_weight(params)
    CHECK_EQ(output, w)

ctx = vcsn.context('lal_char(abc), z')

# Check the property "element=density".
check(ctx, '1', '1=1')
check(ctx, '0', '0=1')
check(ctx, '3563', '3563=1')
w = ctx.random_weight('2=0.5,1=0.5')
CHECK(str(w) in ['1', '2'])

# Check the range.
w = ctx.random_weight('min=0,max=3')
CHECK(re.match('^[0-3]$', str(w)))

# Check some weightSet.
ctx = vcsn.context('lal_char(abc), b')
for _ in range(3):
    w = ctx.random_weight()
    CHECK(re.match('^[0-1]$', str(w)))
ctx = vcsn.context('lal_char(abc), q')
check(ctx, '1/3', '1/3=1')
ctx = vcsn.context('lal_char(abc), r')
CHECK(re.match(r'[0-5].[0-9]*', str(ctx.random_weight('min=0,max=5'))))
