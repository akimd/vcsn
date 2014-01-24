#! /usr/bin/env python

import inspect, os, sys
import vcsn

from test import *


# check EXPECTED AUT
# ------------------
# Check that AUTO.info() == EXPECTED.
def check(aut, exp):
    eff = aut.info()
    if eff == exp:
        PASS()
    else:
        if len(exp) != len(eff):
            FAIL("different dictionary sizes")
        for k in exp.keys():
            if exp[k] != eff[k]:
                FAIL("different " + k + ": " + exp[k] + " != " + eff[k])


ctx = vcsn.context('lal_char(a-z)_z')

check(ctx.ratexp("a+ab").standard(),
      {'is ambiguous': False,
       'is complete': False,
       'is deterministic': False,
       'is empty': False,
       'is eps-acyclic': True,
       'is normalized': False,
       'is proper': True,
       'is standard': True,
       'is trim': True,
       'is useless': False,
       'is valid': True,
       'number of accessible states': 4,
       'number of coaccessible states': 4,
       'number of deterministic states': 3,
       'number of eps transitions': 0,
       'number of final states': 2,
       'number of initial states': 1,
       'number of states': 4,
       'number of transitions': 3,
       'number of useful states': 4,
       'type': 'mutable_automaton<lal_char(abcdefghijklmnopqrstuvwxyz)_z>'})

check(ctx.ratexp("<1>a+(<2>b<3>*<4>)<5>").standard(),
      {'is ambiguous': False,
       'is complete': False,
       'is deterministic': True,
       'is empty': False,
       'is eps-acyclic': True,
       'is normalized': False,
       'is proper': True,
       'is standard': True,
       'is trim': True,
       'is useless': False,
       'is valid': True,
       'number of accessible states': 3,
       'number of coaccessible states': 3,
       'number of deterministic states': 3,
       'number of eps transitions': 0,
       'number of final states': 3,
       'number of initial states': 1,
       'number of states': 3,
       'number of transitions': 3,
       'number of useful states': 3,
       'type': 'mutable_automaton<lal_char(abcdefghijklmnopqrstuvwxyz)_z>'})

PLAN()
