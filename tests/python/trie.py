#! /usr/bin/env python

import os
import vcsn
from test import *

c = vcsn.context('law_char, z')
p = c.polynomial('<2>\e+<3>a+<4>b+<5>abc+<6>abcd+<7>abdc')

a = p.trie()
CHECK_EQ(r'''context = "letterset<char_letters(abcd)>, z"
$ -> 0
0 -> $ <2>
0 -> 1 a
0 -> 2 b
1 -> $ <3>
1 -> 3 b
2 -> $ <4>
3 -> 4 c
3 -> 6 d
4 -> $ <5>
4 -> 5 d
5 -> $ <6>
6 -> 7 c
7 -> $ <7>''', a.format('daut'))
CHECK(a.is_deterministic())
CHECK_EQ(p, a.shortest(100))

# Likewise, but via a file.
with open("series.txt", "w") as file:
    print(p.format("list"), file=file)


c = vcsn.context('law_char, z')
a = c.trie('series.txt')
# FIXME: the context is wrong (empty).
CHECK_EQ(r'''context = "letterset<char_letters()>, z"
$ -> 0
0 -> $ <2>
0 -> 1 a
0 -> 2 b
1 -> $ <3>
1 -> 3 b
2 -> $ <4>
3 -> 4 c
3 -> 6 d
4 -> $ <5>
4 -> 5 d
5 -> $ <6>
6 -> 7 c
7 -> $ <7>''', a.format('daut'))
CHECK(a.is_deterministic())
CHECK_EQ(p, a.shortest(100))



c = vcsn.context('law_char, z')
p = c.polynomial('<2>\e+<3>a+<4>b+<5>cba+<6>dcba+<7>cdba')
a = p.cotrie()
CHECK_EQ(r'''context = "letterset<char_letters(abcd)>, z"
$ -> 0 <2>
$ -> 1 <3>
$ -> 2 <4>
$ -> 4 <5>
$ -> 6 <7>
$ -> 7 <6>
0 -> $
1 -> 0 a
2 -> 0 b
3 -> 1 b
4 -> 3 c
5 -> 3 d
6 -> 5 c
7 -> 4 d''', a.format('daut'))
CHECK(a.is_codeterministic())
CHECK_EQ(p, a.shortest(100))

# Likewise, but via a file.
with open("series.txt", "w") as file:
    print(p.format("list"), file=file)


c = vcsn.context('law_char, z')
# FIXME: the context is wrong (empty).
a = c.cotrie('series.txt')
CHECK_EQ(r'''context = "letterset<char_letters()>, z"
$ -> 0 <2>
$ -> 1 <3>
$ -> 2 <4>
$ -> 4 <5>
$ -> 6 <7>
$ -> 7 <6>
0 -> $
1 -> 0 a
2 -> 0 b
3 -> 1 b
4 -> 3 c
5 -> 3 d
6 -> 5 c
7 -> 4 d''', a.format('daut'))
CHECK(a.is_codeterministic())
CHECK_EQ(p, a.shortest(100))

os.remove('series.txt')
