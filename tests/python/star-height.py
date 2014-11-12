#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context("lal_char(abc), expressionset<lal_char(xyz), z>")

def check(expression, height):
    res = ctx.expression(expression).star_height()
    if res == height:
        PASS()
    else:
        FAIL(str(height) + " != " + str(res))

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
