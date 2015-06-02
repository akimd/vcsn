#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context("lal_char(abcd), b")

## ------------ ##
## Complement.  ##
## ------------ ##
def check_complement(r1):
    '''Check that `complement` on a rational expression corresponds to
    its concrete syntax.'''
    eff = ctx.expression(r1).complement()
    exp = ctx.expression('({}){{c}}'.format(r1))
    CHECK_EQ(exp, eff)

check_complement('\z')
check_complement('ab')

## -------- ##
## Concat.  ##
## -------- ##
def check_concat(r1, r2):
    '''Check that `*` between rational expression corresponds to
    concatenation concrete syntax.'''
    eff = ctx.expression(r1) * ctx.expression(r2)
    exp = ctx.expression('({})({})'.format(r1, r2))
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
    '''Check that `%` between rational expression corresponds to
    `%` in concrete syntax.'''
    eff = ctx.expression(r1) % ctx.expression(r2)
    exp = ctx.expression('({})%({})'.format(r1, r2))
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
    '''Check that `&` between rational expression corresponds to
    `&` in concrete syntax.'''
    eff = ctx.expression(r1) & ctx.expression(r2)
    exp = ctx.expression('({})&({})'.format(r1, r2))
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
    '''Check that `+` between rational expression corresponds to
    `+` in concrete syntax.'''
    eff = ctx.expression(r1) + ctx.expression(r2)
    exp = ctx.expression('({})+({})'.format(r1, r2))
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
    '''Check that `transposition` on a rational expression corresponds to
    its concrete syntax.'''
    eff = ctx.expression(r1).transposition()
    exp = ctx.expression('({}){{T}}'.format(r1))
    CHECK_EQ(exp, eff)

check_transposition('\z')
check_transposition('ab')


## ----------------- ##
## Invalid expressions.  ##
## ----------------- ##

# Check invalid input.
def xfail(r):
    XFAIL(lambda: ctx.expression(r))

ctx = vcsn.context('lal_char(abc), b')
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

ctx = vcsn.context('lal_char(abc), z')
xfail('(?@lal_char(abc), b)<2>a')
xfail('(?@lal_char(abc), b')
xfail('(?@foobar)foo')
xfail('<2>')
xfail('a<2')


## -------- ##
## format.  ##
## -------- ##

def check_format(ctx, r, text, latex):
    ctx = vcsn.context(ctx)
    CHECK_EQ(text, ctx.expression(r).format('text'))
    CHECK_EQ(latex, ctx.expression(r).format('latex'))

check_format('lal_char(abcd), b',
             'abcd',
             'abcd',
             'a \\, b \\, c \\, d')

# Check classes.  A bit redundant with check-rat, except that we check
# LaTeX output here.
check_format('lal_char(abcdef), b',
             '[abcdef]',
             '[^]', r'[\hat{}]')
check_format('lal_char(abcdef), b',
             '[abcde]',
             '[^f]', r'[\hat{}f]')
check_format('lal_char(a-z), b',
             '[abcd]',
             '[a-d]', '[a-d]')

check_format('lal_char(abc), expressionset<lal_char(def), expressionset<lal_char(xyz), z>>',
             '<<<42>x>d>a+<<<51>x>d>a+(<<<42>y>e>b)*',
             '<<<42>x>d>a+<<<51>x>d>a+(<<<42>y>e>b)*',
             r' \left\langle  \left\langle  \left\langle 42 \right\rangle \,x \right\rangle \,d \right\rangle \,a +  \left\langle  \left\langle  \left\langle 51 \right\rangle \,x \right\rangle \,d \right\rangle \,a + \left( \left\langle  \left\langle  \left\langle 42 \right\rangle \,y \right\rangle \,e \right\rangle \,b\right)^{*}')

# Words are in \mathit to get correct inter-letter spacing.
check_format('law_char(abc), z',
             '(abc)a(bc)',
             '(abc)a(bc)',
             r'\left(\mathit{abc}\right) \, \mathit{a} \, \left(\mathit{bc}\right)')

# Check that we do support digits as letters.
check_format('lal_char(0123), b',
             '0123',
             '0123',
             '0 \\, 1 \\, 2 \\, 3')
check_format('lal_char(0123), z',
             '<0123>0123',
             '<123>0123',
             r' \left\langle 123 \right\rangle \,0 \, 1 \, 2 \, 3')

## -------- ##
## Series.  ##
## -------- ##
def check(ctx, s1, exp):
    eff = ctx.series(s1)
    CHECK_EQ(exp, eff.format('text'))

ctx = vcsn.context('lal_char(abcd), z')
check(ctx, 'a+b', 'a+b')
check(ctx, '(a+a)*', '(<2>a)*')

check(ctx, 'a+\z', 'a')
check(ctx, '(<5>a)b', '<5>(ab)')
check(ctx, '(<5>a)(b)(c*)', '<5>(abc*)')
check(ctx, 'a+b(c+<2>d)', 'a+bc+<2>(bd)')
check(ctx, 'a*+b(c+<2>d)', 'a*+bc+<2>(bd)')

ctx = vcsn.context('law_char(abcd), z')

check(ctx, '(a<5>)b', '<5>(ab)')
check(ctx, 'a+b(c+<2>d)', 'a+bc+<2>(bd)')
check(ctx, 'a*+b(c+<2>d)', 'a*+bc+<2>(bd)')
