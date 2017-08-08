#! /usr/bin/env python

import vcsn
from test import *

## ------------------------------- ##
## ldivide(automaton, automaton).  ##
## rdivide(automaton, automaton).  ##
## ------------------------------- ##

def exp(e):
    return e if isinstance(e, vcsn.expression) else vcsn.context(ctx).expression(e)

def label(l):
    return vcsn.context(ctx).word_context().label(l)

def aut(e):
    return exp(e).automaton()

def divide(fun, l, r):
    if fun == 'ldivide':
        return l.ldivide(r)
    elif fun == 'rdivide':
        return l.rdivide(r)
    else:
        FAIL('invalid operation: {}'.format(fun))


def check_fun(fun, res, l, r):
    print("check: {} == {}.{}({})".format(res, l, fun, r))
    print("check", fun, res, l, r)
    lhs = aut(l)
    rhs = aut(r)
    eff = divide(fun, lhs, rhs)
    # Make sure that we did not modify the rhs.  This did happen with
    # ldivide.
    print("check lhs did not change", fun)
    CHECK_EQ(lhs, aut(l))
    print("check rhs did not change", fun)
    CHECK_EQ(rhs, aut(r))
    print("check validity", fun)
    CHECK_EQ(aut(res).is_valid(), eff.is_valid())
    if eff.is_valid():
        print("check equivalence: {} == {} = {}.{}({})"
              .format(res, eff.expression(), l, fun, r))
        CHECK_EQUIV(aut(res), eff)
    else:
        SKIP('invalid expression', res)
    return eff

def check(l, r, resl, resr=None):
    if resr is None:
        resr = resl
    check_fun('ldivide', resl, l, r)
    check_fun('rdivide', resr, r, l)

# Boolean.  Very important, as we have two different implementations.
ctx = 'lal, b'
check('abc', 'abcc+abcd', 'c+d', '\z')
check('abcd+bbcd', 'a+b', '\z', 'bcd')


# Weighted.
ctx = 'lal_char, q'
# Addition
check('<2>a', '<6>(a+b)', r'<12>\e')
# Concatenation
check('<2>a', '<6>ab', '<12>b', '\z')
check('<2>b', '<6>ab', '\z', '<12>a')
# Star
check('<2>a', '<6>a*', '<12>a*')
# Epsilon cycles
check('(<1/2>a)*', 'a*', r'(<1/2>\e)*a*')
print('misc')
# Empty result
check('ab', '<2>a', r'\z')
# Misc
check('<2>a+<3>b', '<6>a*b', r'<12>a*b+<18>\e', r'<18>a*')
check('<2>a+<3>b', '<6>ab*', r'<12>b*', r'<12>\e+<18>ab*')
check('a+b', '(a+b)c', '<2>c', '\z')

CHECK_EQ(metext('ldiv.gv'), aut('a+b').ldivide(aut('a*b*')))
CHECK_EQ(metext('rdiv.gv'), aut('a*b*').rdivide(aut('a+b')))

# Cross check with derived_term and inductive,standard.
def check(l, r):
    for fun in ['ldivide', 'rdivide']:
        print("{}: {}, {}".format(fun, l, r))
        divide_exp = divide(fun, l, r)
        divide_aut = check_fun(fun, divide_exp, l, r)

        if divide_aut.is_valid():
            for algo in ['expansion', 'inductive,standard']:
                a = divide_exp.automaton(algo)
                CHECK_EQUIV(divide_aut, a)
        else:
            SKIP('invalid expression', divide_exp)

exprs = [r'\z', r'<2>\e', '<3>(a+<4>b)*<5>c', '(<6>a*b+<7>ac)*',
        '<8>(a*b+c)*bba+a(b(c+<9>d)*+a)']
exprs = [exp(e) for e in exprs]
for lhs in exprs:
    for rhs in exprs:
        check(lhs, rhs)



# Working in Z, not Q, to check GCD.
z = vcsn.context('law_char(abcd), z')

