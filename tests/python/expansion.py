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

# Also check with lan.
lan = vcsn.context('lan, b')
check(lan.expression('abc'))
check(lan.expression(r'\z'))

# And with LAO.
lao = vcsn.context('lao, q')
check(lao.expression(r'(<1/2>\e)*'))




## --------- ##
## Compose.  ##
## --------- ##
ctx = vcsn.context('lat<lan(abcde), lan(abcde)>, q')
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
def check(r1, r2, exp):
    '''Check that `&` between expansions corresponds to the expansion of
    `&` between expressions.'''
    exp1 = expr(r1)
    exp2 = expr(r2)
    CHECK_EQ(exp, exp1.expansion() & exp2.expansion())
    CHECK_EQ(exp, (exp1 & exp2).expansion())

check('ab', 'cd', r'<0>')
check('ab', 'ac', r'a.[\z]') # Wrong, but will be fixed.
check('(ab)*', 'a*b*', r'<1> + a.[b(ab)*&a*b*]')
check('(<1/2>a)*', '(<1/2>a)*(<1/3>b)*', r'<1> + a.[<1/4>(<1/2>a)*&(<1/2>a)*(<1/3>b)*]')
check('a', r'\e', r'<0>')
check('a', r'\z', r'<0>')



## --------- ##
## Division. ##
## --------- ##
ctx = vcsn.context('lan_char(abcd), q')
def check(r1, r2):
    exp1 = expr(r1)
    CHECK_EQ(r2, str(exp1.expansion()))

check(r'a{\}ab', r'\e.[b]')
check(r'(a*{\}a){\}\e', r'\e.[a{\}\e + (a*{\}\e){\}\e]')
check(r'(a{\}ab){\}bc', r'\e.[b{\}bc]')


## ----- ##
## Mul.  ##
## ----- ##
CHECK_EQ(r'a.[\e(b(cd))]', ctx.expression('a(b(cd))', 'none').expansion())
CHECK_EQ(r'a.[((\eb)c)d]', ctx.expression('((ab)c)d', 'none').expansion())
CHECK_EQ(r'a.[(\eb)(cd)]', ctx.expression('(ab)(cd)', 'none').expansion())

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
check('a', r'\e')
check('a', r'\z')


## -------- ##
## Weight.  ##
## -------- ##
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
