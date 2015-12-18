#! /usr/bin/env python

import vcsn
from test import *

def check(exp, has):
    a = ctx.expression(exp).standard()
    CHECK(a.has_lightening_cycle() == has)

ctx = vcsn.context('lan_char(a), nmin')
check('\e', False)

ctx = vcsn.context('lal_char(a), zmin')
check('(<-1>a)*', True)
check('a*', False)

ctx = vcsn.context('lal_char(a), r')
check('(<0.5>a)*', True)
check('(<1.5>a)*', False)
