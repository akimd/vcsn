#! /usr/bin/env python

from __future__ import print_function

import vcsn
from test import *

def load(file):
    return open(medir + "/" + file + ".gv").read().strip()

## -------------------- ##
## de bruijn/ladybird.  ##
## -------------------- ##

# check AUT EXP DETERMINISTIC = False
# -----------------------------------
def check(aut, expfile, algo = "auto", deterministic = False):
    print("check:", expfile)
    CHECK_EQ(deterministic, aut.is_deterministic())
    det = aut.determinize()
    exp = load(expfile)
    CHECK_EQ(exp, det)
    CHECK_EQ(True, det.is_deterministic())
    CHECK_EQ(det, det.determinize(algo))

    # Codeterminization.
    CHECK_EQ(aut.codeterminize(), aut.transpose().determinize().transpose())
    CHECK_EQ(True, aut.transpose().codeterminize().transpose().is_deterministic())
    CHECK_EQ(True, aut.codeterminize().transpose().is_deterministic())

ctx = vcsn.context('lal_char(ab)_b')
check(ctx.de_bruijn(3), 'de-bruijn-3-det')
check(ctx.de_bruijn(8), 'de-bruijn-8-det')

ctx = vcsn.context('lal_char(abc)_b')
check(ctx.ladybird(4), 'ladybird-4-det')
check(ctx.ladybird(8), 'ladybird-8-det')


## ------------------------------- ##
## Simple deterministic automata.  ##
## ------------------------------- ##

for name in ['deterministic', 'empty', 'epsilon']:
    aut = vcsn.automaton(load(name))
    check(aut, name + "-det", deterministic = True)
    check(aut, name + "-det", deterministic = True, algo = "weighted")


## -------------------------------------- ##
## Determinization of weighted automata.  ##
## -------------------------------------- ##
for name in ['b', 'q', 'z', 'zmin']:
    aut = vcsn.automaton(load(name))
    check(aut, name + '-det')
