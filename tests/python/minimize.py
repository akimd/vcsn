#! /usr/bin/env python

import vcsn

from test import *

def aut(file):
    return vcsn.automaton(filename = medir + "/" + file)

def file_to_string(file):
    return open(medir + "/" + file, "r").read().strip()

def check(algo, aut, exp):
    CHECK_EQ(exp, aut.minimize(algo))
    # Cominimize
    CHECK_EQ(aut.minimize(algo), aut.transpose().cominimize(algo).transpose())

def xfail(algo, aut):
    res = ''
    try:
        res = aut.minimize(algo)
    except RuntimeError:
        PASS()
    else:
        FAIL('did not raise an exception', str(res))

## Simple minimization test.  The example comes from the "Theorie des
## langages" course notes by Akim Demaille.  Automaton 4.23 at page 59,
## as of revision a0761d6.
a = aut("redundant.gv")
exp = file_to_string('redundant.exp.gv')
check('brzozowski', a, vcsn.automaton(exp))
check('moore',      a, exp)
check('signature',  a, exp)
check('weighted',   a, exp)

## An automaton equal to redundant.exp, with one transition removed.
a = aut('incomplete-non-trim.gv')
#xfail('brzozowski', a)
xfail('moore',      a)
xfail('signature',  a)
xfail('weighted',   a)

## An automaton equal to redundant.exp, with no initial states.  It
## must be minimized into an empty automaton.
a = aut('no-initial-states.gv')
z = file_to_string('no-initial-states.exp.gv')
check('brzozowski', a, z)
xfail('moore',      a)
xfail('signature',  a)
xfail('weighted',   a)

## An automaton equal to redundant.exp, with no final states.  It must
## be minimized into an empty automaton.
a = aut("no-final-states.gv")
check('brzozowski', a, z)
xfail('moore',      a)
xfail('signature',  a)
xfail('weighted',   a)

## Non-regression testcase: ensure that moore works and produces a
## correct result even with no non-final states.
all_states_final = vcsn.context('lal_char(a), b').ratexp('a*').standard()
check('moore', all_states_final, all_states_final.minimize('signature'))

## Minimize an intricate automaton into a linear one.
a = vcsn.context('lal_char(a-k), b') \
        .ratexp('[a-k]{10}') \
        .standard()
exp = file_to_string("intricate.exp.gv")
check('brzozowski', a, vcsn.automaton(exp))
check('moore',      a, exp)
check('signature',  a, exp)
check('weighted',   a, exp)

## Compute the quotient of a non-deterministic automaton, in this case
## yielding the minimal deterministic solution.
a = vcsn.context('lal_char(a), b') \
        .ratexp('a{2}*+a{2}*') \
        .standard()
exp = file_to_string("small-nfa.exp.gv")
check('brzozowski', a, vcsn.automaton(exp))
xfail('moore',      a)
check('signature',  a, exp)
check('weighted',   a, exp)

## A small weighted automaton.
a = aut("small-z.gv")
exp = file_to_string("small-z.exp.gv")
xfail('brzozowski', a)
xfail('moore',      a)
xfail('signature',  a)
check('weighted',   a, exp)

## Non-lal automata.
a = vcsn.context('law_char(a-c), b').ratexp("abc(bc)*+acb(bc)*").standard()
exp = file_to_string("nonlal.exp.gv")
check("signature", a, exp)
check("weighted",  a, exp)

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
a = vcsn.context('lal_char(ab), b').ratexp("a+ba").derived_term().strip()
check('brzozowski', a, a)
CHECK_ISOMORPHIC(a.minimize('moore'), a)
CHECK_ISOMORPHIC(a.minimize('signature'), a)

## Check minimization idempotency in the non-lal case as well.
a = vcsn.context('law_char(ab), b').ratexp("ab").standard()
CHECK_ISOMORPHIC(a.minimize('signature'), a)
CHECK_ISOMORPHIC(a.minimize('weighted'), a)
