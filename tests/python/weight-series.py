#! /usr/bin/env python

import vcsn
from test import *

c = vcsn.context('lal, q')

def check(e, w):
    e = c.expression(e)
    a = e.automaton()
    CHECK_EQ(w, a.weight_series())

check('a', '1')
check('a+b', '2')
check('<3>a+<2>b', '5')
check('(<1/2>a)*', '2')
