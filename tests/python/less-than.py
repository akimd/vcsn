#! /usr/bin/env python

import vcsn
from test import *

## ------------- ##
## Expressions.  ##
## ------------- ##

def check_lt(r1, r2):
    e1 = ctx.expression(r1)
    e2 = ctx.expression(r2)
    CHECK_EQ(True, e1 < e2)
    CHECK_EQ(False, e2 < e1)
    CHECK_EQ(e1, e1)
    CHECK_EQ(e2, e2)
    CHECK_NE(e1, e2)
    CHECK_NE(e2, e1)

ctx = vcsn.context('lal_char, q')
check_lt(r'\z', r'\e')
check_lt(r'\z', 'a')
check_lt(r'\e', 'a')
check_lt('a', 'b')
check_lt('a', 'a+b')

check_lt('<1/10>a', '<1/5>a')
check_lt('<-10>a', '<-5>a')
check_lt('<-10>a', '<10>a')

check_lt('a+b', 'a+b+c')
check_lt('a+b', 'ab')
check_lt('ab', 'abc')
check_lt('ab', 'a**')
check_lt('a*', 'a**')
