#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context("lal_char(abc), seriesset<lal_char(xyz), q>")

def check_derived_term(r, exp, algo):
    print('{}: checking: {:u}.derived_term("{}")'.format(here(), r, algo))
    if algo == 'lazy,expansion':
        # When checking the lazy construction, make sure we have a
        # derived_term_automaton, then snapshot it at the beginning
        # (just the initial states), then make sure that when we
        # evaluate the empty word, we have something different.
        #
        # Finally, evaluate it completely (with accessible), and check
        # against the strict computation.
        lazy = r.derived_term(algo)
        CHECK(lazy.type().startswith('derived_term_automaton'))
        # FIXME: we don't support evaluate on non-free.  #101.
        if r.info('tuple') == 0:
            first = str(lazy)
            # Force the evaluation of the empty word to start computing
            # the automaton.
            lazy('')
            second = str(lazy)
            CHECK_NE(second, first)
            lazy.accessible()
            CHECK_NE(lazy, second)
            eff = lazy.strip()
        else:
            lazy.accessible()
            eff = lazy.strip()
    else:
        eff = r.derived_term(algo)
    CHECK_EQ(metext(exp, 'gv'),
             eff)

def check_dt(r, exp):
    'Check derived-term automaton.'
    for algo in ['derivation', 'expansion', 'lazy,expansion']:
        check_derived_term(r, exp, algo)

def check_bdt(r, exp):
    'Check broken derived-term automaton.'
    if not isinstance(r, vcsn.expression):
        r = ctx.expression(r)
    for algo in ['derivation,breaking', 'expansion,breaking']:
        check_derived_term(r, exp, algo)

def check(re, s, exp, breaking=False):
    "Check that ∂/∂s(re) = exp."
    if not isinstance(re, vcsn.expression):
        re = ctx.expression(re)
    eff = re.derivation(s, breaking)
    print("∂/∂{}({:u}) = {:u}".format(s, re, eff))
    CHECK_EQ(exp, eff)
    # Check that the breaking derivation is exactly the breaking of
    # the regular derivation.
    if breaking:
        CHECK_EQ(re.derivation(s, True),
                 re.derivation(s).split())


##########################
## Regular derivation.  ##
##########################

# Check that by default, derivation is non-breaking.  The difference
# is thin: we expect a polynomial with a single expression "a+b", not a
# polynomial of two expressions: "a + b".
CHECK_EQ('a+b',
         ctx.expression('a(a+b)').derivation('a'))

# While at it, check that trivial identities are properly handled.
CHECK_EQ('b+a',
         ctx.expression('a(b+a)', 'trivial').derivation('a'))


## ---------------------------- ##
## Derive wrt a single letter.  ##
## ---------------------------- ##

# Zero, one.
check(   r'\z', 'a', r'\z')
check(   r'\e', 'a', r'\z')
check(r'<x>\e', 'a', r'\z')

# Letters.
check(   'a', 'a', r'\e')
check(   'a', 'b', r'\z')
check('<x>a', 'a', r'<x>\e')
check('<x>a', 'b', r'\z')

# Sum.
check('<x>a+<y>b', 'a', r'<x>\e')
check('<x>a+<y>b', 'b', r'<y>\e')
check('<x>a+<y>a', 'a', r'<x+y>\e')

# Prod.
check('ab', 'a', 'b')
check('ab', 'b', r'\z')
check('(<x>a).(<y>a).(<z>a)', 'a', '<x><y>a<z>a')

# Conjunction.
check('<x>a&<y>a&<z>a', 'a', r'<xyz>\e')
check('<x>a&<y>a&<z>a', 'b', r'\z')

check('(<x>a+<y>b)*&(<z>b+<x>c)*', 'a', r'\z')
check('(<x>a+<y>b)*&(<z>b+<x>c)*', 'b', '<yz>(<x>a+<y>b)*&(<z>b+<x>c)*')
check('(<x>a+<y>b)*&(<z>b+<x>c)*', 'c', r'\z')

# Shuffle.
check('<x>a:<y>a:<z>a', 'a', r'<x>\e:<y>a:<z>a + <y><x>a:\e:<z>a + <z><x>a:<y>a:\e')
check('<x>a:<y>b:<z>c', 'a', r'<x>\e:<y>b:<z>c')
check('<x>a:<y>b:<z>c', 'b', r'<y><x>a:\e:<z>c')
check('<x>a:<y>b:<z>c', 'c', r'<z><x>a:<y>b:\e')

