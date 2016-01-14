#! /usr/bin/env python

import vcsn
from test import *

def check(r1, r2):
    e1 = ctx.expression(r1)
    e2 = ctx.expression(r2)
    CHECK(e1 < e2)

ctx = vcsn.context('lal_char, q')
check('<1/10>a', '<1/5>a')
check('<-10>a', '<-5>a')
check('<-10>a', '<10>a')
