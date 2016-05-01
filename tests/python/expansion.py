#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context("lal_char(abcd), q")

## ------------ ##
## Complement.  ##
## ------------ ##
def check(r):
    '''Check that `~` on expansions corresponds to the expansion of
    `~` on expressions.'''
    e = ctx.expression(r)
    eff = ~(e.expansion())
    exp = (~e).expansion()
    CHECK_EQ(exp, eff)

check('\z')
check('\e')
check('<1/2>\e')
check('a')
check('(<1/2>a*+<1/3>b*)*')
check('ab')
check('abab')
check('a*')
check('(<1/2>a)*')


## ------------- ##
## Conjunction.  ##
## ------------- ##
def check(r1, r2):
    '''Check that `&` between expansions corresponds to the expansion of
    `&` between expressions.'''
    exp1 = ctx.expression(r1)
    exp2 = ctx.expression(r2)
    eff = exp1.expansion() & exp2.expansion()
    exp = (exp1 & exp2).expansion()
    CHECK_EQ(exp, eff)

check('ab', 'cd')
check('(ab)*', 'a*b*')
check('(<1/2>a)*', '(<1/2>a)*(<1/3>b)*')
check('a', '\e')
check('a', '\z')


## ---------- ##
## Multiply.  ##
## ---------- ##
def check(r, w):
    '''Check that `weight * expansion` corresponds to `expansion * weight`
    and to the expansion of `weight * expression` '''
    exp = ctx.expression(r, 'trivial')
    leff = exp.expansion() * w
    lexp = (exp * w).expansion()
    CHECK_EQ(lexp, leff)
    reff = w * exp.expansion()
    rexp = (w * exp).expansion()
    CHECK_EQ(rexp, reff)

check('abab', 2)
check('a*', 10)
check('[ab]{3}', 4)
check('a*+b*+c+c*', 3)
check('a', 1)


## ----- ##
## Sum.  ##
## ----- ##
def check(r1, r2):
    '''Check that `+` between expansions corresponds to the expansion of
    `+` between expressions.'''
    exp1 = ctx.expression(r1)
    exp2 = ctx.expression(r2)
    eff = exp1.expansion() + exp2.expansion()
    exp = (exp1 + exp2).expansion()
    CHECK_EQ(exp, eff)

check('ab', 'cd')
check('a', 'bcd')
check('abab', 'bbbb')
check('(<1/2>a)*', '(<1/2>a)*(<1/3>b)*')
check('a', '\e')
check('a', '\z')


## ------- ##
## Tuple.  ##
## ------- ##
ctx = vcsn.context("lan_char(abcd), q")
def check(r1, r2):
    '''Check that `|` between expansions corresponds to the expansion of
    `|` between expressions.'''
    exp1 = ctx.expression(r1)
    exp2 = ctx.expression(r2)
    eff = exp1.expansion() | exp2.expansion()
    exp = (exp1 | exp2).expansion()
    CHECK_EQ(exp, eff)

check('ab', 'cd')
check('a', 'bcd')
check('abab', 'bbbb')
check('(<1/2>a)*', '(<1/2>a)*(<1/3>b)*')
check('a', '\e')
check('a', '\z')
