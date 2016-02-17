#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context('lan_char, b')

def aut(e):
    return ctx.expression(e).automaton()

CHECK_EQUIV(aut(r'\e'), aut('a').ldiv(aut('a')))
CHECK_EQUIV(aut('b'), aut('a').ldiv(aut('ab')))
CHECK_EQUIV(aut(r'\e'), aut('b').ldiv(aut('a+b')))
CHECK_EQUIV(aut('a*'), aut('a').ldiv(aut('a*')))

exprs = [r'\e', '(a+b)*c', '(a*b+ac)*', '(a*b+c)*bba+a(b(c+d)*+a)']
exprs = [ctx.expression(e) for e in exprs]

for rhs in exprs:
    for lhs in exprs:
        CHECK_EQUIV((lhs.ldiv(rhs)).automaton(),
                lhs.automaton().ldiv(rhs.automaton()))
