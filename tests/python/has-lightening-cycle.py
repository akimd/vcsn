#! /usr/bin/env python

import vcsn
from test import *

def check(exp, has):
    a = ctx.expression(exp).standard()
    CHECK(a.has_lightening_cycle() == has)

ctx = vcsn.context('lal(a), nmin')
check(r'\e', False)

ctx = vcsn.context('lal(a), zmin')
check('(<-1>a)*', True)
check('a*', False)

ctx = vcsn.context('lal(a), r')
check('(<0.5>a)*', True)
check('(<1.5>a)*', False)
