#! /usr/bin/env python

import vcsn
from test import *

## ---------- ##
## Automata.  ##
## ---------- ##

# We do not bind the compare functions, which are too precise
# currently: swapping two transitions make the automata different.
def check_lt(a1, a2):
    CHECK_EQ(True, a1.compare(a2) < 0)
    CHECK_EQ(True, a2.compare(a1) > 0)
    CHECK_EQ(0, a1.compare(a1))
    CHECK_EQ(0, a2.compare(a2))

ctx = vcsn.context('lal, q')
# First, comparison on source state numbers.
a1 = vcsn.automaton('''context = [...]? → ℤ
0 -> 0 a
0 -> 1 a
''')
a2 = vcsn.automaton('''context = [...]? → ℤ
0 -> 0 a
1 -> 0 a
''')
check_lt(a1, a2)

# Second, comparison on labels.
a1 = vcsn.automaton('''context = [...]? → ℤ
$ -> 0
0 -> 0 a
0 -> $''')
a2 = vcsn.automaton('''context = [...]? → ℤ
$ -> 0
0 -> 0 b
0 -> $''')
check_lt(a1, a2)

# Third, comparison on weights.
a1 = vcsn.automaton('''context = [...]? → ℤ
$ -> 0
0 -> 0 <1>a
0 -> $''')
a2 = vcsn.automaton('''context = [...]? → ℤ
$ -> 0
0 -> 0 <2>a
0 -> $''')
check_lt(a1, a2)

# Last, comparison on destination state number.
a1 = vcsn.automaton('''context = [...]? → ℤ
$ -> 0
0 -> 0 a
1 -> $''')
a2 = vcsn.automaton('''context = [...]? → ℤ
$ -> 0
0 -> 1 a
2 -> $''')
check_lt(a1, a2)


## ------------- ##
## Expressions.  ##
## ------------- ##

def check_lt_(e1, e2):
    CHECK_EQ(True, e1 < e2)
    CHECK_EQ(True, e1 <= e2)
    CHECK_EQ(True, e2 > e1)
    CHECK_EQ(True, e2 >= e1)
    CHECK_EQ(e1, e1)
    CHECK_EQ(e2, e2)
    CHECK_NE(e1, e2)
    CHECK_NE(e2, e1)

def check_lt(r1, r2):
    check_lt_(ctx.expression(r1), ctx.expression(r2))

ctx = vcsn.context('lal, q')
check_lt(r'\z', r'\e')
check_lt(r'\z', 'a')
check_lt(r'\e', 'a')
check_lt('a', 'b')
check_lt('a', 'a+b')

check_lt('<1/10>a', '<1/5>a')
check_lt('<-10>a', '<-5>a')
check_lt('<-10>a', '<10>a')

check_lt('a+b', 'a+b+c')
check_lt('a+b', 'ab')
check_lt('ab', 'abc')
check_lt('ab', 'a**')
check_lt('a*', 'a**')


## -------- ##
## Labels.  ##
## -------- ##

def check_lt(r1, r2):
    check_lt_(ctx.label(r1), ctx.label(r2))

ctx = vcsn.context('law, q')
# This is lexicographical order, not shortlex.
check_lt('', r'a')
check_lt('a', 'b')
check_lt('a', 'aa')
check_lt('aa', 'b')



## --------- ##
## Weights.  ##
## --------- ##

def check_lt(w1, w2):
    check_lt_(ctx.weight(w1), ctx.weight(w2))
    # Applies for weights in polynomials.
    p1 = '<{}>a'.format(w1)
    p2 = '<{}>a'.format(w2)
    # In the case of polynomials, the weight 0, being absorbant, is
    # actually smaller than any other polynomial.
    if w2 == '0':
        p1, p2 = p2, p1
    check_lt_(ctx.polynomial(p1), ctx.polynomial(p2))

ctx = vcsn.context('lal, q')
check_lt('0', '1')
check_lt('-1', '0')

check_lt('-1/2', '0')
check_lt('0', '1/2')
check_lt('1/2', '1')

check_lt('1/3', '1/2')
check_lt('1/2', '2/3')
