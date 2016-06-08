#! /usr/bin/env python

import vcsn
from test import *

ctx_string = 'nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, q>'

ctx = vcsn.context(ctx_string)

def check(exp, daut, v='auto'):
    # It compares automata as strings, then if zpc is isomorphic to standard.
    # At last it tests if it is epsilon acyclic.
    print("Check: {}.zpc({})".format(exp, v))
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
        print('Check if trim and proper zpc is isomorphic to standard.')
        CHECK_ISOMORPHIC(zpt.proper(), std)

def xfail(re, err = None):
    r = ctx.expression(re)
    XFAIL(lambda: r.zpc(), err)

# We don't support extended expressions.
xfail(r'a*&b*')
xfail(r'a&:b')
xfail(r'a:b')
xfail(r'a{c}')
xfail(r'a{\}b')
xfail(r'(ab){T}')

## -------------- ##
## ZPC: regular.  ##
## -------------- ##

# "\z".
check(r'\z',
'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, q>"
$ -> 0
1 -> $''')

# "\e".
check(r'\e',
'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, q>"
$ -> 0
0 -> $
1 -> $''')

## ----- ##
## add.  ##
## ----- ##

# "a+b"
check('a+b',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, q>"
$ -> 0
0 -> 1 \e
0 -> 3 \e
1 -> 2 a
2 -> 5 \e
3 -> 4 b
4 -> 5 \e
5 -> $''')

# "\e+a"
check(r'\e+a',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, q>"
$ -> 0
0 -> $
0 -> 1 \e
0 -> 3 \e
2 -> 5 \e
3 -> 4 a
4 -> 5 \e
5 -> $''')

## ----- ##
## mul.  ##
## ----- ##

# "ab"
check('ab',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, q>"
$ -> 0
0 -> 1 \e
1 -> 2 a
2 -> 3 \e
3 -> 4 b
4 -> 5 \e
5 -> $''')

# "a?bb"
check('a?bb',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, q>"
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

## ------ ##
## star.  ##
## ------ ##

# "a*"
check('a*',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, q>"
$ -> 0
0 -> $
0 -> 1 \e
1 -> 2 a
2 -> 1 \e
2 -> 3 \e
3 -> $''')

## --------- ##
## lweight.  ##
## --------- ##

# <x>a*
check('<x>a*',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, q>"
$ -> 0
0 -> $ <x>
0 -> 1 <x>\e
1 -> 2 a
2 -> 1 \e
2 -> 3 \e
3 -> $''')

## --------- ##
## rweight.  ##
## --------- ##

# (a?)<y>
check('(a?)<y>',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, q>"
$ -> 0
0 -> $ <y>
0 -> 1 \e
0 -> 3 \e
2 -> 5 \e
3 -> 4 a
4 -> 5 \e
5 -> $ <y>''')

## ------------------- ##
## lweight and rweight ##
## ------------------- ##

# (<x>a?)<y>
check('(<x>a?)<y>',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, q>"
$ -> 0
0 -> $ <xy>
0 -> 1 <x>\e
0 -> 3 <x>\e
2 -> 5 \e
3 -> 4 a
4 -> 5 \e
5 -> $ <y>''')

## ------------ ##
## add and prod ##
## ------------ ##

# (a?b?+c?)
check('(a?b?+c?)',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, q>"
$ -> 0
0 -> $ <<2>\e>
0 -> 1 \e
0 -> 3 \e
0 -> 5 \e
2 -> 19 \e
3 -> 4 c
4 -> 19 \e
5 -> 6 \e
5 -> 12 \e
6 -> 7 \e
6 -> 9 \e
8 -> 11 \e
9 -> 10 a
10 -> 11 \e
11 -> 12 \e
11 -> 18 \e
12 -> 13 \e
12 -> 15 \e
14 -> 17 \e
15 -> 16 b
16 -> 17 \e
17 -> 18 \e
18 -> 19 \e
19 -> $''')

## ------------- ##
## ZPC compact.  ##
## ------------- ##

## ----- ##
## add.  ##
## ----- ##

# "a+b"
check('a+b',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, q>"
$ -> 0
0 -> 1 a
0 -> 2 \e
1 -> 3 \e
2 -> 3 b
3 -> $''',
'compact')

# "\e+a"
check(r'\e+a',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, q>"
$ -> 0
0 -> $
0 -> 2 \e
1 -> 3 \e
2 -> 3 a
3 -> $''',
'compact')

## ----- ##
## mul.  ##
## ----- ##

# "ab"
check('ab',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, q>"
$ -> 0
0 -> 1 a
1 -> 2 \e
2 -> 3 b
3 -> $''',
'compact')

# "a(a?b?)b"
check('a(a?b?)b',
r'''context = "nullableset<letterset<char_letters(abc)>>, seriesset<letterset<char_letters(xyz)>, q>"
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
