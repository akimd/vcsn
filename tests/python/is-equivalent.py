#! /usr/bin/env python

import vcsn
from test import *

# check EXPECTED RAT1 RAT2
# ------------------------
#
# Check that are-equivalent(RAT1, RAT2) == EXPECTED.  Because
# is-equivalent on expressions uses is-equivalent on automata under
# the hood, this also checks the case of automata equivalence tests.
def check(exp, r1, r2):
    r1 = ctx.expression(r1)
    r2 = ctx.expression(r2)
    CHECK_EQ(exp, r1.is_equivalent(r2))

ctx = vcsn.context('lal_char(ab), b')

check(True, r'\z', r'\z')
check(True, r'\e', r'\e')
check(True, 'a', 'a')
check(True, 'ab', 'a.b')
check(True, 'a*', r'\e+(a.(a*))')
check(True, 'a+b', 'b+a')
check(True, r'a*+a*+(\e+aa*)', 'a*')

check(False, r'\z', r'\e')
check(False, 'a', 'b')
check(False, 'ab', 'ba')
check(False, 'aa*', 'a*')

ctx = vcsn.context('lal_char(ab), z')
check(True, 'a+b+a', '<2>a+b')
check(True, 'a*+b+a*', '<2>a*+b')

ctx = vcsn.context('lat<lan_char, lan_char>,b')
a = ctx.expression('a|x')
# Don't expect more than the first error: clang produces more of them
# but not GCC.
XFAIL(lambda: a.is_equivalent(a),
      'determinize: requires free labelset')