check('(<x>a<y>b)*:(<x>a<x>c)*',
      'a', '<x>(<x>a<y>b)*:<x>c(<x>a<x>c)* + <x><y>b(<x>a<y>b)*:(<x>a<x>c)*')
check('(<x>a<y>b)*:(<x>a<x>c)*', 'b', r'\z')
check('(<x>a<y>b)*:(<x>a<x>c)*', 'c', r'\z')

# Star.
check('a*', 'a', 'a*')
check('a*', 'b', r'\z')
check('(<x>a)*', 'a', '<x>(<x>a)*')
check('(<x>a)*', 'b', r'\z')
check('<x>a*',   'a', '<x>a*')
check('<x>(<y>a)*', 'a', '<xy>(<y>a)*')
check('(<x>a)*<y>', 'a', '<x>(<x>a)*<y>')
check('(<x>a)*<y>', 'aa', '<xx>(<x>a)*<y>')

XFAIL(lambda: vcsn.Q.expression('a**').derivation('a'),
      r'''Q: value is not starrable: 1
  while computing derivative of: a**
                with respect to: a''')

XFAIL(lambda: vcsn.Q.expression('a**').derived_term(),
      r'''Q: value is not starrable: 1
  while computing expansion of: a**
  while computing derived-term of: a**''')

XFAIL(lambda: vcsn.Q.expression('a**').derived_term('derivation'),
      r'''Q: value is not starrable: 1
  while computing constant-term of: a**
  while computing derived-term of: a**''')


# Complement.
check(r'\z{c}', 'a', r'\z{c}')
check(r'\e{c}', 'a', r'\z{c}')

check('a{c}', 'a', r'\e{c}')
check('a{c}', 'b', r'\z{c}')

check('(a+b){c}', 'a', r'\e{c}')
check('(a+b){c}', 'c', r'\z{c}')

check('(a.b){c}', 'a', 'b{c}')
check('(a.b){c}', 'b', r'\z{c}')

check('(a:b){c}', 'a', r'(\e:b){c}')
check('(a:b){c}', 'b', r'(a:\e){c}')
check('(a:b){c}', 'c', r'\z{c}')

check('(a*&a*){c}', 'a', '(a*&a*){c}')
check('(a*&a*){c}', 'b', r'\z{c}')

check('(<x>(<y>a)*<z>){c}', 'a', '(<y>a)*{c}')
check('(<x>(<y>a)*<z>){c}', 'b', r'\z{c}')

check('a{c}{c}', 'a', r'\e{c}{c}')
check('a{c}{c}', 'b', r'\z{c}{c}')

# We could easily generate an infinite derived-term automaton here.
# However, currently we don't support a powerful enough normalization
# of polynomials of expressions, so use a simple type of weights.
e = vcsn.context('lal_char(a), q').expression('((<2>a)*+(<4>aa)*){c}')
check(e, 'a',    '((<2>a)*+<2>(a(<4>(aa))*)){c}')
check(e, 'aa',   '((<2>a)*+(<4>(aa))*){c}')
check(e, 'aaa',  '((<2>a)*+<2>(a(<4>(aa))*)){c}')
check(e, 'aaaa', '((<2>a)*+(<4>(aa))*){c}')


## ------------------------------- ##
## Derive wrt to several letters.  ##
## ------------------------------- ##

check('(<x>a)*', 'aa',   '<xx>(<x>a)*')
check('(<x>a)*', 'aaaa', '<xxxx>(<x>a)*')
check('(<x>a)*', 'aaab', r'\z')

check('(<x>a)*(<y>b)*', 'aa',   '<xx>(<x>a)*(<y>b)*')
check('(<x>a)*(<y>b)*', 'aabb', '<xxyy>(<y>b)*')


## -------------------- ##
## Classical examples.  ##
## -------------------- ##

# EAT, Example 4.3.
E1 = '(<1/6>a*+<1/3>b*)*'
# E1 typed.
E1t = vcsn.context('lal_char(ab), q').expression(E1)
check(E1t,  'a',  "<1/3>a*" + E1)
check(E1t,  'b',  "<2/3>b*" + E1)
check(E1t, 'aa',  "<4/9>a*" + E1)
check(E1t, 'ab',  "<2/9>b*" + E1)
check(E1t, 'ba',  "<2/9>a*" + E1)
check(E1t, 'bb', "<10/9>b*" + E1)

check_dt(E1t, 'e1-dt')


