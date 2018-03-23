#! /usr/bin/env python

import vcsn
from test import *

c = vcsn.context('[abc] x [xyz] -> b')

def check(a, exp):
    aut = c.expression(a).standard()
    CHECK_EQ(exp, aut.has_bounded_lag())


check(r'(a|x)', True)
check(r'(a|x)*', True)
check(r'(a|\e)', True)
check(r'(a|\e)*', False)
check(r'(\e|x)*', False)
check(r'(\e|\e)*', True)
check(r'(a|x)(\e|\e)*(a|x)', True)
check(r'(a|x)(a|\e)*(a|x)', False)
check(r'((a|\e)(\e|x))*', True)
check(r'((a|\e)(a|x)(b|x)(\e|x))*', True)
check(r'((a|\e)(a|\e)(b|x)(\e|x))*', False)
check(r'(a|x)((a|\e)(\e|x))*(a|x)', True)
