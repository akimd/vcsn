#! /usr/bin/env python

import vcsn
from test import *

def exp(e):
    return e if isinstance(e, vcsn.expression) else vcsn.context(ctx).expression(e)

def aut(e):
    return exp(e).automaton()

def check_ldivide(res, l, r):
    lhs = aut(l)
    rhs = aut(r)
    eff = lhs.ldivide(rhs)
    # Make sure that we did not modify the rhs.  This did happen with
    # ldivide.
    CHECK_EQ(lhs, aut(l))
    CHECK_EQ(rhs, aut(r))
    CHECK_EQ(aut(res).is_valid(), eff.is_valid())
    if eff.is_valid():
        CHECK_EQUIV(aut(res), eff)
    else:
        SKIP('invalid expression', res)
    return eff



# Weighted.
ctx = 'lan_char, q'
# Addition
check_ldivide(r'<3>\e', '<2>a', '<6>(a+b)')
# Concatenation
check_ldivide('<3>b', '<2>a', '<6>ab')
# Star
check_ldivide('<3>a*', '<2>a', '<6>a*')
# Epsilon cycles
check_ldivide(r'(<1/2>\e)*a*', '(<2>a)*', 'a*')
# Empty result
check_ldivide(r'\z', 'ab', '<2>a')
# Misc
check_ldivide(r'<3>a*b+<2>\e', '<2>a+<3>b', '<6>a*b')
check_ldivide('<2>c', 'a+b', '(a+b)c')

CHECK_EQ(metext('aut1.gv'), aut('a+b').ldivide(aut('a*b*')))


# Cross check with derived_term and inductive,standard.
def check(l, r):
    print("Checking: {}, {}".format(l, r))

    divide_exp = l.ldivide(r)
    divide_aut = check_ldivide(divide_exp, l, r)

    if divide_aut.is_valid():
        for algo in ['expansion', 'inductive,standard']:
            a = divide_exp.automaton(algo)
            CHECK_EQUIV(divide_aut, a)
    else:
        SKIP('invalid expression', divide_exp)

exprs = [r'\z', r'<2>\e', '<3>(a+<4>b)*<5>c', '(<6>a*b+<7>ac)*',
        '<8>(a*b+c)*bba+a(b(c+<9>d)*+a)']
exprs = [exp(e) for e in exprs]
for lhs in exprs:
    for rhs in exprs:
        check(lhs, rhs)
