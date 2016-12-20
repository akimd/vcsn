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

ctx = vcsn.context('lal_char, q')
# First, comparison on source state numbers.
a1 = vcsn.automaton('''context = lal, z
0 -> 0 a
0 -> 1 a
''')
a2 = vcsn.automaton('''context = lal, z
0 -> 0 a
1 -> 0 a
''')
check_lt(a1, a2)

# Second, comparison on labels.
a1 = vcsn.automaton('''context = lal, z
$ -> 0
0 -> 0 a
0 -> $''')
a2 = vcsn.automaton('''context = lal, z
$ -> 0
0 -> 0 b
0 -> $''')
check_lt(a1, a2)

# Third, comparison on weights.
a1 = vcsn.automaton('''context = lal, z
$ -> 0
0 -> 0 <1>a
0 -> $''')
a2 = vcsn.automaton('''context = lal, z
$ -> 0
0 -> 0 <2>a
0 -> $''')
check_lt(a1, a2)

# Last, comparison on destination state number.
a1 = vcsn.automaton('''context = lal, z
$ -> 0
0 -> 0 a
1 -> $''')
a2 = vcsn.automaton('''context = lal, z
$ -> 0
0 -> 1 a
2 -> $''')
check_lt(a1, a2)


## ------------- ##
## Expressions.  ##
## ------------- ##

def check_lt(r1, r2):
    e1 = ctx.expression(r1)
    e2 = ctx.expression(r2)
    CHECK_EQ(True, e1 < e2)
    CHECK_EQ(True, e1 <= e2)
    CHECK_EQ(True, e2 > e1)
    CHECK_EQ(True, e2 >= e1)
    CHECK_EQ(e1, e1)
    CHECK_EQ(e2, e2)
    CHECK_NE(e1, e2)
    CHECK_NE(e2, e1)

ctx = vcsn.context('lal_char, q')
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
