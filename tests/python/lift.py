#! /usr/bin/env python

import vcsn
from test import *

# Check lift result and cached value for is_proper.
def check_lift(exp, aut):
    CHECK_EQ(exp, aut.format('daut'))
    CHECK_EQ(False, aut.info('is proper'))

## ---------- ##
## Automata.  ##
## ---------- ##

l4 = vcsn.context('[abc] -> b').ladybird(4)
check_lift('''\
context = {ε} → RatE[[abc]? → 𝔹]
$ -> 0
0 -> $
0 -> 1 <a>
1 -> 0 <c>
1 -> 1 <b+c>
1 -> 2 <a>
2 -> 0 <c>
2 -> 2 <b+c>
2 -> 3 <a>
3 -> 0 <a+c>
3 -> 3 <b+c>''',
         l4.lift())


## --------------- ##
## Lift(tape...).  ##
## --------------- ##

c = vcsn.context('[abc] x [def] x [ghi]* -> q')
a = c.expression("(a|d|gh)<2>").standard()

# lift(0).
aref = '''context = [def]? × [ghi]* → RatE[[abc]? → ℚ]
$ -> 0
0 -> 1 <<2>(a)>d|gh
1 -> $'''
check_lift(aref, a.lift(0))

# lift(1).
aref = '''context = [abc]? × [ghi]* → RatE[[def]? → ℚ]
$ -> 0
0 -> 1 <<2>(d)>a|gh
1 -> $'''
check_lift(aref, a.lift(1))

# lift(1, 2).
aref = '''context = [abc]? → RatE[[def]? × [ghi]* → ℚ]
$ -> 0
0 -> 1 <<2>(d|gh)>a
1 -> $'''
check_lift(aref, a.lift(1, 2))
check_lift(aref, a.lift([1, 2]))

## ------------------ ##
## lift(expression).  ##
## ------------------ ##

CHECK_EQ(r'''<abc>\e''',
         vcsn.context('[abc] -> b').expression('abc').lift())
CHECK_EQ(r'''<<2>(abc)>\e''',
         vcsn.context('[abc] -> z').expression('<2>abc').lift())
