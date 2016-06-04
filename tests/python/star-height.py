#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context("lal_char(abc), expressionset<lal_char(xyz), q>")

def check(exp, height):
    CHECK_EQ(height, ctx.expression(exp).star_height())

check('\e', 0)
check('\z', 0)
check('a', 0)
check('<x>a', 0)
check('\e*', 1)
check('a*', 1)
check('<x>a*', 1)
check('a*+b*+c+c*', 1)
check('(a*+b*+c+c*)*', 2)
check('(<x>a*+<y*>b*+c+c*)*', 2)
# Coverage:
check('((a{\}a{c}&a*:a)(a*)<x>){T}*', 2)

check('(a*&b*)*', 2)
check('(a*:b*)*', 2)
check('(a*&:b*)*', 2)
check('(a*{c})*', 2)
check('(a*{T})*', 2)
check('(a*{\}b*)*', 2)

ctx = vcsn.context('lat<lan, lan>, q')
check('a*|b', 1)
check('(a|b)*', 1)
check('(a*|b*)*', 2)
check('(a*|b*)* @ b|c', 2)
