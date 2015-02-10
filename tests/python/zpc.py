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
    CHECK_EQ('''context = "''' + ctx_string + '''"''' + daut,
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
'''
$ -> 0
1 -> $''', True)

# Z: "\e".
check('\e',
'''
$ -> 0
0 -> $
1 -> $''')

## ------- ##
## Z: sum. ##
## ------- ##

# Z: "a+b"
check('a+b',
r'''
$ -> 0
0 -> 2 "\\e"
0 -> 4 "\\e"
1 -> $
2 -> 3 "a"
3 -> 1 "\\e"
4 -> 5 "b"
5 -> 1 "\\e"''')

# Z: "a+\e"
check('a+\e',
r'''
$ -> 0
0 -> $
0 -> 2 "\\e"
0 -> 4 "\\e"
1 -> $
2 -> 3 "a"
3 -> 1 "\\e"
5 -> 1 "\\e"''')

## ------- ##
## Z: mul. ##
## ------- ##

# Z: "ab"
check('ab',
r'''
$ -> 2
0 -> 1 "a"
1 -> 4 "\\e"
2 -> 0 "\\e"
3 -> $
4 -> 5 "b"
5 -> 3 "\\e"''')

# Z: "a?bb"
check('a?bb',
r'''
$ -> 10
0 -> 2 "\\e"
0 -> 4 "\\e"
1 -> 8 "\\e"
3 -> 1 "\\e"
4 -> 5 "a"
5 -> 1 "\\e"
6 -> 0 "\\e"
6 -> 8 "\\e"
7 -> 12 "\\e"
8 -> 9 "b"
9 -> 7 "\\e"
10 -> 6 "\\e"
11 -> $
12 -> 13 "b"
13 -> 11 "\\e"''')

## -------- ##
## Z: star. ##
## -------- ##

# Z: "a*"
check('a*',
r'''
$ -> 0
0 -> $
0 -> 2 "\\e"
1 -> $
2 -> 3 "a"
3 -> 1 "\\e"
3 -> 2 "\\e"''')

## ----------- ##
## Z: lweight. ##
## ----------- ##

# Z: <x>a*
check('<x>a*',
r'''
$ -> 0
0 -> $ "<x>"
0 -> 2 "<x>\\e"
1 -> $
2 -> 3 "a"
3 -> 1 "\\e"
3 -> 2 "\\e"''')

## ----------- ##
## Z: rweight. ##
## ----------- ##

# Z: (a?)<y>
check('(a?)<y>',
r'''
$ -> 0
0 -> $ "<y>"
0 -> 2 "\\e"
0 -> 4 "\\e"
1 -> $ "<y>"
3 -> 1 "\\e"
4 -> 5 "a"
5 -> 1 "\\e"''')

## ---------------------- ##
## Z: lweight and rweight ##
## ---------------------- ##

# Z: (<x>a?)<y>
check('(<x>a?)<y>',
r'''
$ -> 0
0 -> $ "<xy>"
0 -> 2 "<x>\\e"
0 -> 4 "<x>\\e"
1 -> $ "<y>"
3 -> 1 "\\e"
4 -> 5 "a"
5 -> 1 "\\e"''')
