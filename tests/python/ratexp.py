#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context("lal_char(abcd)_b")

## ------------ ##
## Complement.  ##
## ------------ ##
def check_complement(r1):
    """Check that `complement` on a rational expression corresponds to
    its concrete syntax."""
    eff = ctx.ratexp(r1).complement()
    exp = ctx.ratexp('({}){{c}}'.format(r1))
    CHECK_EQ(exp, eff)

check_complement('\z')
check_complement('ab')

## -------- ##
## Concat.  ##
## -------- ##
def check_concat(r1, r2):
    """Check that `*` between rational expression corresponds to
    concatenation concrete syntax."""
    eff = ctx.ratexp(r1) * ctx.ratexp(r2)
    exp = ctx.ratexp('({})({})'.format(r1, r2))
    CHECK_EQ(exp, eff)

check_concat('ab', 'cd')
check_concat('a', 'bcd')
check_concat('abab', 'bbbb')
check_concat('a*', 'a*b*')
check_concat('a*+b*+c+c*', '(a*+b*+c+c*)*')
check_concat('(a*+b*+c+c*)*', '(a*a*a*b*b*a+b+a+b+a)')
check_concat('a', '\e')
check_concat('a', '\z')

## ------------ ##
## Difference.  ##
## ------------ ##
def check_difference(r1, r2):
    """Check that `%` between rational expression corresponds to
    `%` in concrete syntax."""
    eff = ctx.ratexp(r1) % ctx.ratexp(r2)
    exp = ctx.ratexp('({})%({})'.format(r1, r2))
    CHECK_EQ(exp, eff)

check_difference('ab', 'cd')
check_difference('a', 'bcd')
check_difference('abab', 'bbbb')
check_difference('a*', 'a*b*')
check_difference('a*+b*+c+c*', '(a*+b*+c+c*)*')
check_difference('(a*+b*+c+c*)*', '(a*a*a*b*b*a+b+a+b+a)')
check_difference('a', '\e')
check_difference('a', '\z')

## ------------- ##
## Conjunction.  ##
## ------------- ##
def check_conj(r1, r2):
    """Check that `&` between rational expression corresponds to
    `&` in concrete syntax."""
    eff = ctx.ratexp(r1) & ctx.ratexp(r2)
    exp = ctx.ratexp('({})&({})'.format(r1, r2))
    CHECK_EQ(exp, eff)

check_conj('ab', 'cd')
check_conj('a', 'bcd')
check_conj('abab', 'bbbb')
check_conj('a*', 'a*b*')
check_conj('a*+b*+c+c*', '(a*+b*+c+c*)*')
check_conj('(a*+b*+c+c*)*', '(a*a*a*b*b*a+b+a+b+a)')
check_conj('a', '\e')
check_conj('a', '\z')

## ----- ##
## Sum.  ##
## ----- ##
def check_sum(r1, r2):
    """Check that `+` between rational expression corresponds to
    `+` in concrete syntax."""
    eff = ctx.ratexp(r1) + ctx.ratexp(r2)
    exp = ctx.ratexp('({})+({})'.format(r1, r2))
    CHECK_EQ(exp, eff)

check_sum('ab', 'cd')
check_sum('a', 'bcd')
check_sum('abab', 'bbbb')
check_sum('a*', 'a*b*')
check_sum('a*+b*+c+c*', '(a*+b*+c+c*)*')
check_sum('(a*+b*+c+c*)*', '(a*a*a*b*b*a+b+a+b+a)')
check_sum('a', '\e')
check_sum('a', '\z')


## --------------- ##
## Transposition.  ##
## --------------- ##
def check_transposition(r1):
    """Check that `transposition` on a rational expression corresponds to
    its concrete syntax."""
    eff = ctx.ratexp(r1).transposition()
    exp = ctx.ratexp('({}){{T}}'.format(r1))
    CHECK_EQ(exp, eff)

check_transposition('\z')
check_transposition('ab')


## ----------------- ##
## Invalid ratexps.  ##
## ----------------- ##

# Check invalid input.
def xfail(r):
    XFAIL(lambda: ctx.ratexp(r))

ctx = vcsn.context('lal_char(abc)_b')
xfail('')
xfail('<2>a')
xfail('x')
xfail('a+')
xfail('a(')
xfail('a[')
xfail('*')
xfail('&a')
xfail('a&')
xfail('\a')

ctx = vcsn.context('lal_char(abc)_z')
xfail('(?@lal_char(abc)_b)<2>a')
xfail('(?@lal_char(abc)_b')
xfail('(?@foobar)foo')
xfail('<2>')
xfail('a<2')


## -------- ##
## format.  ##
## -------- ##

def check_format(ctx, r, text, latex):
    ctx = vcsn.context(ctx)
    CHECK_EQ(text, ctx.ratexp(r).format('text'))
    CHECK_EQ(latex, ctx.ratexp(r).format('latex'))

check_format('lal_char(abcd)_b',
            "abcd",
            "abcd",
            "a \\, b \\, c \\, d")

check_format('lal_char(abc)_ratexpset<lal_char(def)_ratexpset<lal_char(xyz)_z>>',
             "<<<42>x>d>a+<<<51>x>d>a+(<<<42>y>e>b)*",
             "<<<42>x>d>a+<<<51>x>d>a+(<<<42>y>e>b)*",
             r' \langle  \langle  \langle 42 \rangle \,x \rangle \,d \rangle \,a +  \langle  \langle  \langle 51 \rangle \,x \rangle \,d \rangle \,a + \left( \langle  \langle  \langle 42 \rangle \,y \rangle \,e \rangle \,b\right)^{*}')

# Check that we do support digits as letters.
check_format('lal_char(0123)_b',
             "0123",
             "0123",
             "0 \\, 1 \\, 2 \\, 3")
check_format('lal_char(0123)_z',
             "<0123>0123",
             "<123>0123",
             r' \langle 123 \rangle \,0 \, 1 \, 2 \, 3')
