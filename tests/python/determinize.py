#! /usr/bin/env python

import vcsn
from test import *


# check AUT EXP ALGO = "auto" DETERMINISTIC = False
# -------------------------------------------------
def check(aut, expfile, algo="auto", deterministic=False):
    '''Check that `aut.determinize(algo)` results in the automaton
    `determinize.dir/EXP-det.gv`.'''

    print("check: {}, algo={}".format(expfile, algo))

    CHECK_EQ(deterministic, aut.is_deterministic())
    # Check value in property cache.
    CHECK_EQ(deterministic, aut.info('is deterministic', strict=False))

    t = aut.transpose()
    CHECK_EQ(deterministic, t.strip().is_codeterministic())
    # Check that property cache was invalidated.
    CHECK_EQ('N/A', t.info('is deterministic', strict=False))

    det = aut.determinize(algo)
    exp = metext(expfile + '-det', 'gv')
    CHECK_EQ(exp, det)

    # Check before calling is_deteministic() to ensure that it retrieves a
    # cached value.
    CHECK_EQ(True, det.info('is deterministic', strict=False))
    CHECK(det.is_deterministic())

    # Idempotence.
    idem = det.determinize(algo)
    CHECK_EQ(det, idem)
    CHECK_EQ(True, idem.info('is deterministic', strict=False))


    # Laziness.
    if algo != "boolean" and not aut.is_empty():
        CHECK_NE(exp, aut.determinize(algo, lazy=True))
        CHECK_EQ(exp, aut.determinize(algo, lazy=True).accessible())

    # Codeterminization.
    codet = aut.transpose().strip().determinize().transpose().strip()
    CHECK_EQ(aut.codeterminize(), codet)
    CHECK(codet.is_codeterministic())


## -------------------- ##
## de bruijn/ladybird.  ##
## -------------------- ##

ctx = vcsn.context('[ab] -> b')
check(ctx.de_bruijn(3), 'de-bruijn-3')
check(ctx.de_bruijn(8), 'de-bruijn-8')

ctx = vcsn.context('[abc] -> b')
check(ctx.ladybird(4), 'ladybird-4')
check(ctx.ladybird(8), 'ladybird-8')


## ------------------------------- ##
## Simple deterministic automata.  ##
## ------------------------------- ##

for name in ['deterministic', 'empty', 'epsilon']:
    aut = meaut(name, 'gv')
    check(aut, name, deterministic=True)
    check(aut, name, deterministic=True, algo='weighted')


## ------------------- ##
## Weighted automata.  ##
## ------------------- ##

for name in ['q', 'z', 'zmin']:
    aut = meaut(name, 'gv')
    check(aut, name)


## ----------------------------- ##
## Boolean automata (B and F2).  ##
## ----------------------------- ##

for name in ['b', 'f2']:
    aut = meaut(name, 'gv')
    for algo in ['auto', 'boolean', 'weighted']:
        check(aut, name, algo=algo)


## ------------- ##
## Empty state.  ##
## ------------- ##

# Check that we don't create the empty state when two paths end in the
# same state with opposite weights.
def null_state(weightset, weight):
    return vcsn.automaton('''
context = [...]? → {ws}
$ -> 0
0 -> 1 a
0 -> 2 <{w}>a
1 -> 3 b
2 -> 3 b'''.format(w=weight, ws=weightset))


def null_state_det(weightset, weight):
    return vcsn.automaton('''
digraph
{{
  vcsn_context = "[ab]? → {ws}"
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
    exp = null_state_det(ws, w)
    CHECK_EQ(exp, a.determinize(algo))
