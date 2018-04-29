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

check('[a] -> b', '0', '0')
check('[a] -> b', '1', '1')
check('[a] -> z', '123', '123')
check('[a] -> f2', '1', '1')
check('[x] -> expressionset<[abc] -> q>', 'a*<23>bc', '⟨23⟩(a*bc)')


# Check that we don't ignore trailing characters.
XFAIL(lambda: vcsn.context('[ab] -> z').weight('123a*'))


c = vcsn.context('[x] -> q')
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

# Check log addition

w = lambda s: vcsn.context('[...] -> log').weight(s)
CHECK_EQ(w('-0.693147'), w('0') + w('0'))
CHECK_EQ(w('1.30685'), w('2') + w('2'))
CHECK_EQ(w('1.95141'), w('2') + w('5'))
CHECK_EQ(w('1.95141'), w('5') + w('2'))
CHECK_EQ(w('2'), w('oo') + w('2'))
CHECK_EQ(w('2'), w('2') + w('oo'))
CHECK_EQ(w('oo'), w('oo') + w('oo'))
CHECK_EQ(w('799.99999'), w('800') + w('850'))
CHECK_EQ(w('799.99999'), w('850') + w('800'))

## ---------------- ##
## Random Weights.  ##
## ---------------- ##

def check(ctx, ref, params):
    CHECK_EQ(ref, ctx.random_weight(params))

ctx = vcsn.context('[abc] -> z')

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
ctx = vcsn.context('[abc] -> b')
for _ in range(3):
    w = ctx.random_weight()
    CHECK(re.match('^[0-1]$', str(w)))
ctx = vcsn.context('[abc] -> q')
check(ctx, '1/3', '1/3=1')
ctx = vcsn.context('[abc] -> r')
CHECK(re.match(r'[0-5].[0-9]*', str(ctx.random_weight('min=0,max=5'))))
