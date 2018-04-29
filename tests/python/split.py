#! /usr/bin/env python

import vcsn
from test import *

# We are checking the support for quotients, which requires the label
# one.
ctx = vcsn.context('[...] -> RatE[[...] -> Q]')
cexp = ctx.expression

# check INPUT [RESULT = INPUT]
# ----------------------------
# Check that the splitting of INPUT is RESULT.
def check(e, exp=None):
    if exp is None:
        exp = e
    if not isinstance(e, vcsn.expression):
        e = cexp(e)
    p = e.split()
    CHECK_EQ(exp, p)
    # The polynomial, turned into an expression, and the input
    # expression are equivalent.
    CHECK_EQUIV(cexp(p.format('text')), e)
    # Splitting polynomials is idempotent.
    CHECK_EQ(p, p.split())

check(r'\z',            '∅')
check(r'<x>\e',         '⟨x⟩ε')
check('<x>a',           '⟨x⟩a')
check('(<x>a)*<y>',     '(⟨x⟩a)*⟨y⟩')
check('<xy>a<z>b',      '⟨xy⟩a⟨z⟩b')
check('<x>a+<y>b',      '⟨x⟩a ⊕ ⟨y⟩b')
check('<x>a+<y>b+<z>a', '⟨x+z⟩a ⊕ ⟨y⟩b')

check('a*{c}',        'a*ᶜ')
check(r'a*{\}b*',     'a*{\}b*')
check('(a+b)&(c+d)',  '(a+b)&(c+d)')
check('(a+b):(c+d)',  '(a+b):(c+d)')
check('(a+b)&:(c+d)', '(a+b)&:(c+d)')
check('a*{T}',        'a*ᵗ')

e0 = cexp(r'<E>\e')
e1 = cexp(r'<A>a')
e2 = cexp(r'<B>b')

check((e1 + e2) ** 2,  '⟨A⟩a(⟨A⟩a+⟨B⟩b) ⊕ ⟨B⟩b(⟨A⟩a+⟨B⟩b)')
check((e1 + e2) ** 3,  '⟨A⟩a(⟨A⟩a+⟨B⟩b)² ⊕ ⟨B⟩b(⟨A⟩a+⟨B⟩b)²')
check((e0 + e1 + e2) ** 2,
      r'⟨EE⟩ε ⊕ ⟨EA⟩a ⊕ ⟨EB⟩b ⊕ ⟨A⟩a(⟨E⟩ε+⟨A⟩a+⟨B⟩b) ⊕ ⟨B⟩b(⟨E⟩ε+⟨A⟩a+⟨B⟩b)')


## --------------------- ##
## Documented examples.  ##
## --------------------- ##

# On The Number Of Broken Derived Terms Of A Rational Expression.
# Example 4.
F2 = 'a*+b*'
E2 = "({F2})(a({F2}))".format(F2=F2)
check(E2, "a*a({F2}) ⊕ b*a({F2})".format(F2=F2))
check(F2, "a* ⊕ b*")
