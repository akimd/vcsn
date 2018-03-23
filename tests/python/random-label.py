#! /usr/bin/env python

import vcsn
from test import *

## --------------- ##
## Random Labels.  ##
## --------------- ##

def check(ctx, ref, params):
    CHECK_EQ(ctx.label(ref), ctx.random_label(params))

ctx = vcsn.context('[abc] -> q')

for i in range(10):
    l = ctx.random_label('\e=1')
    CHECK_EQ('\e', l)

for i in range(10):
    l = ctx.random_label('\e=0')
    CHECK_NE('\e', l)
