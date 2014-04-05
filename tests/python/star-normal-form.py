#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context('lal_char(ab)_b')

# check INPUT OUTPUT
# ------------------
def check(re, exp):
    exp = ctx.ratexp(exp)
    snf = ctx.ratexp(re).star_normal_form()
    CHECK_EQ(exp, snf)
    CHECK_EQ(exp, exp)

# fail INPUT
# ----------
def fail(re):
    re = ctx.ratexp(re)
    XFAIL(lambda: re.star_normal_form())
    # If it does not work, it's because anyway the ratexp is invalid.
    CHECK_EQ(False, re.is_valid())

check('a', 'a')
check('a*', 'a*')
check('\e*', '\e')
check('(a*b*)*', '(a+b)*')
check('(a*b*)**', '(a+b)*')

# We don't support conjunction.
XFAIL(lambda: ctx.ratexp("a*&b*").star_normal_form())

## FIXME: This is crap, it does not work when there are weights!!!

ctx = vcsn.context('lal_char(ab)_z')
fail('\e*')
fail('a**')
check('(a+b)*', '(a+b)*')

ctx = vcsn.context('lal_char(ab)_q')
fail('\e*')
check('(<1/2>\e+a)*', '<2>a*')
fail('(<1/2>\e+a)**')
check('(<1/6>a*<1/3>b*)*', '<18/17>(<1/6>a+<1/3>b)*')
check('(<1/6>a*+<1/3>b*)*', '<2>(<1/6>a+<1/3>b)*')

ctx = vcsn.context('lal_char(ab)_ratexpset<lal_char(wxyz)_z>')
check('<w>(<x>\e+<y>a)*<z>', '<wx*>(<y>a)*<z>')
check('<w>(<x>\e+<y>a)*<z>*', '<(wx*z)*>(<wyz>a)*')
