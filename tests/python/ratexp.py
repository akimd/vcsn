#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context("lal_char(abcd)_b")

def check_sum(r1, r2):
    eff = ctx.ratexp(r1) + ctx.ratexp(r2)
    exp = ctx.ratexp('({})+({})'.format(r1, r2))
    if eff == exp:
        PASS()
    else:
        FAIL(str(exp) + " != " + str(eff))

check_sum('ab', 'cd')
check_sum('a', 'bcd')
check_sum('abab', 'bbbb')
check_sum('a*', 'a*b*')
check_sum('a*+b*+c+c*', '(a*+b*+c+c*)*')
check_sum('(a*+b*+c+c*)*', '(a*a*a*b*b*a+b+a+b+a)')
check_sum('a', '\e')
check_sum('a', '\z')

PLAN()
