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


## ---------------------------------------##
## Determinization of weighted automaton. ##
## ---------------------------------------##
def weighted_determinize_check(i, o, words = []):
    deter = i.determinize_weight()
    CHECK_EQ(True, deter.is_deterministic())
    CHECK_EQ(o, deter)
    oaut = vcsn.automaton(o)
    for w in words:
        CHECK_EQ(True, deter.eval(w) == oaut.eval(w))

b_aut_in = vcsn.automaton(load("weighted-b-aut-in"))
b_aut_out = load("weighted-b-aut-out")
weighted_determinize_check(b_aut_in, b_aut_out, ["baa", "aa"])

zmin_aut_in = vcsn.automaton(load("weighted-zmin-aut-in"))
zmin_aut_out = load("weighted-zmin-aut-out")
weighted_determinize_check(zmin_aut_in, zmin_aut_out, ["abc", "acd", "acb"])

q_aut_in = vcsn.automaton(load("weighted-q-aut-in"))
q_aut_out = load("weighted-q-aut-out")
weighted_determinize_check(q_aut_in, q_aut_out, ["ac", "ab"])
