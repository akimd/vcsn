#! /usr/bin/env python

import vcsn
from test import *

lal = vcsn.context("lal,b")
law = vcsn.context("law,b")

def check(ctx, a, ctx2 = None, b = None):
    if ctx2 is None:
        ctx2 = ctx
    if b is None:
        b = a
    aut = ctx.expression(a).derived_term()
    ref = ctx2.expression(b).derived_term()
    res = aut.letterize()
    CHECK_ISOMORPHIC(ref, res)
    CHECK(res.is_letterized())

# Idempotence for lal
check(lal, 'a')
check(lal, 'abc')
check(lal, '(ab+c)*de')

# law -> lal
check(law, 'a', lal)
check(law, 'abc', lal)
check(law, '(ab+cd)*de', lal)

# transducers might need padding.
#
latw = vcsn.context("lat<law, lal>, z")
latl = vcsn.context("[...] x [...] -> z")

check(latw, "(a|b)", latl)
check(latw, "<2>(abc|x)", latl, r"<2>(a|x)(b|\e)(c|\e)")
check(latw, r"((abc|x)+(def|y))*(ghi|z)",
      latl, r"(((a|x)(b|\e)(c|\e))+((d|y)(e|\e)(f|\e)))*(g|z)(h|\e)(i|\e)")

def check(aut, res):
    CHECK_EQ(res, aut.is_letterized())
    if not res:
        CHECK(aut.letterize().is_letterized())

def check_standard(ctx, a, res):
    check(ctx.expression(a).standard(), res)

def check_thompson(ctx, a, res):
    check(ctx.expression(a).thompson(), res)

check_standard(lal, "a", True)
check_standard(lal, "abc", True)
check_standard(lal, "a*+(bc+de*)*", True)

check_thompson(lal, "a", True)
check_thompson(lal, "abc", True)
check_thompson(lal, "a*+(bc+de*)*", True)


check_standard(law, "a", True)
check_standard(law, "abc", False)
check_standard(law, "a*+(bc+de*)*", False)

check_thompson(law, "a", True)
check_thompson(law, "abc", False)
check_thompson(law, "a*+(bc+de*)*", False)


check_standard(latw, "(a|b)", True)
check_standard(latw, "(abc|d)", False)
check_standard(latw, "(a|d)(b|e)(c|f)", True)
check_standard(latw, "(a|b)+((bc|x)+(de|z))", False)

check_thompson(latw, "(a|b)", True)
check_thompson(latw, "(abc|d)", False)
check_thompson(latw, "(a|d)(b|e)(c|f)", True)
check_thompson(latw, "(a|b)+((bc|x)+(de|z))", False)

def check(ctx, a):
    exp = ctx.expression(a)
    ref = exp.derived_term().letterize()
    aut = exp.thompson().realtime()
    CHECK(ref.is_realtime())
    CHECK_ISOMORPHIC(ref.minimize(), aut.minimize())
    CHECK(aut.is_realtime())

check(lal, "a")
check(lal, "ab*+cde")

check(law, "a")
check(law, "abc")
check(law, "(ab)*+(cd+ef)*")

check(latw, "(a|b)")
check(latw, "<2>(abc|x)")
check(latw, r"((abc|x)+(def|y))*(ghi|z)")
