#! /usr/bin/env python

import os
import vcsn
from test import *

# law -> Q.
c = vcsn.context('law_char, q')
p = c.polynomial('<2>\e+<3>a+<4>b+<5>abc+<6>abcd+<7>abdc')

a = p.trie()
CHECK_EQ(r'''context = "letterset<char_letters(abcd)>, q"
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


c = vcsn.context('law_char, q')
a = c.trie('series.txt')
# FIXME: the context is wrong (empty).
CHECK_EQ(r'''context = "letterset<char_letters()>, q"
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



c = vcsn.context('law_char, q')
p = c.polynomial('<2>\e+<3>a+<4>b+<5>cba+<6>dcba+<7>cdba')
a = p.cotrie()
CHECK_EQ(r'''context = "letterset<char_letters(abcd)>, q"
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


c = vcsn.context('law_char, q')
# FIXME: the context is wrong (empty).
a = c.cotrie('series.txt')
CHECK_EQ(r'''context = "letterset<char_letters()>, q"
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


# law x law -> Q.
c = vcsn.context('lat<law_char, law_char>, q')
p = c.polynomial('<1>one|un + <2>two|deux + <3>three|trois  + <4>four|quatre'
                 + ' + <14>forteen|quatorze + <40>forty|quarante')
CHECK_EQ(r'''digraph
{
  vcsn_context = "lat<nullableset<letterset<char_letters(efhnortuwy)>>, nullableset<letterset<char_letters(adeinoqrstuxz)>>>, q"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F3
    F7
    F13
    F19
    F24
    F29
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
    3
    4
    5
    6
    7
    8
    9
    10
    11
    12
    13
    14
    15
    16
    17
    18
    19
    20
    21
    22
    23
    24
    25
    26
    27
    28
    29
  }
  I0 -> 0
  0 -> 1 [label = "o|u"]
  0 -> 4 [label = "t|d"]
  0 -> 8 [label = "f|q"]
  0 -> 20 [label = "t|t"]
  1 -> 2 [label = "n|n"]
  2 -> 3 [label = "e|\\e"]
  3 -> F3
  4 -> 5 [label = "w|e"]
  5 -> 6 [label = "o|u"]
  6 -> 7 [label = "\\e|x"]
  7 -> F7 [label = "<2>"]
  8 -> 9 [label = "o|u"]
  9 -> 10 [label = "u|a"]
  9 -> 14 [label = "r|a"]
  10 -> 11 [label = "r|t"]
  11 -> 12 [label = "\\e|r"]
  12 -> 13 [label = "\\e|e"]
  13 -> F13 [label = "<4>"]
  14 -> 15 [label = "t|r"]
  14 -> 25 [label = "t|t"]
  15 -> 16 [label = "y|a"]
  16 -> 17 [label = "\\e|n"]
  17 -> 18 [label = "\\e|t"]
  18 -> 19 [label = "\\e|e"]
  19 -> F19 [label = "<40>"]
  20 -> 21 [label = "h|r"]
  21 -> 22 [label = "r|o"]
  22 -> 23 [label = "e|i"]
  23 -> 24 [label = "e|s"]
  24 -> F24 [label = "<3>"]
  25 -> 26 [label = "e|o"]
  26 -> 27 [label = "e|r"]
  27 -> 28 [label = "n|z"]
  28 -> 29 [label = "\\e|e"]
  29 -> F29 [label = "<14>"]
}''',
         p.trie())
