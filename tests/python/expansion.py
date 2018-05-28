#! /usr/bin/env python

import vcsn
from test import *


def expr(e, *args):
    if not isinstance(e, vcsn.expression):
        e = ctx.expression(e, *args)
    return e

def check_aut(e):
    ind = e.automaton('inductive')
    exp = e.automaton('expansion')
    CHECK_EQ(ind.is_valid(), exp.is_valid())
    if ind.is_valid():
        CHECK_EQUIV(ind, exp)


## ----- ##
## Add.  ##
## ----- ##
ctx = vcsn.context('[...] -> Q')
def check(r1, r2, exp):
    e1 = expr(r1)
    e2 = expr(r2)
    CHECK_EQ(exp, e1.expansion() + e2.expansion())
    CHECK_EQ(exp, (e1 + e2).expansion())
    check_aut(e1+e2)

check('a', '<-1>a', '⟨0⟩')
check('a', 'a', 'a⊙[⟨2⟩ε]')

check('ab', 'cd', 'a⊙[b] ⊕ c⊙[d]')
check('a', 'bcd', 'a⊙[ε] ⊕ b⊙[cd]')
check('abab', 'bbbb', 'a⊙[bab] ⊕ b⊙[b³]')
check('(<1/2>a)*', '(<1/2>a)*(<1/3>b)*', '⟨2⟩ ⊕ a⊙[⟨1/2⟩(⟨1/2⟩a)* ⊕ ⟨1/2⟩(⟨1/2⟩a)*(⟨1/3⟩b)*] ⊕ b⊙[⟨1/3⟩(⟨1/3⟩b)*]')
check('a', r'\e', '⟨1⟩ ⊕ a⊙[ε]')
check('a', r'\z', 'a⊙[ε]')


## ------------ ##
## Complement.  ##
## ------------ ##
def check(r):
    '''Check that `~` on expansions corresponds to the expansion of
    `~` on expressions.'''
    e = expr(r)
    eff = ~(e.expansion())
    exp = (~e).expansion()
    CHECK_EQ(exp, eff)
# Currently, inductive does not support ~ with nullable.
#    check_aut(~e)

check(r'\z')
check(r'\e')
check(r'<1/2>\e')
check('a')
check('(<1/2>a*+<1/3>b*)*')
check('ab')
check('abab')
check('a*')
check('(<1/2>a)*')

# Also check with [...] -> B.
lal = vcsn.context('[...] -> B')
check(lal.expression('abc'))
check(lal.expression(r'\z'))

# And with LAO.
lao = vcsn.context('lao, q')
check(lao.expression(r'(<1/2>\e)*'))




## --------- ##
## Compose.  ##
## --------- ##
ctx = vcsn.context('[abcde] x [abcde] -> q')
def check(r1, r2, exp):
    '''Check that `@` between expansions corresponds to the expansion of
    `@` between expressions.'''
    e1 = expr(r1)
    e2 = expr(r2)
    print('check: {} @ {}'.format(e1, e2))
    # Cannot yet require @ support from Python.
    e = e1.compose(e2)
    CHECK_EQ(exp, e1.expansion().compose(e2.expansion()))
    CHECK_EQ(exp, e.expansion())
    check_aut(e)

check('a|a', 'a|a', 'a|a⊙[ε]')
check('a|b', 'b|c', 'a|c⊙[ε]')
check('a*|b*', 'b*|c*',
      '⟨1⟩ ⊕ ε|ε⊙[ε|b*@b*|ε] ⊕ ε|c⊙[ε@ε|c* ⊕ ε|b*@b*|c* ⊕ (ε|b)(ε|b*)@ε|c*] ⊕ a|ε⊙[a*|ε@ε ⊕ a*|b*@b*|ε ⊕ a*|ε@(b|ε)(b*|ε)] ⊕ a|c⊙[a*|ε@ε|c* ⊕ a*|b*@b*|c* ⊕ a*|ε@(b|ε)(b*|c*) ⊕ (ε|b)(a*|b*)@ε|c*]')
check(r'a|\e', r'\e|b', 'a|b⊙[ε]')
check(r'(a|\e)(b|c)', 'c|a', 'a|a⊙[b|ε]')
check('a|b', r'(\e|a)(b|c)', 'a|a⊙[ε|c]')
check(r'(a|c)+(b|\e)', r'(c|d)(\e|e)', 'a|d⊙[ε|e] ⊕ b|d⊙[ε@(c|ε)(ε|e)]')
check(r'(a|c)(b|\e)', 'c|d', r'a|d⊙[b|ε]')



