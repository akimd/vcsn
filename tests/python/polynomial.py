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

check('lal_char(a), b', r'\z', r'\z')
check('lal_char(a), b', r'\z+\z', r'\z')

check('lal_char(a), b', r'a', r'a')
check('lal_char(ab), b', r'a+b+b+a', r'a + b')

check('lal_char(abcd), z', r'[abcd]',     r'a + b + c + d')
check('lal_char(abcd), z', r'[dcba]',     r'a + b + c + d')
check('lal_char(abcd), z', r'[abcdabcd]', r'a + b + c + d')
check('lal_char(abcd), z', r'[a-d]',      r'a + b + c + d')
check('lal_char(abcd), z', r'[a-bd]',     r'a + b + d')

check('lal_char(ab), z', r'a+b+b+a+b', r'<2>a + <3>b')
check('lal_char(ab), z', r'a+b+b+<-1>a+b', r'<3>b')

check('law_char(a), b', r'\e+\e', r'\e')

check('law_char(ab), z', r'ba+ab+bb+aa+a+b+\e+bb+aa',
      r'\e + a + b + <2>aa + ab + ba + <2>bb')

check('lal_char(abc), expressionset<lal_char(xyz), q>',
      r'a + a + <x>b + <y>b', r'<<2>\e>a + <x+y>b')

# Be sure to have proper ordering on tuples with LAN.
# For the time being, we support both | and , as separators.
check('lat<lan_char(abc), lan_char(xyz)>, z',
      r'a|\e + \e|x + a|\e + a|x + \e|y',
      r'\e|x + \e|y + <2>a|\e + a|x')
check('lat<lan_char(abc), lan_char(xyz)>, z',
      r'(a,\e) + (\e,x) + (a,\e) + (a, x) + (\e,y)',
      r'\e|x + \e|y + <2>a|\e + a|x')

# Check that we don't ignore trailing characters.
XFAIL(lambda: vcsn.context('lal_char(ab), z').polynomial('<123>a*'))

## ----------------- ##
## LaTeX rendering.  ##
## ----------------- ##

c = vcsn.context("lal_char(abc), expressionset<lal_char(xyz), q>")
CHECK_EQ(r'\left\langle  \left\langle 2 \right\rangle \,\varepsilon\right\rangle a \oplus \left\langle x + y\right\rangle b',
         c.polynomial(r'a + a + <x>b + <y>b').format("latex"))


## ------- ##
## Tuple.  ##
## ------- ##

poly = vcsn.context('lan, q').polynomial
CHECK_EQ('a|x + a|y + b|x + b|y', poly('a+b') | poly('x+y'))
CHECK_EQ('<10>a|x + <14>a|y + <15>b|x + <21>b|y',
         poly('<2>a+<3>b') | poly('<5>x+<7>y'))
