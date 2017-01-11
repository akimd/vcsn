#! /usr/bin/env python

import vcsn
from test import *

lal = vcsn.context('lal_char(abcd), q')

ctx = lal
def expr(e, *args):
    if not isinstance(e, vcsn.expression):
        e = ctx.expression(e, *args)
    return e


## ------ ##
## Star.  ##
## ------ ##

XFAIL(lambda: vcsn.Q.expression('a**').expansion(),
      r'''Q: value is not starrable: 1
  while computing expansion of: a**''')


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

check(r'\z')
check(r'\e')
check(r'<1/2>\e')
check('a')
check('(<1/2>a*+<1/3>b*)*')
check('ab')
check('abab')
check('a*')
check('(<1/2>a)*')

# Also check with lal, b.
lalb = vcsn.context('lal, b')
check(lal.expression('abc'))
check(lal.expression(r'\z'))

# And with LAO.
lao = vcsn.context('lao, q')
check(lao.expression(r'(<1/2>\e)*'))




## --------- ##
## Compose.  ##
## --------- ##
ctx = vcsn.context('lat<lal(abcde), lal(abcde)>, q')
def check(r1, r2):
    '''Check that `@` between expansions corresponds to the expansion of
    `@` between expressions.'''
    exp1 = expr(r1)
    exp2 = expr(r2)
    eff = exp1.expansion().compose(exp2.expansion())
    exp = exp1.compose(exp2).expansion()
    CHECK_EQ(exp, eff)

check('a|a', 'a|a')
check('a|b', 'b|c')
check('a*|b*', 'b*|c*')
check(r'a|\e', r'\e|b')
check(r'(a|\e)(b|c)', 'c|a')
check('a|b', r'(\e|a)(b|c)')
check(r'(a|c) + (b|\e)', r'(c|d)(\e|e)')
CHECK_EQ(r'a|d.[b|\e@\e]', ctx.expression(r'(a|c)(b|\e) @ c|d').expansion())



## ------------- ##
## Conjunction.  ##
## ------------- ##
ctx = lal
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
check('a', r'\e')
check('a', r'\z')



## --------- ##
## Division. ##
## --------- ##
ctx = vcsn.context('lal_char(abcd), q')
def check(r1, r2):
    exp1 = expr(r1)
    CHECK_EQ(r2, str(exp1.expansion()))

check(r'a{\}ab', r'\e.[b]')
check(r'(a*{\}a){\}\e', r'\e.[a{\}\e + (a*{\}\e){\}\e]')
check(r'(a{\}ab){\}bc', r'\e.[b{\}bc]')


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
check('a', r'\e')
check('a', r'\z')


## ------- ##
## Tuple.  ##
## ------- ##
ctx = vcsn.context('lal_char(abcd), q')
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
check('a', r'\e')
check('a', r'\z')
