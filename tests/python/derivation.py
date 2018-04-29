#! /usr/bin/env python
# -*- coding: utf-8 -*-

import vcsn
from test import *

ctx = vcsn.context("[abc] -> seriesset<[xyz] -> q>")

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
    print(f"Check: ∂/∂{s}({re:u}) = {eff:u}")
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
# polynomial of two expressions: "a ⊕ b".
CHECK_EQ('a+b',
         ctx.expression('a(a+b)').derivation('a'))

# While at it, check that trivial identities are properly handled.
CHECK_EQ('b+a',
         ctx.expression('a(b+a)', 'trivial').derivation('a'))


## ---------------------------- ##
## Derive wrt a single letter.  ##
## ---------------------------- ##

# Zero, one.
check(   r'\z', 'a', '∅')
check(   r'\e', 'a', '∅')
check(r'<x>\e', 'a', '∅')

# Letters.
check(   'a', 'a', r'ε')
check(   'a', 'b', '∅')
check('<x>a', 'a', r'⟨x⟩ε')
check('<x>a', 'b', '∅')

# Sum.
check('<x>a+<y>b', 'a', r'⟨x⟩ε')
check('<x>a+<y>b', 'b', r'⟨y⟩ε')
check('<x>a+<y>a', 'a', r'⟨x+y⟩ε')

# Prod.
check('ab', 'a', 'b')
check('ab', 'b', '∅')
check('(<x>a).(<y>a).(<z>a)', 'a', '⟨x⟩⟨y⟩a⟨z⟩a')

# Conjunction.
check('<x>a&<y>a&<z>a', 'a', r'⟨xyz⟩ε')
check('<x>a&<y>a&<z>a', 'b', '∅')

check('(<x>a+<y>b)*&(<z>b+<x>c)*', 'a', '∅')
check('(<x>a+<y>b)*&(<z>b+<x>c)*', 'b', '⟨yz⟩(⟨x⟩a+⟨y⟩b)*&(⟨z⟩b+⟨x⟩c)*')
check('(<x>a+<y>b)*&(<z>b+<x>c)*', 'c', '∅')

# Shuffle.
check('<x>a:<y>a:<z>a', 'a', r'⟨x⟩ε:⟨y⟩a:⟨z⟩a ⊕ ⟨y⟩⟨x⟩a:ε:⟨z⟩a ⊕ ⟨z⟩⟨x⟩a:⟨y⟩a:ε')
check('<x>a:<y>b:<z>c', 'a', r'⟨x⟩ε:⟨y⟩b:⟨z⟩c')
check('<x>a:<y>b:<z>c', 'b', r'⟨y⟩⟨x⟩a:ε:⟨z⟩c')
check('<x>a:<y>b:<z>c', 'c', r'⟨z⟩⟨x⟩a:⟨y⟩b:ε')

check('(<x>a<y>b)*:(<x>a<x>c)*',
      'a', '⟨x⟩(⟨x⟩a⟨y⟩b)*:⟨x⟩c(⟨x⟩a⟨x⟩c)* ⊕ ⟨x⟩⟨y⟩b(⟨x⟩a⟨y⟩b)*:(⟨x⟩a⟨x⟩c)*')
check('(<x>a<y>b)*:(<x>a<x>c)*', 'b', '∅')
check('(<x>a<y>b)*:(<x>a<x>c)*', 'c', '∅')

# Star.
check('a*', 'a', 'a*')
check('a*', 'b', '∅')
check('(<x>a)*', 'a', '⟨x⟩(⟨x⟩a)*')
check('(<x>a)*', 'b', '∅')
check('<x>a*',   'a', '⟨x⟩a*')
check('<x>(<y>a)*', 'a', '⟨xy⟩(⟨y⟩a)*')
check('(<x>a)*<y>', 'a', '⟨x⟩(⟨x⟩a)*⟨y⟩')
check('(<x>a)*<y>', 'aa', '⟨xx⟩(⟨x⟩a)*⟨y⟩')

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
check(r'\z{c}', 'a', r'∅ᶜ')
check(r'\e{c}', 'a', r'∅ᶜ')

check('a{c}', 'a', r'εᶜ')
check('a{c}', 'b', r'∅ᶜ')

check('(a+b){c}', 'a', r'εᶜ')
check('(a+b){c}', 'c', r'∅ᶜ')

check('(a.b){c}', 'a', 'bᶜ')
check('(a.b){c}', 'b', r'∅ᶜ')

check('(a:b){c}', 'a', r'(ε:b)ᶜ')
check('(a:b){c}', 'b', r'(a:ε)ᶜ')
check('(a:b){c}', 'c', r'∅ᶜ')

check('(a*&a*)ᶜ', 'a', '(a*&a*)ᶜ')
check('(a*&a*)ᶜ', 'b', r'∅ᶜ')

check('(<x>(<y>a)*<z>){c}', 'a', '(⟨y⟩a)*ᶜ')
check('(<x>(<y>a)*<z>){c}', 'b', r'∅ᶜ')

check('a{c}{c}', 'a', r'εᶜᶜ')
check('a{c}{c}', 'b', r'∅ᶜᶜ')