###########################
## Breaking derivation.  ##
###########################

def check_br(re, letter, exp):
    check(re, letter, exp, True)


## ---------------------------- ##
## Derive wrt a single letter.  ##
## ---------------------------- ##

# Zero, one.
check_br(   r'\z', 'a', r'\z')
check_br(   r'\e', 'a', r'\z')
check_br(r'<x>\e', 'a', r'\z')

# Letters.
check_br(   'a', 'a', r'\e')
check_br(   'a', 'b', r'\z')
check_br('<x>a', 'a', r'<x>\e')
check_br('<x>a', 'b', r'\z')

# Sum.
check_br('<x>a+<y>b', 'a', r'<x>\e')
check_br('<x>a+<y>b', 'b', r'<y>\e')
check_br('<x>a+<y>a', 'a', r'<x+y>\e')

# Prod.
check_br('ab', 'a', 'b')
check_br('ab', 'b', r'\z')
check_br('(<x>a)(<y>a)(<z>a)', 'a', '<xy>a<z>a')

# Exterior products.
check_br('(<x>ab)<y>', 'a', '<xy>b')

# Star.
check_br('a*', 'a', 'a*')
check_br('a*', 'b', r'\z')
check_br('(<x>a)*', 'a', '<x>(<x>a)*')
check_br('(<x>a)*', 'b', r'\z')
check_br('<x>a*',   'a', '<x>a*')
check_br('<x>(<y>a)*', 'a', '<xy>(<y>a)*')

# Check the right-exterior product.
check_bdt('<x>(<y>a)*<z>', 'ext-prod-breaking')

## --------------------- ##
## Documented examples.  ##
## --------------------- ##

# On The Number Of Broken Derived Terms Of A Rational Expression.
ctx = vcsn.context('lal_char(ab), b')
F2 = 'a*+b*'
E2 = "({F2})(a({F2}))".format(F2=F2)
E2t = ctx.expression(E2)
check(E2t, 'a', "{F2} + a*a({F2})".format(F2=F2))
check(E2t, 'b', "b*a({})".format(F2))

# Example 2.
check(E2t, 'aa', "a* + a*+b* + a*a({})".format(F2))
check(E2t, 'ab', 'b*')
check(E2t, 'ba', F2)
check(E2t, 'bb', "b*a({})".format(F2))

# Example 3.
check_dt(E2t, 'e2-dt')

# Example 5.
check_br(E2t,  'a', "a* + b* + a*a({})".format(F2))
check_br(E2t,  'b', "b*a({})".format(F2))
check_br(E2t, 'aa', "a* + b* + a*a({})".format(F2))
check_br(E2t, 'ab', 'b*')
check_br(E2t, 'ba', "a* + b*")
check_br(E2t, 'bb', "b*a({})".format(F2))

# Example 6.
check_bdt(E2t, 'e2-dt-breaking')

# Figure 3.
fig3 = vcsn.context('lal_char(abcd), b').expression('a(b+c+d)')
check_dt(fig3, 'h3-dt')
check_bdt(fig3, 'h3-dt-breaking')


## -------- ##
## Tuples.  ##
## -------- ##

ctx = vcsn.context('lat<lal<char>, lal<char>>, q')
check('ab|xy', 'a|x', 'b|y')

ctx = vcsn.context('lat<lal<char>, lal<char>>, q')
check('ab|xy', 'a|x', 'b|y')
check('ab|xy', r'a|\e', r'\z')
check('a*|x*', 'a|x', 'a*|x*')

exp = ctx.expression
e = exp('(a{+}|x + b{+}|y)*')
f = exp(r'a*|\e') * e
g = exp(r'b*|\e') * e
z = exp(r'\z|\z')
check_dt(e, 'ax-by')
check(e, r'a|\e', str(z))
check(e, 'a|x',   str(f))
check(e, 'a|y',   str(z))
check(e, r'b|\e', str(z))
check(e, 'b|x',   str(z))
check(e, 'b|y',   str(g))

check(f, r'a|\e', str(f))
check(f, 'a|x',   str(f))
check(f, 'a|y',   str(z))
check(f, r'b|\e', str(z))
check(f, 'b|x',   str(z))
check(f, 'b|y',   str(g))

check(g, r'a|\e', str(z))
check(g, 'a|x',   str(f))
check(g, 'a|y',   str(z))
check(g, r'b|\e', str(g))
check(g, 'b|x',   str(z))
check(g, 'b|y',   str(g))
