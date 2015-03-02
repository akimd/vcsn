#! /usr/bin/env python

import vcsn
from test import *

lal = vcsn.context("lal_char,b")
lan = vcsn.context("lan_char,b")
law = vcsn.context("law_char,b")

def check(ctx, a, ctx2 = None, b = None):
    if ctx2 is None:
        ctx2 = ctx
    if b is None:
        b = a
    aut = ctx.expression(a).derived_term()
    ref = ctx2.expression(b).derived_term()
    CHECK_ISOMORPHIC(ref, aut.letterize())

# Idempotence for lal
check(lal, 'a')
check(lal, 'abc')
check(lal, '(ab+c)*de')

# law -> lan
check(law, 'a', lan)
check(law, 'abc', lan)
check(law, '(ab+cd)*de', lan)

# transducers might need padding.
latw = vcsn.context("lat<law_char, lal_char>, z")
latn = vcsn.context("lat<lan_char, lan_char>, z")

check(latw, "'(a,b)'", latn)
check(latw, "<2>'(abc,x)'", latn, r"<2>'(a,x)''(b,\e)''(c,\e)'")
check(latw, r"('(abc,x)'+'(def,y)')*'(ghi,z)'",
      latn, r"(('a,x''b,\e''c,\e')+('d,y''e,\e''f,\e'))*'g,z''h,\e''i,\e'")