# We could easily generate an infinite derived-term automaton here.
# However, currently we don't support a powerful enough normalization
# of polynomials of expressions, so use a simple type of weights.
e = vcsn.context('[a] -> q').expression('((<2>a)*+(<4>aa)*){c}')
check(e, 'a',    '((⟨2⟩a)*+⟨2⟩(a(⟨4⟩(aa))*))ᶜ')
check(e, 'aa',   '((⟨2⟩a)*+(⟨4⟩(aa))*)ᶜ')
check(e, 'aaa',  '((⟨2⟩a)*+⟨2⟩(a(⟨4⟩(aa))*))ᶜ')
check(e, 'aaaa', '((⟨2⟩a)*+(⟨4⟩(aa))*)ᶜ')


## ------------------------------- ##
## Derive wrt to several letters.  ##
## ------------------------------- ##

check('(<x>a)*', 'aa',   '⟨xx⟩(⟨x⟩a)*')
check('(<x>a)*', 'aaaa', '⟨x⁴⟩(⟨x⟩a)*')
check('(<x>a)*', 'aaab', '∅')

check('(<x>a)*(<y>b)*', 'aa',   '⟨xx⟩(⟨x⟩a)*(⟨y⟩b)*')
check('(<x>a)*(<y>b)*', 'aabb', '⟨xxyy⟩(⟨y⟩b)*')


## -------------------- ##
## Classical examples.  ##
## -------------------- ##

# EAT, Example 4.3.
E1 = '(⟨1/6⟩a*+⟨1/3⟩b*)*'
# E1 typed.
E1t = vcsn.context('[ab] -> q').expression(E1)
check(E1t,  'a',  "⟨1/3⟩a*" + E1)
check(E1t,  'b',  "⟨2/3⟩b*" + E1)
check(E1t, 'aa',  "⟨4/9⟩a*" + E1)
check(E1t, 'ab',  "⟨2/9⟩b*" + E1)
check(E1t, 'ba',  "⟨2/9⟩a*" + E1)
check(E1t, 'bb', "⟨10/9⟩b*" + E1)

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
check_br(   r'\z', 'a', '∅')
check_br(   r'\e', 'a', '∅')
check_br(r'<x>\e', 'a', '∅')

# Letters.
check_br(   'a', 'a', r'ε')
check_br(   'a', 'b', '∅')
check_br('<x>a', 'a', r'⟨x⟩ε')
check_br('<x>a', 'b', '∅')

# Sum.
check_br('<x>a+<y>b', 'a', r'⟨x⟩ε')
check_br('<x>a+<y>b', 'b', r'⟨y⟩ε')
check_br('<x>a+<y>a', 'a', r'⟨x+y⟩ε')

# Prod.
check_br('ab', 'a', 'b')
check_br('ab', 'b', '∅')
check_br('(<x>a)(<y>a)(<z>a)', 'a', '⟨xy⟩a⟨z⟩a')

# Exterior products.
check_br('(<x>ab)<y>', 'a', '⟨xy⟩b')

# Star.
check_br('a*', 'a', 'a*')
check_br('a*', 'b', '∅')
check_br('(<x>a)*', 'a', '⟨x⟩(⟨x⟩a)*')
check_br('(<x>a)*', 'b', '∅')
check_br('<x>a*',   'a', '⟨x⟩a*')
check_br('<x>(<y>a)*', 'a', '⟨xy⟩(⟨y⟩a)*')

# Check the right-exterior product.
check_bdt('<x>(<y>a)*<z>', 'ext-prod-breaking')

## --------------------- ##
## Documented examples.  ##
## --------------------- ##

# On The Number Of Broken Derived Terms Of A Rational Expression.
ctx = vcsn.context('[ab] -> b')
F2 = 'a*+b*'
E2 = "({F2})(a({F2}))".format(F2=F2)
E2t = ctx.expression(E2)
check(E2t, 'a', "{F2} ⊕ a*a({F2})".format(F2=F2))
check(E2t, 'b', "b*a({})".format(F2))

# Example 2.
check(E2t, 'aa', "a* ⊕ a*+b* ⊕ a*a({})".format(F2))
check(E2t, 'ab', 'b*')
check(E2t, 'ba', F2)
check(E2t, 'bb', "b*a({})".format(F2))

# Example 3.
check_dt(E2t, 'e2-dt')

# Example 5.
check_br(E2t,  'a', "a* ⊕ b* ⊕ a*a({})".format(F2))
check_br(E2t,  'b', "b*a({})".format(F2))
check_br(E2t, 'aa', "a* ⊕ b* ⊕ a*a({})".format(F2))
check_br(E2t, 'ab', 'b*')
check_br(E2t, 'ba', "a* ⊕ b*")
check_br(E2t, 'bb', "b*a({})".format(F2))

# Example 6.
check_bdt(E2t, 'e2-dt-breaking')

# Figure 3.
fig3 = vcsn.context('[abcd] -> B').expression('a(b+c+d)')
check_dt(fig3, 'h3-dt')
check_bdt(fig3, 'h3-dt-breaking')


## -------- ##
## Tuples.  ##
## -------- ##

ctx = vcsn.context('[...] x [...] -> Q')
check('ab|xy', 'a|x', 'b|y')

ctx = vcsn.context('[...] x [...] -> Q')
check('ab|xy', 'a|x', 'b|y')
check('ab|xy', r'a|ε', '∅')
check('a*|x*', 'a|x', 'a*|x*')

ctx = vcsn.context('[...] x [...] -> Q')
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
