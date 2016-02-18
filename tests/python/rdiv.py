#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context('lan_char, b')

def aut(e):
    return ctx.expression(e).automaton()

CHECK_EQUIV(aut(r'\e'), aut('a') / aut('a'))
CHECK_EQUIV(aut('a'), aut('ab') / aut('b'))
CHECK_EQUIV(aut(r'\e'), aut('a+b') / aut('b'))
CHECK_EQUIV(aut('a*'), aut('a*') / aut('a'))

CHECK((aut('ab') / aut('b')).type() == aut('a').type())

exprs = [r'\e', '(a+b)*c', '(a*b+ac)*', '(a*b+c)*bba+a(b(c+d)*+a)']
exprs = [ctx.expression(e) for e in exprs]

for rhs in exprs:
    for lhs in exprs:
        CHECK_EQUIV((lhs / rhs).automaton(),
                lhs.automaton() / rhs.automaton())
