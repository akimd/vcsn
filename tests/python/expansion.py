#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context('lal_char(abcd), q')

def expr(e, *args):
    if not isinstance(e, vcsn.expression):
        e = ctx.expression(e, *args)
    return e

## ------------ ##
## Complement.  ##
## ------------ ##
def check(r):
    '''Check that `~` on expansions corresponds to the expansion of
    `~` on expressions.'''
    e = expr(r)
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

# Also check with lan.
lan = vcsn.context('lan, b')
check(lan.expression('abc'))
check(lan.expression('\z'))

# And with LAO.
lao = vcsn.context('lao, q')
check(lao.expression('(<1/2>\e)*'))

## ------------- ##
## Conjunction.  ##
## ------------- ##
def check(r1, r2):
    '''Check that `&` between expansions corresponds to the expansion of
    `&` between expressions.'''
    exp1 = expr(r1)
    exp2 = expr(r2)
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
    e = expr(r, 'trivial')
    leff = e.expansion() * w
    lexp = (e * w).expansion()
    CHECK_EQ(lexp, leff)
    reff = w * e.expansion()
    rexp = (w * e).expansion()
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
    exp1 = expr(r1)
    exp2 = expr(r2)
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
ctx = vcsn.context('lan_char(abcd), q')
def check(r1, r2):
    '''Check that `|` between expansions corresponds to the expansion of
    `|` between expressions.'''
    exp1 = expr(r1)
    exp2 = expr(r2)
    eff = exp1.expansion() | exp2.expansion()
    exp = (exp1 | exp2).expansion()
    CHECK_EQ(exp, eff)

check('ab', 'cd')
check('a', 'bcd')
check('abab', 'bbbb')
check('(<1/2>a)*', '(<1/2>a)*(<1/3>b)*')
check('a', '\e')
check('a', '\z')

## --------- ##
## Division. ##
## --------- ##

def check(r1, r2):
    exp1 = expr(r1)
    CHECK_EQ(r2, str(exp1.expansion()))

check(r'a{\}ab', r'\e.[b]')
check(r'(a{\}ab){\}bc', r'\e.[b{\}bc]')
