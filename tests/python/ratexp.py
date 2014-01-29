#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context("lal_char(abcd)_b")

## -------- ##
## Concat.  ##
## -------- ##
def check_concat(r1, r2):
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

## -------------- ##
## Intersection.  ##
## -------------- ##
def check_inter(r1, r2):
    eff = ctx.ratexp(r1) & ctx.ratexp(r2)
    exp = ctx.ratexp('({})&({})'.format(r1, r2))
    CHECK_EQ(exp, eff)

check_inter('ab', 'cd')
check_inter('a', 'bcd')
check_inter('abab', 'bbbb')
check_inter('a*', 'a*b*')
check_inter('a*+b*+c+c*', '(a*+b*+c+c*)*')
check_inter('(a*+b*+c+c*)*', '(a*a*a*b*b*a+b+a+b+a)')
check_inter('a', '\e')
check_inter('a', '\z')

## ----- ##
## Sum.  ##
## ----- ##
def check_sum(r1, r2):
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

PLAN()
