#! /usr/bin/env python

import vcsn
from test import *

# Not deterministic, yet not ambiguous.
a = vcsn.automaton('''
context = [ab]? → 𝔹
$ -> 0
0 -> 1 a
0 -> 2 a
1 -> $
''')
CHECK(not a.is_ambiguous())
XFAIL(lambda: a.ambiguous_word(), "automaton is unambiguous")
CHECK(not a.is_deterministic())

# Not deterministic, and ambiguous.
a = vcsn.automaton('''
context = [ab]? → 𝔹
$ -> 0
0 -> 1 a
0 -> 2 a
1 -> $
2 -> $
''')
CHECK(a.is_ambiguous())
CHECK_EQ("a", a.ambiguous_word())
CHECK(not a.is_deterministic())

# Check the ambiguous word for a not so trivial case.
CHECK_EQ('introABCDEFoutro',
         vcsn.B.expression('intro (ABC + DEF + AB + CD + E + F)* outro').standard()
         .ambiguous_word())
# Another type of ambiguity.
CHECK_EQ('introAoutro',
         vcsn.B.expression('intro A{+}{+} outro').thompson()
         .ambiguous_word())

# Test ambiguous word on automata with two ambiguous parts. Hence,
# test monomial_path with states different from pre and post.
b = vcsn.automaton('''
context = [ab]? → 𝔹
$ -> 0
$ -> 1
0 -> 2 a
1 -> 2 a
2 -> $
''')
CHECK(b.is_ambiguous())
CHECK((b*a).is_ambiguous())
CHECK((a*b).is_ambiguous())
CHECK_EQ("a", b.ambiguous_word())
CHECK_EQ("aa", (b*a).ambiguous_word())
CHECK_EQ("aa", (a*b).proper().ambiguous_word())

# Likewise, but with a non-commutative product.
a = vcsn.automaton('''
context = [ab]? → RatE[[xy]? → 𝔹]
$ -> 0
0 -> 1 <x>a
0 -> 2 <y>a
1 -> $
2 -> $
''')
CHECK(a.is_ambiguous())
CHECK_EQ("a", a.ambiguous_word())
CHECK(not a.is_deterministic())

# deterministic, and unambiguous (obviously).
a = vcsn.automaton('''
context = [ab]? → 𝔹
$ -> 0
0 -> 1 a
0 -> 2 b
1 -> $
2 -> $
''')
CHECK(not a.is_ambiguous())
XFAIL(lambda: a.ambiguous_word(), "automaton is unambiguous")
CHECK(a.is_deterministic())

# Retrieve shortest word, not lightest word.
# When using lightest to retrieve the shortest ambiguous word, ambiguous_word
# would return abb instead of ab.
a = vcsn.automaton('''
context = [ab]? → ℕmin
$ -> 0 <0>
0 -> 1 <0>a
0 -> 2 <0>a
1 -> 1 <0>b
1 -> 3 <4>b
2 -> 4 <0>b
2 -> 5 <4>b
3 -> $ <0>
4 -> 6 <0>b
5 -> $ <0>
6 -> $ <0>
''')
CHECK(a.is_ambiguous())
CHECK_EQ("ab", a.ambiguous_word())
CHECK(not a.is_deterministic())

## -------------------- ##
## is_cycle_ambiguous.  ##
## -------------------- ##

ctx = vcsn.context("[abc] -> b")

aut1 = vcsn.automaton('''context = [abc]? → 𝔹
$ -> 0
0 -> 1 a
0 -> 2 a
1 -> 0 b
1 -> 3 b
2 -> 1 c
2 -> 2 b
3 -> $
3 -> 1 c
''')
CHECK(aut1.is_cycle_ambiguous())

e1 = "((abc)*){5}abc" + format(aut1.expression())
aut = ctx.expression(e1).derived_term()
CHECK(aut.is_cycle_ambiguous())


aut2 = vcsn.automaton('''context = [abc]? → 𝔹
$ -> 0
0 -> 1 a
0 -> 2 a
1 -> 0 b
1 -> 3 b
2 -> 1 c
2 -> 2 b
3 -> $
3 -> 1 b
''')
CHECK(not aut2.is_cycle_ambiguous())

e2 = "((abc)*){5}abc" + format(aut2.expression())
aut = ctx.expression(e2).derived_term()
CHECK(not aut.is_cycle_ambiguous())


aut = vcsn.automaton('''
context = [abc]? → 𝔹
$ -> 0
0 -> 1 c
1 -> 2 a
2 -> 3 b
2 -> 4 b
3 -> 5 c
3 -> 6 c
4 -> 7 c
5 -> 2 a
6 -> 0 b
7 -> $
7 -> 2 a''')
CHECK(aut.is_cycle_ambiguous())


# Regression: broke when we fused lan and lal.
a = ctx.expression('a{4}*+a{4}*', "associative").standard()
CHECK(not a.is_cycle_ambiguous())


CHECK(ctx.ladybird(5).is_cycle_ambiguous())
CHECK(not ctx.de_bruijn(5).is_cycle_ambiguous())
CHECK(ctx.ladybird(20).is_cycle_ambiguous())
CHECK(not ctx.de_bruijn(20).is_cycle_ambiguous())
