#! /usr/bin/env python

import vcsn
from test import *

## ------------- ##
## Polynomials.  ##
## ------------- ##

# check CONTEXT INPUT OUTPUT
# --------------------------
def check(ctx, p, utf8=None, **kwargs):
    ctx = vcsn.context(ctx)
    if utf8:
        kwargs['utf8'] = utf8
    p = ctx.polynomial(p)
    for fmt, exp in kwargs.items():
        print("format: {}".format(fmt))
        CHECK_EQ(exp, p.format(fmt))

check('[a] -> b', r'\z', '∅')
check('[a] -> b', r'\z+\z', '∅')

check('[a] -> b', r'a', r'a')
check('[ab] -> b', r'a+b+b+a', r'a ⊕ b')

for p in [r'[abcd]', r'[dcba]', r'[abcdabcd]', r'[a-d]']:
    check('[abcd] -> q', p, text='a + b + c + d', utf8=r'a ⊕ b ⊕ c ⊕ d',
          list='a\nb\nc\nd')

check('[abcd] -> q', r'[a-bd]', r'a ⊕ b ⊕ d')

check('[ab] -> q', r'a+b+b+a+b',
      text=r'<2>a + <3>b', utf8=r'⟨2⟩a ⊕ ⟨3⟩b', list='<2>a\n<3>b')
check('[ab] -> q', r'a+b+b+<-1>a+b', r'⟨3⟩b')

check('[a]* -> b', r'\e+\e', 'ε')

check('[ab]* -> q', r'ba+ab+bb+aa+a+b+\e+bb+aa',
      r'ε ⊕ a ⊕ b ⊕ ⟨2⟩aa ⊕ ab ⊕ ba ⊕ ⟨2⟩bb')

check('[abc] -> RatE[[xyz] -> Q]',
      'a + a + <x>b + <y>b',
      '⟨⟨2⟩ε⟩a ⊕ ⟨x+y⟩b')

# Be sure to have proper ordering on tuples with LAN.
# For the time being, we support both | and , as separators.
check('[abc] x [xyz] -> q',
      r'a|\e + \e|x + a|\e + a|x + \e|y',
      r'ε|x ⊕ ε|y ⊕ ⟨2⟩a|ε ⊕ a|x')
check('[abc] x [xyz] -> q',
      r'(a,\e) + (\e,x) + (a,\e) + (a, x) + (\e,y)',
      r'ε|x ⊕ ε|y ⊕ ⟨2⟩a|ε ⊕ a|x')

# Check that we don't ignore trailing characters.
XFAIL(lambda: vcsn.context('[ab] -> q').polynomial('<123>a*'))

## ----------------- ##
## LaTeX rendering.  ##
## ----------------- ##

c = vcsn.context("[abc] -> expressionset<[xyz] -> q>")
CHECK_EQ(r'\left\langle  \left\langle 2 \right\rangle \,\varepsilon\right\rangle a \oplus \left\langle x + y\right\rangle b',
         c.polynomial(r'a + a + <x>b + <y>b').format("latex"))


## ----- ##
## Add.  ##
## ----- ##

poly = vcsn.context('[...] -> Q').polynomial
CHECK_EQ('a ⊕ b ⊕ x ⊕ y', poly('a+b') + poly('x+y'))
CHECK_EQ('⟨7⟩a ⊕ ⟨7⟩c',
         poly('<2>a+<3>b') + poly('<5>a + <-3>b + <7>c'))


## --------- ##
## Weights.  ##
## --------- ##

poly = vcsn.context('[...]* -> Q').polynomial
# Left.
CHECK_EQ('⟨2⟩a ⊕ ⟨2⟩b', 2 * poly('a+b'))
CHECK_EQ('a ⊕ b',       1 * poly('a+b'))
CHECK_EQ('∅',           0 * poly('a+b'))

# Right.
#
# Here the result must be different when the labels support weights,
# e.g., polynomials of expressions.  This is checked elsewhere, when
# dealing with expansions of expressions.
CHECK_EQ('⟨2⟩a ⊕ ⟨2⟩b', poly('a+b') * 2)
CHECK_EQ('a ⊕ b',       poly('a+b') * 1)
CHECK_EQ('∅',           poly('a+b') * 0)


## ----- ##
## Mul.  ##
## ----- ##

poly = vcsn.context('[...]* -> Q').polynomial
CHECK_EQ('ax ⊕ ay ⊕ bx ⊕ by', poly('a+b') * poly('x+y'))
CHECK_EQ('⟨10⟩ax ⊕ ⟨14⟩ay ⊕ ⟨15⟩bx ⊕ ⟨21⟩by',
         poly('<2>a+<3>b') * poly('<5>x+<7>y'))
CHECK_EQ('⟨20⟩a ⊕ ⟨30⟩b',
         poly('<2>a+<3>b') * 10)
CHECK_EQ('⟨20⟩a ⊕ ⟨30⟩b',
         10 * poly('<2>a+<3>b'))


## ----- ##
## Pow.  ##
## ----- ##

poly = vcsn.context('[...]* -> Q').polynomial
CHECK_EQ(r'ε', poly('a') ** 0)
CHECK_EQ('⟨4⟩a ⊕ ⟨12⟩b ⊕ ⟨-3⟩c', poly('<4>a + <12>b + <-3>c') ** 1)
CHECK_EQ('aaa ⊕ aab ⊕ aba ⊕ abb ⊕ baa ⊕ bab ⊕ bba ⊕ bbb', poly('a + b') ** 3)



## ------------- ##
## Conjunction.  ##
## ------------- ##

poly = vcsn.context('[...] -> Q').polynomial
CHECK_EQ('∅', poly('a+b') & poly('x+y'))
CHECK_EQ('⟨10⟩a',
         poly('<2>a+<3>b') & poly('<5>a+<7>c'))


## ------- ##
## Tuple.  ##
## ------- ##

poly = vcsn.context('[...] -> Q').polynomial
CHECK_EQ('a|x ⊕ a|y ⊕ b|x ⊕ b|y', poly('a+b') | poly('x+y'))
CHECK_EQ('⟨10⟩a|x ⊕ ⟨14⟩a|y ⊕ ⟨15⟩b|x ⊕ ⟨21⟩b|y',
         poly('<2>a+<3>b') | poly('<5>x+<7>y'))


## --------- ##
## Compose.  ##
## --------- ##

poly = vcsn.context('[...] x [...] -> Q').polynomial
CHECK_EQ('a|A ⊕ b|B', poly('a|x + b|y').compose(poly('x|A + y|B')))
CHECK_EQ('⟨10⟩a|A ⊕ ⟨21⟩b|B',
         poly('<2>a|x + <3>b|y + c|z').compose(poly('<5>x|A + <7>y|B + c|C')))
