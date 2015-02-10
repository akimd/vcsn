#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context('lal_char(ab), b')

# check INPUT OUTPUT
# ------------------
def check(re, exp):
    exp = ctx.expression(exp)
    snf = ctx.expression(re).star_normal_form()
    CHECK_EQ(exp, snf)
    CHECK_EQ(exp, exp)
    CHECK_EQUIV(exp, snf)
    # Should be "only" isomorphic, let's try "equal".
    CHECK_EQ(exp.standard(), snf.standard())

# fail INPUT
# ----------
def fail(re):
    re = ctx.expression(re)
    XFAIL(lambda: re.star_normal_form())

check('a', 'a')
check('a*', 'a*')
check('\e*', '\e')
check('(a*b*)*', '(a+b)*')
check('(a*b*)**', '(a+b)*')

# We don't support extended operators.
fail('a{c}')
fail('a*&b*')
fail('a:b')
fail('a{\}b')
fail('a*{T}')
