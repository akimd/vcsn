#! /usr/bin/env python

import vcsn
from test import *

aut1 = vcsn.automaton(r'''
context = [abcd]? → ℤmin
$ -> 0 <0>
0 -> 1 <1>a
0 -> 2 <2>a
1 -> 1 <3>b
1 -> 3 <5>c
2 -> 2 <3>b
2 -> 3 <6>d
3 -> $ <0>
''')
CHECK(aut1.has_twins_property())


aut2 = vcsn.automaton(r'''
context = [abcd]? → ℤmin
$ -> 0 <0>
0 -> 1 <1>a
0 -> 2 <2>a
1 -> 1 <3>b
1 -> 3 <5>c
2 -> 2 <4>b
2 -> 3 <6>d
3 -> $ <0>
''')
CHECK(not aut2.has_twins_property())


aut3 = vcsn.automaton('''context = [abcd]? → ℤmin
$ -> 0 <0>
0 -> 1 <2>a
0 -> 2 <3>a
1 -> 3 <3>b
1 -> 4 <2>c
2 -> 4 <5>d
2 -> 5 <3>b
3 -> 6 <6>c
4 -> $ <0>
5 -> 7 <6>c
6 -> 1 <9>d
7 -> 2 <9>d
''')
CHECK(aut3.has_twins_property())


aut4 = vcsn.automaton('''context = [abcd]? → ℤmin
$ -> 0 <0>
0 -> 1 <2>a
0 -> 2 <3>a
1 -> 3 <3>b
1 -> 4 <2>c
2 -> 4 <5>d
2 -> 5 <3>b
3 -> 6 <6>c
4 -> $ <0>
5 -> 7 <6>c
6 -> 1 <9>d
7 -> 2 <13>d
''')
CHECK(not aut4.has_twins_property())


aut5 = vcsn.automaton('''context = [abcd]? → ℚ
$ -> 0
0 -> 1 <2/3>a
0 -> 2 <3/5>a
1 -> 3 <4>b
1 -> 4 <2>c
2 -> 4 <7>d
2 -> 5 <2>b
3 -> 1 <6>d
4 -> $
5 -> 2 <12>d
''')
CHECK(aut5.has_twins_property())


aut6 = vcsn.automaton('''context = [abcd]? → ℚ
$ -> 0
0 -> 1 <2/3>a
0 -> 2 <3/5>a
1 -> 3 <4>b
1 -> 4 <2>c
2 -> 4 <7>d
2 -> 5 <2>b
3 -> 1 <6>d
4 -> $
5 -> 2 <17>d
''')
CHECK(not aut6.has_twins_property())


aut7 = vcsn.automaton('''context = [abc]? → ℤ
$ -> 0
0 -> 1 <3>a
0 -> 2 <5>a
1 -> $
1 -> 3 <3>b
2 -> $
2 -> 4 <2>b
3 -> 1 <2>a
4 -> 2 <3>a
''')
CHECK(aut7.has_twins_property())


aut8 = vcsn.automaton('''context = [abc]? → ℤ
$ -> 0
0 -> 1 <3>a
0 -> 2 <5>a
1 -> $
1 -> 3 <3>b
2 -> $
2 -> 4 <2>b
3 -> 1 <2>a
4 -> 2 <5>a
''')
CHECK(not aut8.has_twins_property())
