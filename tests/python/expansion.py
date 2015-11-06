#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context("lal_char(abcd), b")

## ----- ##
## Sum.  ##
## ----- ##
def check_sum(r1, r2):
    '''Check that `+` between expansions corresponds to the expansion of
    `+` between expressions.'''
    exp1 = ctx.expression(r1)
    exp2 = ctx.expression(r2)
    eff = exp1.expansion() + exp2.expansion()
    exp = (exp1 + exp2).expansion()
    CHECK_EQ(exp, eff)

check_sum('ab', 'cd')
check_sum('a', 'bcd')
check_sum('abab', 'bbbb')
check_sum('a*', 'a*b*')
check_sum('a*+b*+c+c*', '(a*+b*+c+c*)*')
check_sum('(a*+b*+c+c*)*', '(a*a*a*b*b*a+b+a+b+a)')
check_sum('a', '\e')
check_sum('a', '\z')

ctx = vcsn.context("lal_char(abcd), q")

## ---------- ##
## Multiply.  ##
## ---------- ##
def check_mult(r, w):
    '''Check that `weight * expansion` corresponds to `expansion * weight`
    and to the expansion of `weight * expression` '''
    exp = ctx.expression(r, 'trivial')
    leff = exp.expansion() * w
    lexp = (exp * w).expansion()
    CHECK_EQ(lexp, leff)
    reff = w * exp.expansion()
    rexp = (w * exp).expansion()
    CHECK_EQ(rexp, reff)

check_mult('abab', 2)
check_mult('a*', 10)
check_mult('[ab]{3}', 4)
check_mult('a*+b*+c+c*', 3)
check_mult('a', 1)
