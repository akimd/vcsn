#! /usr/bin/env python

import vcsn
from test import *

c = vcsn.context("lat<lan<lal_char(abc)>,lan<lal_char(xyz)>>,b")

def check(a, res):
    aut = c.expression(a).standard()
    CHECK_EQ(aut.has_bounded_lag(), res)


check(r"'(a,x)'", True)
check(r"'(a,x)'*", True)
check(r"'(a,\e)'", True)
check(r"'(a,\e)'*", False)
check(r"'(\e,x)'*", False)
check(r"'(\e,\e)'*", True)
check(r"'(a,x)''(\e,\e)'*'(a,x)'", True)
check(r"'(a,x)''(a,\e)'*'(a,x)'", False)
check(r"('(a,\e)''(\e,x)')*", True)
check(r"('(a,\e)''(a,x)''(b,x)''(\e,x)')*", True)
check(r"('(a,\e)''(a,\e)''(b,x)''(\e,x)')*", False)
check(r"'(a,x)'('(a,\e)''(\e,x)')*'(a,x)'", True)
