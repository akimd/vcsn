#! /usr/bin/env python

import inspect, os, sys
import vcsn

from test import *

srcdir = os.environ["srcdir"]
medir = sys.argv[0][:-3] + ".dir"

def aut(file):
    return vcsn.automaton.load(medir + "/" + file)

def check(algo, aut, exp):
    eff = aut.minimize(algo)
    if eff == exp:
        PASS()
    else:
        FAIL(str(exp) + " != " + str(eff))

def xfail(algo, aut):
    res = ''
    try:
        res = aut.minimize(algo)
    except RuntimeError:
        PASS()
    else:
        FAIL('did not raise an exception', res)


## Simple minimization test.  The example comes from the "Theorie des
## langages" course notes by Akim Demaille.  Automaton 4.23 at page 59,
## as of revision a0761d6.
redundant = aut("redundant.gv")
redundant_exp = aut("redundant.exp.gv")
check('moore', redundant, redundant_exp)
check('signature', redundant, redundant_exp)

## An automaton equal to redundant.exp, with one transition removed.
a = aut('incomplete-non-trim.gv')
xfail('moore', a)
xfail('signature', a)

## An automaton equal to redundant.exp, with no initial states.  It
## must be minimized into an empty automaton.
a = aut('no-initial-states.gv')
xfail('moore', a)
xfail('signature', a)

## An automaton equal to redundant.exp, with no final states.  It must
## be minimized into an empty automaton.
a = aut("no-final-states.gv")
xfail('moore', a)
xfail('signature', a)

## Minimize an intricate automaton into a linear one.
intricate = vcsn.context('lal_char(abcdefghijk)_b') \
                .ratexp('(a+b+c+d+e+f+g+h+i+j+k){10}') \
                .standard()
intricate_exp = aut("intricate.exp.gv")
check('moore', intricate, intricate_exp)
check('signature', intricate, intricate_exp)

## Compute the quotient of a non-deterministic automaton, in this case
## yielding the minimal deterministic solution.
smallnfa = vcsn.context('lal_char(a)_b') \
               .ratexp('a{2}*+a{2}*') \
               .standard()
smallnfa_exp = aut("small-nfa.exp.gv")
xfail('moore', smallnfa)
check('signature', smallnfa, smallnfa_exp)

PLAN()
