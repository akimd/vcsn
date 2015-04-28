#! /usr/bin/env python

import vcsn
from test import *


def load(file):
    return open(medir + "/" + file + ".gv").read().strip()

# check AUT EXP ALGO = "auto" DETERMINISTIC = False
# -------------------------------------------------
def check(aut, expfile, algo="auto", deterministic=False):
    print("check: {}, algo={}".format(expfile, algo))
    CHECK_EQ(deterministic, aut.is_deterministic())
    CHECK_EQ(deterministic, aut.transpose().strip().is_codeterministic())

    det = aut.determinize()
    exp = load(expfile)
    CHECK_EQ(exp, det)
    CHECK(det.is_deterministic())
    # Idempotence.
    CHECK_EQ(det, det.determinize(algo))

    # Codeterminization.
    codet = aut.transpose().strip().determinize().transpose().strip()
    CHECK_EQ(aut.codeterminize(), codet)
    CHECK(codet.is_codeterministic())


## -------------------- ##
## de bruijn/ladybird.  ##
## -------------------- ##

ctx = vcsn.context('lal_char(ab), b')
check(ctx.de_bruijn(3), 'de-bruijn-3-det')
check(ctx.de_bruijn(8), 'de-bruijn-8-det')

ctx = vcsn.context('lal_char(abc), b')
check(ctx.ladybird(4), 'ladybird-4-det')
check(ctx.ladybird(8), 'ladybird-8-det')


## ------------------------------- ##
## Simple deterministic automata.  ##
## ------------------------------- ##

for name in ['deterministic', 'empty', 'epsilon']:
    aut = vcsn.automaton(load(name))
    check(aut, name + "-det", deterministic=True)
    check(aut, name + "-det", deterministic=True, algo='weighted')


## ------------------- ##
## Weighted automata.  ##
## ------------------- ##

for name in ['q', 'z', 'zmin']:
    aut = vcsn.automaton(load(name))
    check(aut, name + '-det')


## ----------------------------- ##
## Boolean automata (B and F2).  ##
## ----------------------------- ##

for name in ['b', 'f2']:
    for algo in ['auto', 'boolean', 'weighted']:
        aut = vcsn.automaton(load(name))
        check(aut, name + '-det', algo=algo)
