#! /usr/bin/env python

import vcsn
from test import *

ctx_string = 'nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, z>'

ctx = vcsn.context(ctx_string)

def check(exp, daut, z=False):
    # It compares automata as strings, then if zpc is isomorphic to standard.
    # At last it tests if it is epsilon acyclic.
    e = ctx.expression(exp)
    zpc = e.zpc()
    zpt = zpc.trim().proper()
    std = e.standard()
    print(e)
    print('Check if zpc\'s daut expected format is correct.')
    CHECK_EQ(daut,
             zpc.format('daut'))
    if z == False:
        print('Check if trimed and propered zpc is isomorphic to standard.')
        CHECK_ISOMORPHIC(zpt, std)
    print('Check if zpc is epsilon acyclic.')
    CHECK_IS_EPS_ACYCLIC(zpc)

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
1 -> $''', True)

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
0 -> 1 "\\e"
0 -> 3 "\\e"
1 -> 2 "a"
2 -> 5 "\\e"
3 -> 4 "b"
4 -> 5 "\\e"
5 -> $''')

# Z: "a+\e"
check('a+\e',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, z>"
$ -> 0
0 -> $
0 -> 1 "\\e"
0 -> 3 "\\e"
1 -> 2 "a"
2 -> 5 "\\e"
4 -> 5 "\\e"
5 -> $''')

## ------- ##
## Z: mul. ##
## ------- ##

# Z: "ab"
check('ab',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, z>"
$ -> 0
0 -> 1 "\\e"
1 -> 2 "a"
2 -> 3 "\\e"
3 -> 4 "b"
4 -> 5 "\\e"
5 -> $''')

# Z: "a?bb"
check('a?bb',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, z>"
$ -> 0
0 -> 1 "\\e"
1 -> 2 "\\e"
1 -> 8 "\\e"
2 -> 3 "\\e"
2 -> 5 "\\e"
4 -> 7 "\\e"
5 -> 6 "a"
6 -> 7 "\\e"
7 -> 8 "\\e"
8 -> 9 "b"
9 -> 10 "\\e"
10 -> 11 "\\e"
11 -> 12 "b"
12 -> 13 "\\e"
13 -> $''')

## -------- ##
## Z: star. ##
## -------- ##

# Z: "a*"
check('a*',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, z>"
$ -> 0
0 -> $
0 -> 1 "\\e"
1 -> 2 "a"
2 -> 1 "\\e"
2 -> 3 "\\e"
3 -> $''')

## ----------- ##
## Z: lweight. ##
## ----------- ##

# Z: <x>a*
check('<x>a*',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, z>"
$ -> 0
0 -> $ "<x>"
0 -> 1 "<x>\\e"
1 -> 2 "a"
2 -> 1 "\\e"
2 -> 3 "\\e"
3 -> $''')

## ----------- ##
## Z: rweight. ##
## ----------- ##

# Z: (a?)<y>
check('(a?)<y>',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, z>"
$ -> 0
0 -> $ "<y>"
0 -> 1 "\\e"
0 -> 3 "\\e"
2 -> 5 "\\e"
3 -> 4 "a"
4 -> 5 "\\e"
5 -> $ "<y>"''')

## ---------------------- ##
## Z: lweight and rweight ##
## ---------------------- ##

# Z: (<x>a?)<y>
check('(<x>a?)<y>',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, z>"
$ -> 0
0 -> $ "<xy>"
0 -> 1 "<x>\\e"
0 -> 3 "<x>\\e"
2 -> 5 "\\e"
3 -> 4 "a"
4 -> 5 "\\e"
5 -> $ "<y>"''')
