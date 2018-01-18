#! /usr/bin/env python

import re

import vcsn
from test import *

def check(n, ctx):
    c = vcsn.context(ctx)
    print(c)
    CHECK_EQ(n, c.num_tapes())

check(0, 'lal, b')
check(1, 'lat<lal>, b')
check(2, 'lat<lal, lal>, b')
