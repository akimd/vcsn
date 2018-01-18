#! /usr/bin/env python

import vcsn
from test import *

## ------------- ##
## Polynomials.  ##
## ------------- ##

# check CONTEXT INPUT OUTPUT
# --------------------------
def check(ctx, p, output):
    ctx = vcsn.context(ctx)
    p = ctx.polynomial(p)
    CHECK_EQ(output, str(p))

check('lal(a), b', r'\z', r'\z')
check('lal(a), b', r'\z+\z', r'\z')

check('lal(a), b', r'a', r'a')
check('lal(ab), b', r'a+b+b+a', r'a + b')

check('lal(abcd), q', r'[abcd]',     r'a + b + c + d')
check('lal(abcd), q', r'[dcba]',     r'a + b + c + d')
check('lal(abcd), q', r'[abcdabcd]', r'a + b + c + d')
check('lal(abcd), q', r'[a-d]',      r'a + b + c + d')
check('lal(abcd), q', r'[a-bd]',     r'a + b + d')

check('lal(ab), q', r'a+b+b+a+b', r'<2>a + <3>b')
check('lal(ab), q', r'a+b+b+<-1>a+b', r'<3>b')

check('law(a), b', r'\e+\e', r'\e')

check('law(ab), q', r'ba+ab+bb+aa+a+b+\e+bb+aa',
      r'\e + a + b + <2>aa + ab + ba + <2>bb')

check('lal(abc), expressionset<lal(xyz), q>',
      r'a + a + <x>b + <y>b', r'<<2>\e>a + <x+y>b')

# Be sure to have proper ordering on tuples with LAN.
# For the time being, we support both | and , as separators.
check('lat<lal(abc), lal(xyz)>, q',
      r'a|\e + \e|x + a|\e + a|x + \e|y',
      r'\e|x + \e|y + <2>a|\e + a|x')
check('lat<lal(abc), lal(xyz)>, q',
      r'(a,\e) + (\e,x) + (a,\e) + (a, x) + (\e,y)',
      r'\e|x + \e|y + <2>a|\e + a|x')

# Check that we don't ignore trailing characters.
XFAIL(lambda: vcsn.context('lal(ab), q').polynomial('<123>a*'))

## ----------------- ##
## LaTeX rendering.  ##
## ----------------- ##

c = vcsn.context("lal(abc), expressionset<lal(xyz), q>")
CHECK_EQ(r'\left\langle  \left\langle 2 \right\rangle \,\varepsilon\right\rangle a \oplus \left\langle x + y\right\rangle b',
         c.polynomial(r'a + a + <x>b + <y>b').format("latex"))


## ----- ##
## Add.  ##
## ----- ##

poly = vcsn.context('lal, q').polynomial
CHECK_EQ('a + b + x + y', poly('a+b') + poly('x+y'))
CHECK_EQ('<7>a + <7>c',
         poly('<2>a+<3>b') + poly('<5>a + <-3>b + <7>c'))


## --------- ##
## Weights.  ##
## --------- ##

poly = vcsn.context('law, q').polynomial
# Left.
CHECK_EQ('<2>a + <2>b', 2 * poly('a+b'))
CHECK_EQ('a + b',       1 * poly('a+b'))
CHECK_EQ(r'\z',         0 * poly('a+b'))

# Right.
#
# Here the result must be different when the labels support weights,
# e.g., polynomials of expressions.  This is checked elsewhere, when
# dealing with expansions of expressions.
CHECK_EQ('<2>a + <2>b', poly('a+b') * 2)
CHECK_EQ('a + b',       poly('a+b') * 1)
CHECK_EQ(r'\z',         poly('a+b') * 0)


## ----- ##
## Mul.  ##
## ----- ##

poly = vcsn.context('law, q').polynomial
CHECK_EQ('ax + ay + bx + by', poly('a+b') * poly('x+y'))
CHECK_EQ('<10>ax + <14>ay + <15>bx + <21>by',
         poly('<2>a+<3>b') * poly('<5>x+<7>y'))
CHECK_EQ('<20>a + <30>b',
         poly('<2>a+<3>b') * 10)
CHECK_EQ('<20>a + <30>b',
         10 * poly('<2>a+<3>b'))


## ----- ##
## Pow.  ##
## ----- ##

poly = vcsn.context('law, q').polynomial
CHECK_EQ(r'\e',                  poly('a') ** 0)
CHECK_EQ('<4>a + <12>b + <-3>c', poly('<4>a + <12>b + <-3>c') ** 1)
CHECK_EQ('aaa + aab + aba + abb + baa + bab + bba + bbb', poly('a + b') ** 3)



## ------------- ##
## Conjunction.  ##
## ------------- ##

poly = vcsn.context('lal, q').polynomial
CHECK_EQ(r'\z', poly('a+b') & poly('x+y'))
CHECK_EQ('<10>a',
         poly('<2>a+<3>b') & poly('<5>a+<7>c'))


## ------- ##
## Tuple.  ##
## ------- ##

poly = vcsn.context('lal, q').polynomial
CHECK_EQ('a|x + a|y + b|x + b|y', poly('a+b') | poly('x+y'))
CHECK_EQ('<10>a|x + <14>a|y + <15>b|x + <21>b|y',
         poly('<2>a+<3>b') | poly('<5>x+<7>y'))


## --------- ##
## Compose.  ##
## --------- ##

poly = vcsn.context('lat<lal, lal>, q').polynomial
CHECK_EQ('a|A + b|B', poly('a|x + b|y').compose(poly('x|A + y|B')))
CHECK_EQ('<10>a|A + <21>b|B',
         poly('<2>a|x + <3>b|y + c|z').compose(poly('<5>x|A + <7>y|B + c|C')))
