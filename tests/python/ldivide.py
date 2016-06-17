#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context('lan_char, q')

def aut(e):
    return ctx.expression(e).automaton()

CHECK_EQ(metext('aut1.gv'), aut('a+b').ldivide(aut('a*b*')))

# Addition
CHECK_EQUIV(aut(r'<3>\e'), aut('<2>a').ldivide(aut('<6>(a+b)')))

# Concatenation
CHECK_EQUIV(aut('<3>b'), aut('<2>a').ldivide(aut('<6>ab')))

# Star
CHECK_EQUIV(aut('<3>a*'), aut('<2>a').ldivide(aut('<6>a*')))

# Epsilon cycles
CHECK_EQUIV(aut(r'(<1/2>\e)*a*'), aut('(<2>a)*').ldivide(aut('a*')))

# Empty result
CHECK_EQUIV(aut(r'\z'), aut('ab').ldivide(aut('<2>a')))

# Misc
CHECK_EQUIV(aut(r'<3>a*b+<2>\e'), aut('<2>a+<3>b').ldivide(aut('<6>a*b')))
CHECK_EQUIV(aut('<2>c'), aut('a+b').ldivide(aut('(a+b)c')))

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
exprs = [ctx.expression(e) for e in exprs]
for lhs in exprs:
    for rhs in exprs:
        check(lhs, rhs)