## ------------- ##
## Conjunction.  ##
## ------------- ##
ctx = vcsn.context('[...] -> Q')
def check(r1, r2, exp):
    '''Check that `&` between expansions corresponds to the expansion of
    `&` between expressions.'''
    e1 = expr(r1)
    e2 = expr(r2)
    CHECK_EQ(exp, e1.expansion() & e2.expansion())
    CHECK_EQ(exp, (e1&e2).expansion())
    check_aut(e1&e2)

check('ab', 'cd', '⟨0⟩')
check('(ab)*', 'a*b*', '⟨1⟩ ⊕ a⊙[b(ab)*&a*b*]')
check('(<1/2>a)*', '(<1/2>a)*(<1/3>b)*',
      r'⟨1⟩ ⊕ a⊙[⟨1/4⟩(⟨1/2⟩a)*&(⟨1/2⟩a)*(⟨1/3⟩b)*]')
check('a', r'\e', '⟨0⟩')
check('a', r'\z', '⟨0⟩')

# Check that ab&ac = 0, not a.[0].
check('ab', 'ac', '⟨0⟩')


## --------- ##
## Division. ##
## --------- ##
ctx = vcsn.context('[...] -> Q')
def check(e1, e2, exp):
    e1 = expr(e1)
    e2 = expr(e2)
    e = e1.ldivide(e2)
    CHECK_EQ(exp, e.expansion())
    CHECK_EQ(exp, e1.expansion().ldivide(e2.expansion()))
    check_aut(e)

check(r'a', r'ab', r'ε⊙[b]')
check(r'a*{\}a', r'\e', r'ε⊙[a{\}ε ⊕ (a*{\}ε){\}ε]')
check(r'a{\}ab', r'bc', r'ε⊙[b{\}bc]')


## ----- ##
## Mul.  ##
## ----- ##
CHECK_EQ(r'a⊙[ε(b(cd))]', ctx.expression('a(b(cd))', 'none').expansion())
CHECK_EQ(r'a⊙[((εb)c)d]', ctx.expression('((ab)c)d', 'none').expansion())
CHECK_EQ(r'a⊙[(εb)(cd)]', ctx.expression('(ab)(cd)', 'none').expansion())


## ------ ##
## Star.  ##
## ------ ##

XFAIL(lambda: vcsn.Q.expression('a**').expansion(),
      r'''ℚ: value is not starrable: 1
  while computing expansion of: a**''')



## ------- ##
## Tuple.  ##
## ------- ##
ctx = vcsn.context('[...] -> Q')
def check(r1, r2):
    '''Check that `|` between expansions corresponds to the expansion of
    `|` between expressions.'''
    exp1 = expr(r1)
    exp2 = expr(r2)
    eff = exp1.expansion() | exp2.expansion()
    exp = (exp1 | exp2).expansion()
    CHECK_EQ(exp, eff)
    check_aut(exp1|exp2)

check('ab', 'cd')
check('a', 'bcd')
check('abab', 'bbbb')
check('(<1/2>a)*', '(<1/2>a)*(<1/3>b)*')
check('a', r'\e')
check('a', r'\z')


## -------- ##
## Weight.  ##
## -------- ##
def check(r, w, exp):
    '''Check that `weight * expansion` corresponds to `expansion * weight`
    and to the expansion of `weight * expression`.'''
    e = expr(r, 'trivial')
    leff = e.expansion() * w
    lexp = (e * w).expansion()
    CHECK_EQ(lexp, leff)
    check_aut(e*w)
    reff = w * e.expansion()
    rexp = (w * e).expansion()
    CHECK_EQ(rexp, reff)
    check_aut(w*e)

check('abcd', 2, 'a.[<2>bcd]')
check('a*', 10, '<10> + a.[<10>a*]')
check('[ab]{3}', 4, 'a.[<4>[ab]{2}] + b.[<4>[ab]{2}]')
check('a*+b*+c+c*', 3, 'toto')
check('a', 1, 'a⊙[ε]')
check('a', 0, '⟨0⟩')
