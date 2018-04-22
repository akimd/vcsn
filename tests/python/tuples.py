#! /usr/bin/env python

import vcsn
from test import *

# FIXME: we should probably merge transducers.py in here.

## ---------- ##
## Contexts.  ##
## ---------- ##

c1 = vcsn.context('[ab]*')
c2 = vcsn.context('[xy] -> q')
c3 = vcsn.context('[abxy]* -> q')
CHECK_EQ(c3, c1.join(c2))
CHECK_EQ(c3, c2.join(c1))


## ------------- ##
## Expressions.  ##
## ------------- ##

def check(ctx, exp):
    '''Check round-tripping a multitape expression.'''
    c = vcsn.context(ctx)
    try:
        r = c.expression(exp)
    except RuntimeError:
        FAIL("error parsing " + exp)
    else:
        CHECK_EQ(exp, r)

ctx1 = '[ab] -> q x RatE[[xyz]] x z'
check(ctx1, 'a')
check(ctx1, '<2/3,x,-3>a')
check(ctx1, '<2/3,x+y*,-3>a')
# Yes, once we worked properly with 3 tapes, but not 4.  So check 5.
check('[...] x [...] x [...] x [...] x [...] -> q',
      '1|2|3|4|5')
check('[ab] -> RatE[[xyz] -> q x q] x (q x q)',
      '<<(1,2)>x+<(2,1/3)>y*,(2,3)>a')


## ------------------ ##
## tuple(automaton).  ##
## ------------------ ##

ind = lambda e: vcsn.context('[...] -> Q').expression(e).inductive()
a1 = ind('<2>[ab]')
a2 = ind('x')
CHECK_EQ('''context = [ab]? × [x]? → ℚ
$ -> 0
0 -> 1 <2>a|x
0 -> 2 <2>b|x
1 -> $
2 -> $''', (a1 | a2).format('daut'))


## ------------------- ##
## tuple(expression).  ##
## ------------------- ##

# Regression: Make sure the expressions of one-tuple contexts are
# indeed one-tuple expressions.
c = vcsn.context('lat<lal(abc)>, b')
e = c.expression('[^]')
CHECK_EQ(c, e.context())

# FIXME: this shows that we really need to visit (in the sense of
# visitors) the context to parse it properly.
#
# c = vcsn.context('lat<lat<lal(abc)>>, b')
# e = c.expression('[^]')
# CHECK_EQ(c, e.context())
