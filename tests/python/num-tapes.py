#! /usr/bin/env python

import re

import vcsn
from test import *

def check(n, ctx):
    c = vcsn.context(ctx)
    print(c)
    CHECK_EQ(n, c.num_tapes())

check(0, '[...] -> B')
# check(1, 'lat<[...]>, b')
check(2, '[...] x [...] -> b')
