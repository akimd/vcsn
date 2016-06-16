#! /usr/bin/env python

import vcsn
from test import *

# Working in Z, not Q, to check GCD.
z = vcsn.context('law_char(abcd), z')

## ----------------------------- ##
## div(expression, expression).  ##
## ----------------------------- ##

def check_div(l, r):
    make = z.expression
    CHECK_EQ(make(r'({}){{\}}({})'.format(l, r)), make(l) // make(r))
    CHECK_EQ(make(r'({}){{\}}({})'.format(l, r)), make(l).ldivide(make(r)))

    CHECK_EQ(make('({}){{/}}({})'.format(l, r)), make(l) / make(r))
    CHECK_EQ(make('({}){{/}}({})'.format(l, r)), make(l).rdivide(make(r)))

check_div(r'\e', 'a')
check_div(r'\e', '<2>a')
check_div(r'<2>\e', '<2>a')
check_div(r'\e', 'a+b')
check_div(r'\e', 'a+<2>b')
check_div(r'c+d', 'ab')
check_div(r'<2>c+<3>d', 'ab')
check_div(r'<2>c+<3>d', '<2>ab')

## ------------------------------ ##
## ldivide(polynomial, polynomial).  ##
## ------------------------------ ##

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
