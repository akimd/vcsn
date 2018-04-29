#! /usr/bin/env python

import vcsn
from itertools import product
from test import *

## ----------------------- ##
## automaton + automaton.  ##
## ----------------------- ##

def std(ctx, exp):
    return vcsn.context(ctx).expression(exp).standard()

ctxbr = vcsn.context('[a] -> expressionset<[uv] -> b>')
ctxz = vcsn.context('[b] -> z')
ctxq = vcsn.context('[c] -> q')
ctxr = vcsn.context('[d] -> r')

ab = std('[ab] -> b', '(a+b)*')
bc = std('[bc] -> b', '(b+c)*')
CHECK_EQ(metext('ab+bc.gv'), ab.add(bc))

CHECK_EQ(metext('a1+b1.gv'), meaut('a1.gv').add(meaut('b1.gv')))

# Check join of contexts.
a = std('[a] -> expressionset<[x] -> b>', '<x>a*')
b = std('[b] -> q', '<1/2>b*')
CHECK_EQ(metext('join.gv'), a.add(b))


# lal x lal + lal x lal = lal x lal.
a = std('[...] x [...] -> b', r"\e|x")
b = std('[...] x [...] -> b', r"a|\e")
c = std('[...] x [...] -> b', r"\e|x+a|\e")
# Not the same states numbers, so not CHECK_EQ.
CHECK_ISOMORPHIC(c, a + b)

# Deterministic add.
def check_det(lhs, rhs):
    print('check_det({}, {})'.format(lhs, rhs))
    a = std('[ab] -> b', lhs)
    b = std('[ab] -> b', rhs)
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
CHECK_EQ(r'⟨2.73333⟩ε+u', br + z + q + r)
