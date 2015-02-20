#! /usr/bin/env python

import vcsn
from test import *

lal = vcsn.context("lal_char,b")
lan = vcsn.context("lan_char,b")
law = vcsn.context("law_char,b")

def check(ctx, a, res):
    aut = ctx.expression(a).derived_term()
    CHECK(aut.letterize().is_isomorphic(res))

def check_exp(ctx, a, ctx2, b):
    check(ctx, a, ctx2.expression(b).derived_term().strip())

# Idempotence for lal
check_exp(lal, 'a', lal, 'a')
check_exp(lal, 'abc', lal, 'abc')
check_exp(lal, '(ab+c)*de', lal, '(ab+c)*de')

# law -> lan
check_exp(law, 'a', lan, 'a')
check_exp(law, 'abc', lan, 'abc')
check_exp(law, '(ab+cd)*de', lan, '(ab+cd)*de')

latw = vcsn.context("lat<law_char, lal_char>, z")
latn = vcsn.context("lat<lan_char, lan_char>, z")

check_exp(latw, "'(a,b)'", latn, "'(a,b)'")
check_exp(latw, "<2>'(abc,x)'", latn, r"<2>'(a,x)''(b,\e)''(c,\e)'")
check_exp(latw, r"('(abc,x)'+'(def,y)')*'(ghi,z)'", latn,
          r"(('(a,x)''(b,\e)''(c,\e)')+('(d,y)''(e,\e)''(f,\e)'))*'(g,z)''(h,\e)''(i,\e)'")
