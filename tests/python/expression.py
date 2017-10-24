#! /usr/bin/env python

# Check parsing and pretty-printing.

import vcsn
from test import *


## --------- ##
## Escapes.  ##
## --------- ##

ctx = vcsn.context("lal_char, b")
def check(n):
    e = r'\x{:02x}'.format(n)
    print(e)
    e = ctx.expression(e)
    CHECK_EQ(e, ctx.expression(str(e)))

# We use 0 for \e and -1 for $.
for i in range(1, 255):
    check(i)

XFAIL(lambda: ctx.expression(r'\xff'),
      r'add_letter: the special letter is reserved: \xff')


## ------------- ##
## Copy/convert. ##
## ------------- ##

def check(e, ctx=None, ids=None, exp=None):
    '''When `e` is converted to `ctx` and `ids`, it should be `exp`.
    `exp` defaults to `e`.'''
    if ctx is None:
        ctx = e.context()
    if ids is None:
        ids = e.identities()
    if exp is None:
        exp = e
    if not isinstance(ctx, vcsn.context):
        ctx = vcsn.context(ctx)
    CHECK_EQ(exp, e.expression(ctx, ids))

q = vcsn.context('lan, q')
qexp = q.expression

e = qexp('a+b+<1/2>a', 'none')
check(e, q, 'none', e)
check(e, q, 'linear', '<3/2>a+b')
check(e, 'law, q', 'none', '(a+b)+<1/2>a')
check(e, 'lan, r', 'none', '(a+b)+<0.5>a')
check(e, 'law, r', 'linear', '<1.5>a+b')

check(qexp(r'\z*', 'none'), q, 'none', r'\z*')
check(qexp(r'\z*', 'none'), q, 'linear', r'\e')

check(qexp(r'\z'))
check(qexp(r'\e'))

check(qexp(r'ab'))
check(qexp(r'a+b'))
check(qexp(r'a&b'))
check(qexp(r'a:b'))
check(qexp(r'a&:b'))
check(qexp(r'a{\}b'))

check(qexp(r'a*'))
check(qexp(r'a{c}'))
check(qexp(r'a{T}'))
# Doesn't make any sense, but that's not for copy to check that.
check(qexp(r'a@b'))

# FIXME: check the tuple case.

## ------------ ##
## Complement.  ##
## ------------ ##

ctx = vcsn.context("lal_char(abcd), b")

def check_complement(r1):
    '''Check that `complement` on a rational expression corresponds to
    its concrete syntax.'''
    eff = ctx.expression(r1).complement()
    exp = ctx.expression('({}){{c}}'.format(r1))
    CHECK_EQ(exp, eff)

check_complement(r'\z')
check_complement('ab')

## ---------- ##
## Multiply.  ##
## ---------- ##
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
check_concat('a', r'\e')
check_concat('a', r'\z')

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
check_difference('a', r'\e')
check_difference('a', r'\z')

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
check_conj('a', r'\e')
check_conj('a', r'\z')

## ----- ##
## Add.  ##
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
check_sum('a', r'\e')
check_sum('a', r'\z')


## --------------- ##
## Transposition.  ##
## --------------- ##
def check_transposition(r1):
    '''Check that `transposition` on a rational expression corresponds to
    its concrete syntax.'''
    eff = ctx.expression(r1).transposition()
    exp = ctx.expression('({}){{T}}'.format(r1))
    CHECK_EQ(exp, eff)

check_transposition(r'\z')
check_transposition('ab')


## --------------------- ##
## Invalid expressions.  ##
## --------------------- ##

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
xfail(r'\a')

ctx = vcsn.context('lal_char(abc), q')
xfail('(?@lal_char(abc), b)<2>a')
xfail('(?@lal_char(abc), b')
xfail('(?@foobar)foo')
xfail('<2>')
xfail('a<2')


## -------- ##
## format.  ##
## -------- ##

# FIXME: This should be part of check-rat.
def check_format(ctx, r, text, utf8, latex):
    ctx = vcsn.context(ctx)
    e = ctx.expression(r)
    CHECK_EQ(text, e.format('text'))
    CHECK_EQ(utf8, e.format('utf8'))
    CHECK_EQ(latex, e.format('latex'))

check_format('lal_char(abcd), b',
             'abcd',
             'abcd',
             'abcd',
             r'a \, b \, c \, d')

# Check classes.  FIXME: Redundant with check-rat.
check_format('lal_char(abcdef), b',
             '[abcdef]',
             '[^]', '[^]', r'[\hat{}]')
