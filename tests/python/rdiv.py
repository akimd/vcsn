#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context('lan_char, q')

def aut(e):
    return ctx.expression(e).automaton()

def load(fname):
    return open(medir + '/' + fname).read().strip()

CHECK_EQ(load('aut1.gv'), aut('a*b*').rdiv(aut('a+b')))

# Addition
CHECK_EQUIV(aut(r'<3>\e'), aut('<6>(a+b)').rdiv(aut('<2>a')))

# Concatenation
CHECK_EQUIV(aut('<3>a'), aut('<6>ab').rdiv(aut('<2>b')))

# Star
CHECK_EQUIV(aut('<3>a*'), aut('<6>a*').rdiv(aut('<2>a')))

# Epsilon cycles
CHECK_EQUIV(aut(r'(<1/2>\e)*a*'), aut('a*').rdiv(aut('(<2>a)*')))

# Empty result
CHECK_EQUIV(aut(r'\z'), aut('<2>a').rdiv(aut('ab')))

# Misc
CHECK_EQUIV(aut('<2>ab*+<3>\e'), aut('<6>ab*').rdiv(aut('<2>a+<3>b')))
CHECK_EQUIV(aut('<2>a'), aut('a(b+c)').rdiv(aut('b+c')))

# Cross check with derived_term

def check(lhs_expr, rhs_expr):
    lhs_aut = lhs_expr.derived_term()
    rhs_aut = rhs_expr.derived_term()

    div_expr = lhs_expr.ldiv(rhs_expr).derived_term()
    div_aut = lhs_aut.ldiv(rhs_aut)

    if div_aut.info()['is valid'] and div_expr.info()['is valid']:
        CHECK_EQUIV(div_expr, div_aut)

exprs = [r'\z', r'<2>\e', '<3>(a+<4>b)*<5>c', '(<6>a*b+<7>ac)*',
        '<8>(a*b+c)*bba+a(b(c+<9>d)*+a)']
exprs = [ctx.expression(e) for e in exprs]
for lhs in exprs:
    for rhs in exprs:
        check(lhs, rhs)
