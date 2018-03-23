#! /usr/bin/env python

import vcsn

from test import *

algos = ['hopcroft', 'moore', 'signature', 'weighted']

def check(algo, aut, exp):
    if isinstance(algo, list):
        for a in algo:
            check(a, aut, exp)
    else:
        print('checking minimize with algorithm ', algo)
        CHECK_EQ(exp, aut.minimize(algo))
        # Check that repeated minimization still gives the same type of
        # automaton.  We don't want to get partition_automaton of
        # partition_automaton: one "layer" suffices.
        CHECK_EQ(exp, aut.minimize(algo).minimize(algo))

        # Cominimize.
        #
        # Do not work just on the transpose_automaton, to make sure it
        # works as expected for "forward" automata (which did have one
        # such bug!).  So copy the transposed automaton.
        t = aut.transpose().automaton(aut.context())
        if isinstance(exp, str):
            exp = vcsn.automaton(exp)
        CHECK_ISOMORPHIC(exp.transpose(), t.cominimize(algo))

def xfail(algo, aut):
    res = ''
    try:
        res = aut.minimize(algo)
    except RuntimeError:
        PASS()
    else:
        FAIL('did not raise an exception', str(res))

## Simple minimization test.  The example comes from the "Théorie des
## langages" lecture notes by François Yvon & Akim Demaille.
## Automaton 4.23 at page 59, as of revision a0761d6.
a = meaut('redundant.gv')
exp = metext('redundant.exp.gv')
check('brzozowski', a, vcsn.automaton(exp))
check(algos, a, exp)

## An automaton equal to redundant.exp, with one transition removed.
a = meaut('incomplete-non-trim.gv')
#xfail('brzozowski', a)
xfail('moore',      a)
xfail('signature',  a)
xfail('weighted',   a)

## An automaton equal to redundant.exp, with no initial states.  It
## must be minimized into an empty automaton.
a = meaut('no-initial-states.gv')
z = metext('no-initial-states.exp.gv')
check('brzozowski', a, z)
xfail('moore',      a)
xfail('signature',  a)
xfail('weighted',   a)

## An automaton equal to redundant.exp, with no final states.  It must
## be minimized into an empty automaton.
a = meaut('no-final-states.gv')
check('brzozowski', a, z)
xfail('moore',      a)
xfail('signature',  a)
xfail('weighted',   a)

## Non-regression testcase: ensure that moore works and produces a
## correct result even with no non-final states.
all_states_final = vcsn.context('[a] -> b').expression('a*').standard()
check('moore', all_states_final, all_states_final.minimize('signature'))

## Minimize an intricate automaton into a linear one.
a = vcsn.context('lal(a-k), b') \
        .expression('[a-k]{10}') \
        .standard()
exp = metext('intricate.exp.gv')
check('brzozowski', a, vcsn.automaton(exp))
check(algos, a, exp)

## Compute the quotient of a non-deterministic automaton, in this case
## yielding the minimal deterministic solution.
a = vcsn.context('[a] -> b') \
        .expression('a{2}*+a{2}*', 'trivial') \
        .standard()
exp = metext('small-nfa.exp.gv')
check('brzozowski', a, vcsn.automaton(exp))
xfail('moore',      a)
check('signature',  a, exp)
check('weighted',   a, exp)

## A small weighted automaton.
a = meaut('small-z.gv')
exp = metext('small-z.exp.gv')
xfail('brzozowski', a)
xfail('moore',      a)
xfail('signature',  a)
check('weighted',   a, exp)

## Non-lal automata.
a = vcsn.context('law(a-c), b').expression('abc(bc)*+acb(bc)*').standard()
exp = metext('nonlal.exp.gv')
check('signature', a, exp)
check('weighted',  a, exp)

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
a = vcsn.context('[ab] -> b').expression('a+ba').automaton()
check('brzozowski', a, a)
CHECK_ISOMORPHIC(a.minimize('moore'), a)
CHECK_ISOMORPHIC(a.minimize('signature'), a)

## Check minimization idempotency in the non-lal case as well.
a = vcsn.context('law(ab), b').expression('ab').standard()
CHECK_ISOMORPHIC(a.minimize('signature'), a)
CHECK_ISOMORPHIC(a.minimize('weighted'), a)
