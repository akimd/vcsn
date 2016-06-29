#! /usr/bin/env python

import vcsn
from itertools import product
from test import *

## ----------------------- ##
## automaton + automaton.  ##
## ----------------------- ##

def std(ctx, exp):
    return vcsn.context(ctx).expression(exp).standard()

ctxbr = vcsn.context('lal_char(a), expressionset<lal_char(uv), b>')
ctxz = vcsn.context('lal_char(b), z')
ctxq = vcsn.context('lal_char(c), q')
ctxr = vcsn.context('lal_char(d), r')

ab = std('lal_char(ab), b', '(a+b)*')
bc = std('lal_char(bc), b', '(b+c)*')
CHECK_EQ(metext('ab+bc.gv'), ab.add(bc))

CHECK_EQ(metext('a1+b1.gv'), meaut('a1.gv').add(meaut('b1.gv')))

# Check join of contexts.
a = std('lal_char(a), expressionset<lal_char(x), b>', '<x>a*')
b = std('lal_char(b), q', '<1/2>b*')
CHECK_EQ(metext('join.gv'), a.add(b))


# lan x lal + lal x lan = lan x lan.
a = std('lat<lan_char, lal_char>, b', r"\e|x")
b = std('lat<lal_char, lan_char>, b', r"a|\e")
c = std('lat<lan_char, lan_char>, b', r"\e|x+a|\e")
# Not the same states numbers, so not CHECK_EQ.
CHECK_ISOMORPHIC(c, a + b)

# Deterministic add.
def check_det(lhs, rhs):
    print('check_det({}, {})'.format(lhs, rhs))
    a = std('lal_char(ab), b', lhs)
    b = std('lal_char(ab), b', rhs)
    res = a.add(b, algo='deterministic')
    CHECK(res.is_deterministic())
    CHECK_EQUIV(a.add(b), res)

exprs = ['a', 'a+b', '(a+b)*', 'ab', 'a*b*']
for lhs, rhs in product(exprs, repeat=2):
    check_det(lhs, rhs)

## ------------------------- ##
## expression + expression.  ##
## ------------------------- ##

br = ctxbr.expression('<u>a')
z = ctxz.expression('<2>b')
q = ctxq.expression('<1/3>c')
r = ctxr.expression('<.4>d')
CHECK_EQ(r'<u>a+<<2>\e>b+<<0.333333>\e>c+<<0.4>\e>d', br + z + q + r)


## ------------------------- ##
## polynomial + polynomial.  ##
## ------------------------- ##

br = ctxbr.polynomial('<u>a')
z = ctxz.polynomial('<2>b')
q = ctxq.polynomial('<1/3>c')
r = ctxr.polynomial('<.4>d')
CHECK_EQ(r'<u>a + <<2>\e>b + <<0.333333>\e>c + <<0.4>\e>d', br + z + q + r)


## ----------------- ##
## weight + weight.  ##
## ----------------- ##

br = ctxbr.weight('u')
z = ctxz.weight('2')
q = ctxq.weight('1/3')
r = ctxr.weight('.4')
CHECK_EQ('2.73333', str(z + q + r))
CHECK_EQ(r'<2.73333>\e+u', br + z + q + r)