## ----------------------------- ##
## div(expression, expression).  ##
## ----------------------------- ##

def check(l, r):
    make = z.expression
    CHECK_EQ(make(r'({}){{\}}({})'.format(l, r)), make(l) // make(r))
    CHECK_EQ(make(r'({}){{\}}({})'.format(l, r)), make(l).ldivide(make(r)))

    CHECK_EQ(make('({}){{/}}({})'.format(l, r)), make(l) / make(r))
    CHECK_EQ(make('({}){{/}}({})'.format(l, r)), make(l).rdivide(make(r)))

check(r'\e', 'a')
check(r'\e', '<2>a')
check(r'<2>\e', '<2>a')
check(r'\e', 'a+b')
check(r'\e', 'a+<2>b')
check(r'c+d', 'ab')
check(r'<2>c+<3>d', 'ab')
check(r'<2>c+<3>d', '<2>ab')

## --------------------- ##
## div(label, label).    ##
## --------------------- ##

def check(fun, l, r, res):
    CHECK_EQ(res, divide(fun, label(l), label(r)))

check('ldivide', 'a', 'ab', 'b')
check('ldivide', 'aba', 'abaaba', 'aba')
check('ldivide', r'\e', 'abaaba', 'abaaba')
check('ldivide', r'\e', r'\e', r'\e')

XFAIL(lambda: label('b').ldivide(label('ab')),
      'ldivide: invalid arguments: b, ab')

check('rdivide', 'ba', 'a', 'b')
check('rdivide', 'abaaba', 'aba', 'aba')
check('rdivide', 'abaaba', r'\e', 'abaaba')
check('rdivide', r'\e', r'\e', r'\e')

XFAIL(lambda: label('a').rdivide(label('ab')),
      'rdivide: invalid arguments: a, ab')

## --------------------------------- ##
## ldivide(polynomial, polynomial).  ##
## --------------------------------- ##

def check(exp, lhs, rhs):
    make = z.polynomial
    exp = make(exp)
    lhs = make(lhs)
    rhs = make(rhs)
    CHECK_EQ(exp, lhs.ldivide(rhs))

def xfail(lhs, rhs):
    make = z.polynomial
    lhs = make(lhs)
    rhs = make(rhs)
    XFAIL(lambda: lhs.ldivide(rhs))

xfail('a', 'b')

check(r'\e', 'a', 'a')
check(r'\e', '<2>a', '<2>a')
check(r'<2>\e', '<2>a', '<4>a')

xfail('a', 'a+b')
check(r'\e', 'a+b', 'a+b')
xfail('a+b', 'a+<2>b')
check(r'\e', 'a+<2>b', 'a+<2>b')

xfail('ab', 'ac')
check(r'c+d', 'ab', 'abc+abd')
check(r'<2>c+<3>d', 'ab', '<2>abc+<3>abd')

check(r'<2>c+<3>d', '<2>ab', '<4>abc+<6>abd')

xfail('a+aa', 'a')

## ------------------------------ ##
## lgcd(polynomial, polynomial).  ##
## ------------------------------ ##

def check(exp, lhs, rhs):
    make = z.polynomial
    exp = make(exp)
    lhs = make(lhs)
    rhs = make(rhs)
    CHECK_EQ(exp, lhs.lgcd(rhs))

check('a', 'a', 'a')
check(r'\e', 'a', 'b')

check('<2>a', '<2>a', '<2>a')
check('<2>a', '<2>a', '<4>a')
check('a',    '<2>a', '<3>a')

check('ab', 'ab', 'ab')
#check('a', 'ab', 'ac')

#check('<2>a', '<4>ab', '<6>ac')

#check('<2>a+<3>b', '<2>a+<3>b', '<4>ac+<6>bc')
#check('<2>a+<3>b', '<6>a+<9>b', '<4>ac+<6>bc')

check(r'\e', 'a+b', 'a+c')