check_format('lal_char(abcdef), b',
             '[abcde]',
             '[^f]', '[^f]', r'[\hat{}f]')
check_format('lal_char(a-z), b',
             '[abcd]',
             '[a-d]', '[a-d]', r'[a\textrm{-}d]')

# Check weights.
check_format('lal_char(abc), expressionset<lal_char(def), expressionset<lal_char(xyz), q>>',
             '<<<42>x>d>a+<<<51>x>d>a+(<<<42>y>e>b)*',
             '<<<93>x>d>a+(<<<42>y>e>b)*',
             '⟨⟨⟨93⟩x⟩d⟩a+(⟨⟨⟨42⟩y⟩e⟩b)*',
             r' \left\langle  \left\langle  \left\langle 93 \right\rangle \,x \right\rangle \,d \right\rangle \,a + \left( \left\langle  \left\langle  \left\langle 42 \right\rangle \,y \right\rangle \,e \right\rangle \,b\right)^{*}')

# Words are in \mathit to get correct inter-letter spacing.
check_format('law(abc), q',
             '(abc)a(bc)',
             '(abc)a(bc)',
             '(abc)a(bc)',
             r'\left(\mathit{abc}\right) \, \mathit{a} \, \left(\mathit{bc}\right)')

# Exponents.
for ctx in ['lal, q', 'lan, q', 'law, q']:
    # In wordset, for LaTeX, we use \mathit.
    A = '\mathit{a}' if ctx == 'law, q' else 'a'
    check_format(ctx,
                 'a{2}',
                 'aa', 'aa', r'a \, a'.replace('a', A))
    check_format(ctx,
                 'a{3}',
                 'aaa', 'a³', r'{a}^{3}'.replace('a', A))
    check_format(ctx,
                 'a{4}',
                 'aaaa', 'a⁴', r'{a}^{4}'.replace('a', A))
    check_format(ctx,
                 'a{5}',
                 'a{5}', 'a⁵', r'{a}^{5}'.replace('a', A))
    check_format(ctx,
                 'a{10}',
                 'a{10}', 'a¹⁰', r'{a}^{10}'.replace('a', A))
    check_format(ctx,
                 'a{100}',
                 'a{100}', 'a¹⁰⁰', r'{a}^{100}'.replace('a', A))
    check_format(ctx,
                 'a{987}',
                 'a{987}', 'a⁹⁸⁷', r'{a}^{987}'.replace('a', A))

# Check that we do support digits as letters.
check_format('lal(0123), b',
             '0123',
             '0123',
             '0123',
             '0 \\, 1 \\, 2 \\, 3')
check_format('lal(0123), q',
             '<0123>0123',
             '<123>(0123)',
             '⟨123⟩(0123)',
             r' \left\langle 123 \right\rangle \,\left(0 \, 1 \, 2 \, 3\right)')

## ------------------------- ##
## Distributive identities.  ##
## ------------------------- ##
def check(s1, exp):
    eff = ctx.expression(s1, 'distributive')
    CHECK_EQ(exp, eff.format('text'))

ctx = vcsn.context('lal_char(abcd), q')
check('a+b', 'a+b')
check('(a+a)*', '(<2>a)*')
check(r'a+\z', 'a')
check('(<5>a)b', '<5>(ab)')
check('(<5>a)(b)(c*)', '<5>(abc*)')
check('a+b(c+<2>d)', 'a+bc+<2>(bd)')
check('a*+b(c+<2>d)', 'a*+bc+<2>(bd)')

ctx = vcsn.context('law_char(abcd), q')
check('(a<5>)b', '<5>(ab)')
check('a+b(c+<2>d)', 'a+bc+<2>(bd)')
check('a*+b(c+<2>d)', 'a*+bc+<2>(bd)')


## ------------- ##
## Dot output.   ##
## ------------- ##

def check(exp, dot, identities=['trivial', 'associative']):
    for ids in identities:
        e = ctx.expression(exp, ids)
        for type in ['logical', 'physical']:
            fname = '{}-{}-{}.gv'.format(dot, ids, type)
            print("Checking dot output:", fname)
            ref = metext(fname)
            CHECK_EQ(ref, e.dot(type == 'physical'))

ctx = vcsn.context('lal, q')
check('(<2>[abc])*a([abc]<3>){3}',
      'de-bruijn')
check('\e{2}+\z{2}+a{2}',
      'labels', ['none'])


ctx = vcsn.context('lat<lan, lan>, q')
check('([ab]{3}|x* + [cd]{2}|y*){2}',
      'lan-lan')
