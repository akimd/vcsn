#! /usr/bin/env python

import vcsn
from test import *

ctx_string = 'nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, z>'

ctx = vcsn.context(ctx_string)

def check(exp, daut, v='auto'):
    # It compares automata as strings, then if zpc is isomorphic to standard.
    # At last it tests if it is epsilon acyclic.
    e = ctx.expression(exp)
    zpc = e.zpc(v)
    std = e.standard()
    print(e)
    print('Check if zpc\'s daut expected format is correct.')
    CHECK_EQ(daut,
             zpc.format('daut'))
    print('Check if zpc is epsilon acyclic.')
    CHECK_IS_EPS_ACYCLIC(zpc)
    zpt = zpc.trim()
    if not zpt.is_empty():
        print('Check if trimed and propered zpc is isomorphic to standard.')
        CHECK_ISOMORPHIC(zpt.proper(), std)

def xfail(re, err = None):
    r = ctx.expression(re)
    XFAIL(lambda: r.zpc(), err)

# We don't support extended expressions.
xfail(r'a*&b*')
xfail(r'a:b')
xfail(r'a{c}')
xfail(r'a{\}b')
xfail(r'(ab){T}')

## --- ##
## Z.  ##
## --- ##

# Z: "\z".
check('\z',
'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, z>"
$ -> 0
1 -> $''')

# Z: "\e".
check('\e',
'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, z>"
$ -> 0
0 -> $
1 -> $''')

## ------- ##
## Z: sum. ##
## ------- ##

# Z: "a+b"
check('a+b',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, z>"
$ -> 0
0 -> 1 \e
0 -> 3 \e
1 -> 2 a
2 -> 5 \e
3 -> 4 b
4 -> 5 \e
5 -> $''')

# Z: "a+\e"
check('a+\e',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, z>"
$ -> 0
0 -> $
0 -> 1 \e
0 -> 3 \e
1 -> 2 a
2 -> 5 \e
4 -> 5 \e
5 -> $''')

## ------- ##
## Z: mul. ##
## ------- ##

# Z: "ab"
check('ab',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, z>"
$ -> 0
0 -> 1 \e
1 -> 2 a
2 -> 3 \e
3 -> 4 b
4 -> 5 \e
5 -> $''')

# Z: "a?bb"
check('a?bb',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, z>"
$ -> 0
0 -> 1 \e
1 -> 2 \e
1 -> 8 \e
2 -> 3 \e
2 -> 5 \e
4 -> 7 \e
5 -> 6 a
6 -> 7 \e
7 -> 8 \e
8 -> 9 b
9 -> 10 \e
10 -> 11 \e
11 -> 12 b
12 -> 13 \e
13 -> $''')

## -------- ##
## Z: star. ##
## -------- ##

# Z: "a*"
check('a*',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, z>"
$ -> 0
0 -> $
0 -> 1 \e
1 -> 2 a
2 -> 1 \e
2 -> 3 \e
3 -> $''')

## ----------- ##
## Z: lweight. ##
## ----------- ##

# Z: <x>a*
check('<x>a*',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, z>"
$ -> 0
0 -> $ <x>
0 -> 1 <x>\e
1 -> 2 a
2 -> 1 \e
2 -> 3 \e
3 -> $''')

## ----------- ##
## Z: rweight. ##
## ----------- ##

# Z: (a?)<y>
check('(a?)<y>',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, z>"
$ -> 0
0 -> $ <y>
0 -> 1 \e
0 -> 3 \e
2 -> 5 \e
3 -> 4 a
4 -> 5 \e
5 -> $ <y>''')

## ---------------------- ##
## Z: lweight and rweight ##
## ---------------------- ##

# Z: (<x>a?)<y>
check('(<x>a?)<y>',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, z>"
$ -> 0
0 -> $ <xy>
0 -> 1 <x>\e
0 -> 3 <x>\e
2 -> 5 \e
3 -> 4 a
4 -> 5 \e
5 -> $ <y>''')

## --------------- ##
## Z: sum and prod ##
## --------------- ##

#Z: (a?b?+c?)
check('(a?b?+c?)',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, z>"
$ -> 0
0 -> $ <<2>\e>
0 -> 1 \e
0 -> 15 \e
0 -> 17 \e
1 -> 2 \e
1 -> 8 \e
2 -> 3 \e
2 -> 5 \e
4 -> 7 \e
5 -> 6 a
6 -> 7 \e
7 -> 8 \e
7 -> 14 \e
8 -> 9 \e
8 -> 11 \e
10 -> 13 \e
11 -> 12 b
12 -> 13 \e
13 -> 14 \e
14 -> 19 \e
16 -> 19 \e
17 -> 18 c
18 -> 19 \e
19 -> $''')

## ----------- ##
## ZPC compact ##
## ----------- ##

## ------- ##
## Z: sum. ##
## ------- ##

# Z: "a+b"
check('a+b',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, z>"
$ -> 0
0 -> 1 a
0 -> 2 \e
1 -> 3 \e
2 -> 3 b
3 -> $''',
'compact')

# Z: "a+\e"
check('a+\e',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, z>"
$ -> 0
0 -> $
0 -> 1 a
0 -> 2 \e
1 -> 3 \e
3 -> $''',
'compact')

## ------- ##
## Z: mul. ##
## ------- ##

# Z: "ab"
check('ab',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, z>"
$ -> 0
0 -> 1 a
1 -> 2 \e
2 -> 3 b
3 -> $''',
'compact')

# Z: "a(a?b?)b"
check('a(a?b?)b',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, z>"
$ -> 0
0 -> 1 a
1 -> 2 \e
1 -> 5 \e
2 -> 4 \e
3 -> 5 \e
4 -> 5 a
5 -> 6 \e
5 -> 9 \e
6 -> 8 \e
7 -> 9 \e
8 -> 9 b
9 -> 10 \e
10 -> 11 b
11 -> $''',
'compact')


