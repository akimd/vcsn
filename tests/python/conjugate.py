#! /usr/bin/env python

import random
import vcsn
from test import *

def load(fname):
    return open(medir + '/' + fname).read().strip()

def check(exp, input):
    CHECK_EQ(exp, input.conjugate())


def subset(aut1, aut2):
    return (aut1 % aut2).is_useless()

def list_shortest(aut, n):
    l = str(aut.shortest(n)).split(' + ')
    return [w for w in l if w != r'\\e']


def check_conj(aut, conj):
    aut = aut.proper()
    conj = conj.proper()
    CHECK(subset(aut, conj))

    uv = random.choice(list_shortest(aut, 20)[10:])
    # Check that a random rotation of uv is accepted by conj
    if (len(uv) >= 2):
        i = random.randint(1, len(uv) - 1)
        vu = uv[i:] + uv[:i]
        print(vu)
        CHECK(conj(vu))

    vu = random.choice(list_shortest(conj, 20))
    ok = False
    # At least one rotation of vu is accepted by the original automaton
    for i in range(len(vu)):
        uv = vu[i:] + vu[:i]
        if (aut(uv)):
            ok = True
    CHECK(ok)

# Make random number generation deterministic
random.seed(0)

ctx = vcsn.context('lan_char, b')

# Known results
abc = ctx.expression('abc').automaton()
check(load('abc-conjug.gv'), abc)

ab_ac = ctx.expression('ab+ac').automaton()
check(load('ab-ac-conjug.gv'), ab_ac)

# Arbitrary expressions
for expr in ['(ab)*(c+d)', '(a(b+c)*de+f)']:
    aut = ctx.expression(expr).automaton()
    conj = aut.conjugate()
    check_conj(aut, conj)
