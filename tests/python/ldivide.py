#! /usr/bin/env python

import vcsn
from test import *

ctx = 'lan_char, q'
def exp(e):
    return vcsn.context(ctx).expression(e)

def aut(e):
    return exp(e).automaton()

def check(res, l, r):
    CHECK_EQUIV(aut(res), aut(l).ldivide(aut(r)))

# Addition
check(r'<3>\e', '<2>a', '<6>(a+b)')
# Concatenation
check('<3>b', '<2>a', '<6>ab')
# Star
check('<3>a*', '<2>a', '<6>a*')
# Epsilon cycles
check(r'(<1/2>\e)*a*', '(<2>a)*', 'a*')
# Empty result
check(r'\z', 'ab', '<2>a')
# Misc
check(r'<3>a*b+<2>\e', '<2>a+<3>b', '<6>a*b')
check('<2>c', 'a+b', '(a+b)c')

CHECK_EQ(metext('aut1.gv'), aut('a+b').ldivide(aut('a*b*')))


# Cross check with derived_term and inductive,standard.
def check(lhs_expr, rhs_expr):
    divide_expr = lhs_expr.ldivide(rhs_expr)
    print("Checking:", divide_expr)

    lhs_aut = lhs_expr.automaton()
    rhs_aut = rhs_expr.automaton()
    divide_aut = lhs_aut.ldivide(rhs_aut)

    if divide_aut.is_valid():
        for algo in ['expansion', 'inductive,standard']:
            a = divide_expr.automaton(algo)
            CHECK_EQUIV(divide_aut, a)
    else:
        SKIP('invalid expression', divide_expr)

exprs = [r'\z', r'<2>\e', '<3>(a+<4>b)*<5>c', '(<6>a*b+<7>ac)*',
        '<8>(a*b+c)*bba+a(b(c+<9>d)*+a)']
exprs = [exp(e) for e in exprs]
for lhs in exprs:
    for rhs in exprs:
        check(lhs, rhs)
