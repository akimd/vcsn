#! /usr/bin/env python

import vcsn
from test import *

# check EXPECTED EXP1 EXP2
# ------------------------
#
# Check that are-equivalent(EXP1, EXP2) == EXPECTED.  Because
# is-equivalent on expressions uses is-equivalent on automata under
# the hood, this also checks the case of automata equivalence tests.
def check(exp, r1, r2):
    r1 = ctx.expression(r1, 'trivial')
    r2 = ctx.expression(r2, 'trivial')
    print('check({:u}, {:u})'.format(r1, r2))
    CHECK_EQ(exp, r1.is_equivalent(r2))
    # We use inductive because (i) it covers all the operators, (ii)
    # it introduces less equivalences than derived-term (which, for
    # instance, builds the same automata for `a` and `a+a`.
    a1 = r1.automaton('inductive')
    a2 = r2.automaton('inductive')
    CHECK_EQ(exp, a1.is_equivalent(a2))

ctx = vcsn.context('lal_char(ab), b')

check(True, r'\z', r'\z')
check(True, r'\e', r'\e')
check(True, 'a', 'a')
check(True, 'a+a', 'a')
check(True, 'a+b', 'b+a')
check(True, 'a+b+a', 'a+b')
check(True, 'ab', 'a.b')
check(True, 'a*', r'\e+(a.(a*))')
check(True, r'a*+a*+(\e+aa*)', 'a*')

check(False, r'\z', r'\e')
check(False, 'a', 'b')
check(False, 'ab', 'ba')
check(False, 'aa*', 'a*')

ctx = vcsn.context('lal_char(ab), z')
check(True, 'a+b+a', '<2>a+b')
check(True, 'a*+b+a*', '<2>a*+b')
check(False, 'a*+a*', 'a*')

ctx = vcsn.context('lat<lan_char, lan_char>,b')
a = ctx.expression('a|x')
# Don't expect more than the first error: clang produces more of them
# but not GCC.
XFAIL(lambda: a.is_equivalent(a),
      'determinize: requires free labelset')
