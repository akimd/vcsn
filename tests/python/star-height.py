#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context("lal_char(abc)_ratexpset<lal_char(xyz)_z>")

def check(ratexp, height):
    res = ctx.ratexp(ratexp).star_height()
    if res == height:
        PASS()
    else:
        FAIL(str(height) + " != " + str(res))

check('\e', 0)
check('a', 0)
check('<x>a', 0)
check('\e*', 1)
check('a*', 1)
check('<x>a*', 1)
check('a*+b*+c+c*', 1)
check('(a*+b*+c+c*)*', 2)
check('(<x>a*+<y*>b*+c+c*)*', 2)

PLAN()
