#! /usr/bin/env python

import vcsn

from test import *

def aut(file):
    return vcsn.automaton.load(medir + "/" + file)

def check(algo, aut, exp):
    CHECK_EQ(exp, aut.minimize(algo))

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
check('brzozowski', redundant, redundant_exp)
check('moore',      redundant, redundant_exp)
check('signature',  redundant, redundant_exp)
check('weighted',   redundant, redundant_exp)

## An automaton equal to redundant.exp, with one transition removed.
a = aut('incomplete-non-trim.gv')
#xfail('brzozowski', a)
xfail('moore',      a)
xfail('signature',  a)
xfail('weighted',   a)

## An automaton equal to redundant.exp, with no initial states.  It
## must be minimized into an empty automaton.
a = aut('no-initial-states.gv')
#xfail('brzozowski', a)
xfail('moore',      a)
xfail('signature',  a)
xfail('weighted',   a)

## An automaton equal to redundant.exp, with no final states.  It must
## be minimized into an empty automaton.
a = aut("no-final-states.gv")
#xfail('brzozowski', a)
xfail('moore',      a)
xfail('signature',  a)
xfail('weighted',   a)

## Non-regression testcase: ensure that moore works and produces a
## correct result even with no non-final states.
all_states_final = vcsn.context('lal_char(a)_b').ratexp('a*').standard()
check('moore', all_states_final, all_states_final.minimize('signature'))

## Minimize an intricate automaton into a linear one.
intricate = vcsn.context('lal_char(a-k)_b') \
                .ratexp('[a-k]{10}') \
                .standard()
intricate_exp = aut("intricate.exp.gv")
check('brzozowski', intricate, intricate_exp)
check('moore',      intricate, intricate_exp)
check('signature',  intricate, intricate_exp)
check('weighted',   intricate, intricate_exp)

## Compute the quotient of a non-deterministic automaton, in this case
## yielding the minimal deterministic solution.
smallnfa = vcsn.context('lal_char(a)_b') \
               .ratexp('a{2}*+a{2}*') \
               .standard()
smallnfa_exp = aut("small-nfa.exp.gv")
check('brzozowski', smallnfa, smallnfa_exp)
xfail('moore',      smallnfa)
check('signature',  smallnfa, smallnfa_exp)
check('weighted',   smallnfa, smallnfa_exp)

## A small weighted automaton.
smallweighted = aut("small-weighted.gv")
smallweighted_exp = aut("small-weighted.exp.gv")
xfail('brzozowski', smallweighted)
xfail('moore',      smallweighted)
xfail('signature',  smallweighted)
check('weighted',   smallweighted, smallweighted_exp)

## Non-lal automata.
nonlal = \
  vcsn.context('law_char(a-c)_b').ratexp("abc(bc)*+acb(bc)*").standard().strip()
check("signature", nonlal, aut("nonlal.exp.gv"))
check("weighted",  nonlal, aut("nonlal.exp.gv"))

## An already-minimal automaton.  This used to fail with Moore,
## because of a subtly wrong optimization attempt in
## vcsn/algos/minimize.hh.  The idea was to never store invalid_class
## as a key in target_class_to_c_states, so as to avoid some hash
## accesses in the (common) case of some state having no
## out-transition with some label, as it happens in incomplete
## automata.  The optimization attempt consisted in setting a Boolean
## flag if the state being considered had no out-transition with the
## label being considered; when deciding whether to split a class,
## instead of the current test (2 <= target_class_to_c_states.size()),
## we used to test (2 <= (target_class_to_c_states.size() + flag)); at
## that point, however, it was possible to lose track of the state
## with no out-transitions, which was not in target_class_to_c_states.
## It remained associated to its old class identifier in
## state_to_class, which in the mean time would come to identify some
## subset of its old value.
a = vcsn.context('lal_char(ab)_b').ratexp("a+ba").derived_term().strip()
check('brzozowski', a, a)
check('moore',      a, a)
check('signature',  a, a)

## Check minimization idempotency in the non-lal case as well.
a = vcsn.context('law_char(ab)_b').ratexp("ab").standard()
check('signature',  a, a)
check('weighted',   a, a)
