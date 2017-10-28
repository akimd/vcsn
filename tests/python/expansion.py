#! /usr/bin/env python

import vcsn
from test import *


def expr(e, *args):
    if not isinstance(e, vcsn.expression):
        e = ctx.expression(e, *args)
    return e

def check_aut(e):
    ind = e.automaton('inductive')
    exp = e.automaton('expansion')
    CHECK_EQ(ind.is_valid(), exp.is_valid())
    if ind.is_valid():
        CHECK_EQUIV(ind, exp)


## ----- ##
## Add.  ##
## ----- ##
ctx = vcsn.context('lan, q')
def check(r1, r2, exp):
    e1 = expr(r1)
    e2 = expr(r2)
    CHECK_EQ(exp, e1.expansion() + e2.expansion())
    CHECK_EQ(exp, (e1 + e2).expansion())
    check_aut(e1+e2)

check('a', '<-1>a', '<0>')
check('a', 'a', 'a.[<2>\e]')

check('ab', 'cd', 'a.[b] + c.[d]')
check('a', 'bcd', 'a.[\e] + b.[cd]')
check('abab', 'bbbb', 'a.[bab] + b.[bbb]')
check('(<1/2>a)*', '(<1/2>a)*(<1/3>b)*', '<2> + a.[<1/2>(<1/2>a)* + <1/2>(<1/2>a)*(<1/3>b)*] + b.[<1/3>(<1/3>b)*]')
check('a', r'\e', '<1> + a.[\e]')
check('a', r'\z', 'a.[\e]')


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
# Currently, inductive does not support ~ with nullable.
#    check_aut(~e)

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
ctx = vcsn.context('lat<lan, lan>, q')
def check(r1, r2, exp):
    '''Check that `@` between expansions corresponds to the expansion of
    `@` between expressions.'''
    e1 = expr(r1)
    e2 = expr(r2)
    print('check: {} @ {}'.format(e1, e2))
    # Cannot yet require @ support from Python.
    e = e1.compose(e2)
    CHECK_EQ(exp, e1.expansion().compose(e2.expansion()))
    CHECK_EQ(exp, e.expansion())
    check_aut(e)

check('a|a', 'a|a', 'a|a.[\e]')
check('a|b', 'b|c', 'a|c.[\e]')
check('a*|b*', 'b*|c*',
      '<1>'
      ' + \e|\e.[\e|b*@b*|\e]'
      ' + \e|c.[\e@\e|c* + \e|b*@b*|c* + (\e|b)(\e|b*)@\e|c*]'
      ' + a|\e.[a*|\e@\e + a*|b*@b*|\e + a*|\e@(b|\e)(b*|\e)]'
      ' + a|c.[a*|\e@\e|c* + a*|b*@b*|c* + a*|\e@(b|\e)(b*|c*) + (\e|b)(a*|b*)@\e|c*]')
check(r'a|\e', r'\e|b', 'a|b.[\e]')
check(r'(a|\e)(b|c)', 'c|a', 'a|a.[b|\e]')
check('a|b', r'(\e|a)(b|c)', 'a|a.[\e|c]')
check(r'(a|c)+(b|\e)', r'(c|d)(\e|e)', 'a|d.[\e|e] + b|d.[\e@(c|\e)(\e|e)]')
check(r'(a|c)(b|\e)', 'c|d', r'a|d.[b|\e]')



## ------------- ##
## Conjunction.  ##
## ------------- ##
for ctx in [vcsn.context('lal, q'), vcsn.context('lan, q')]:
    def check(r1, r2, exp):
        '''Check that `&` between expansions corresponds to the expansion of
        `&` between expressions.'''
        e1 = expr(r1)
        e2 = expr(r2)
        CHECK_EQ(exp, e1.expansion() & e2.expansion())
        CHECK_EQ(exp, (e1&e2).expansion())
        check_aut(e1&e2)

    check('ab', 'cd', '<0>')
    check('(ab)*', 'a*b*', '<1> + a.[b(ab)*&a*b*]')
    check('(<1/2>a)*', '(<1/2>a)*(<1/3>b)*',
          r'<1> + a.[<1/4>(<1/2>a)*&(<1/2>a)*(<1/3>b)*]')
    check('a', r'\e', '<0>')
    check('a', r'\z', '<0>')

    # Check that ab&ac = 0, not a.[0].
    check('ab', 'ac', '<0>')


## --------- ##
## Division. ##
## --------- ##
ctx = vcsn.context('lan, q')
def check(e1, e2, exp):
    e1 = expr(e1)
    e2 = expr(e2)
    e = e1.ldivide(e2)
    CHECK_EQ(exp, e.expansion())
    CHECK_EQ(exp, e1.expansion().ldivide(e2.expansion()))
    check_aut(e)

check(r'a', r'ab', r'\e.[b]')
check(r'a*{\}a', r'\e', r'\e.[a{\}\e + (a*{\}\e){\}\e]')
check(r'a{\}ab', r'bc', r'\e.[b{\}bc]')


## ----- ##
## Mul.  ##
## ----- ##
CHECK_EQ(r'a.[\e(b(cd))]', ctx.expression('a(b(cd))', 'none').expansion())
CHECK_EQ(r'a.[((\eb)c)d]', ctx.expression('((ab)c)d', 'none').expansion())
CHECK_EQ(r'a.[(\eb)(cd)]', ctx.expression('(ab)(cd)', 'none').expansion())


## ------ ##
## Star.  ##
## ------ ##

XFAIL(lambda: vcsn.Q.expression('a**').expansion(),
      r'''Q: value is not starrable: 1
  while computing expansion of: a**''')



## ------- ##
## Tuple.  ##
## ------- ##
ctx = vcsn.context('lan, q')
def check(r1, r2):
    '''Check that `|` between expansions corresponds to the expansion of
    `|` between expressions.'''
    exp1 = expr(r1)
    exp2 = expr(r2)
    eff = exp1.expansion() | exp2.expansion()
    exp = (exp1 | exp2).expansion()
    CHECK_EQ(exp, eff)
    check_aut(exp1|exp2)

check('ab', 'cd')
check('a', 'bcd')
check('abab', 'bbbb')
check('(<1/2>a)*', '(<1/2>a)*(<1/3>b)*')
check('a', r'\e')
check('a', r'\z')


## -------- ##
## Weight.  ##
## -------- ##
def check(r, w, exp):
    '''Check that `weight * expansion` corresponds to `expansion * weight`
    and to the expansion of `weight * expression`.'''
    e = expr(r, 'trivial')
    leff = e.expansion() * w
    lexp = (e * w).expansion()
    CHECK_EQ(lexp, leff)
    check_aut(e*w)
    reff = w * e.expansion()
    rexp = (w * e).expansion()
    CHECK_EQ(rexp, reff)
    check_aut(w*e)

check('abcd', 2, 'a.[<2>bcd]')
check('a*', 10, '<10> + a.[<10>a*]')
check('[ab]{3}', 4, 'a.[<4>[ab]{2}] + b.[<4>[ab]{2}]')
check('a*+b*+c+c*', 3, 'toto')
check('a', 1, 'a.[\e]')
check('a', 0, '<0>')
