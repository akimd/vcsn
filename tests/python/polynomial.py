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

check('lal_char(a)_b', r'\z', r'\z')
check('lal_char(a)_b', r'\z+\z', r'\z')

check('lal_char(a)_b', r'a', r'a')
check('lal_char(ab)_b', r'a+b+b+a', r'a + b')

check('lal_char(ab)_z', r'a+b+b+a+b', r'<2>a + <3>b')
check('lal_char(ab)_z', r'a+b+b+<-1>a+b', r'<3>b')

check('law_char(a)_b', r'\e+\e', r'\e')

check('law_char(ab)_z', r'ba+ab+bb+aa+a+b+\e+bb+aa',
      r'\e + a + b + <2>aa + ab + ba + <2>bb')

check('lal_char(abc)_ratexpset<lal_char(xyz)_z>',
      r'a + a + <x>b + <y>b', r'<\e+\e>a + <x+y>b')

# Be sure to have proper ordering on tuples with LAN.
check('lat<lan_char(abc), lan_char(xyz)>_z',
      r'(a,\e) + (\e,x) + (a,\e) + (a, x) + (\e,y)',
      r'(\e,x) + (\e,y) + <2>(a,\e) + (a,x)')

# Check that we don't ignore trailing characters.
XFAIL(lambda: vcsn.context('lal_char(ab)_z').polynomial('<123>a*'))

## ----------------- ##
## LaTeX rendering.  ##
## ----------------- ##

c = vcsn.context("lal_char(abc)_ratexpset<lal_char(xyz)_z>")
CHECK_EQ(r'\langle \varepsilon + \varepsilon\rangle a \oplus \langle x + y\rangle b',
         c.polynomial(r'a + a + <x>b + <y>b').format("latex"))
