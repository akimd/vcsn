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


## ------------- ##
## Empty state.  ##
## ------------- ##

# Check that we don't create the empty state when two paths ends in
# the same state with opposite weights.
def null_state(weightset, weight):
    return vcsn.automaton('''
context = "lal_char, {ws}"
$ -> 0
0 -> 1 a
0 -> 2 <{w}>a
1 -> 3 b
2 -> 3 b'''.format(w=weight,ws=weightset))


def null_state_det(weightset, weight):
    return vcsn.automaton('''
digraph
{{
  vcsn_context = "letterset<char_letters(ab)>, {ws}"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {{
    node [shape = point, width = 0]
    I0
  }}
  I0 -> 0 [color = DimGray]
  0 -> 1 [label = "a", color = DimGray]
}}'''.format(ws=ws, w=w))


for ws, w, algo in [('f2', '1', 'boolean'),
                    ('f2', '1', 'weighted'),
                    ('q', '-1', 'weighted')]:
    a = null_state(ws, w)
    exp = null_state_det(ws, w).strip()
    CHECK_EQ(exp, a.determinize(algo))
