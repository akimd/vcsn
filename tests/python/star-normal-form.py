#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context('lal(ab), b')

# check INPUT [OUTPUT = INPUT]
# ----------------------------
def check(re, exp = None):
    if exp is None:
        exp = re
    exp = ctx.expression(exp)
    re  = ctx.expression(re)
    snf = re.star_normal_form()
    # Check is equal to the expected result.
    CHECK_EQ(exp, snf)
    # Check that is indeed equivalent to the original expression.
    CHECK_EQUIV(re, snf)
    # Check that an expression and its SNF have the same standard.
    # Should be "only" isomorphic, let's try "equal".
    CHECK_EQ(re.standard(), snf.standard())

# fail INPUT
# ----------
def fail(re):
    re = ctx.expression(re)
    XFAIL(lambda: re.star_normal_form())

check(r'\z')
check('a')
check('a*')
check(r'\e*', r'\e')
check('(a*b*)*', '(a+b)*')
check('(a*b*)**', '(a+b)*')
check('(a*b*)**+b**', '(a+b)*+b*')

# Check the dot operator: when one member of a product has a null
# constant term.
check('(ab?)*')

# We don't support extended operators.
fail('a{c}')
fail('a*&b*')
fail('a:b')
fail(r'a{\}b')
fail('a*{T}')

# We don't support weighted expressions.
fail('(?@lal, z)<2>a*')
fail('(?@lal, z)a*<2>')
