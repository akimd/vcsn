#! /usr/bin/env python

from __future__ import print_function

import vcsn
from test import *

def load(file):
    return open(medir + "/" + file + ".gv").read().strip()

## -------------------- ##
## de bruijn/ladybird.  ##
## -------------------- ##

# check AUT EXP COMPLETE = False IS-DETERMINISTIC = False
# ------------------------------
def check(aut, exp, deterministic = False):
    CHECK_EQ(deterministic, aut.is_deterministic())
    det = aut.determinize()
    exp = load(exp)
    CHECK_EQ(exp, det)
    CHECK_EQ(True, det.is_deterministic())
    # Idempotence.
    CHECK_EQ(exp, det.determinize())

ctx = vcsn.context('lal_char(ab)_b')
check(ctx.de_bruijn(3), 'de-bruijn-3-det')
check(ctx.de_bruijn(8), 'de-bruijn-8-det')

ctx = vcsn.context('lal_char(abc)_b')
check(ctx.ladybird(4), 'ladybird-4-det')
check(ctx.ladybird(8), 'ladybird-8-det')


## ------------------------------- ##
## Simple deterministic automata.  ##
## ------------------------------- ##

for name in ['deterministic', 'epsilon', 'empty']:
    aut = vcsn.automaton(load(name))
    check(aut, name + "-det", True)
