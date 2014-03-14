#! /usr/bin/env python

import vcsn
from test import *

## ------------- ##
## Polynomials.  ##
## ------------- ##

# check_poly CONTEXT INPUT OUTPUT
# -------------------------------
# Check -C and (?@...) support.
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

# Check that we don't ignore trailing characters.
# run 1 r'' -vcsn cat -C 'lal_char(ab)_z' -We '<123>a*'
