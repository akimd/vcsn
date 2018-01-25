#! /usr/bin/env python

import os
import vcsn
from test import *

# In law, when reading the monomial `a|b` (yes, `|` is not
# escaped), we looped for ever: the `a` was read by
# setalpha::get_word, which then returned, and then
# polynomialset::conv_label repeatedly called get_word on `|b`, which
# endlessly returned the empty word, refusing to pass the `|`.
#
# Make sure we catch this.
with open('input.txt', 'w') as f:
    print('a|b', file=f)
XFAIL(lambda: vcsn.B.trie(filename='input.txt', format='monomials'))
os.remove('input.txt')

## ---------- ##
## law -> Q.  ##
## ---------- ##

def trie(algo, *args, **kwargs):
    if algo == 'trie':
        return args[0].trie(*args[1:], **kwargs)
    elif algo == 'cotrie':
        return args[0].cotrie(*args[1:], **kwargs)
    else:
        raise RuntimeError('invalid algo: ', algo)

def check(algo, ctx, polynomial, exp):
    print(algo, ctx, polynomial)

    # Starting from a polynomial.
    c = vcsn.context(ctx)
    p = c.polynomial(polynomial)
    a = trie(algo, p)
    CHECK_EQ(exp, a.format('daut'))

    # Check cached value for 'is deterministic'.
    if algo == 'trie':
        CHECK_EQ(True, a.info('is deterministic'))

    if ctx.startswith('lal'):
        CHECK(a.is_deterministic() if algo == 'trie' else a.is_codeterministic())
    CHECK_EQ(p, a.shortest(100))

    # Likewise, but via a string.
    c = vcsn.context(ctx)
    a = trie(algo, c, data=p.format('list'))
    CHECK_EQ(exp, a.format('daut'))

    if algo == 'trie':
        CHECK_EQ(True, a.info('is deterministic'))

    # Likewise, but via a file.
    with open('series.txt', 'w') as file:
        print(p.format('list'), file=file)
    c = vcsn.context(ctx)
    a = trie(algo, c, filename='series.txt')
    os.remove('series.txt')
    CHECK_EQ(exp, a.format('daut'))

    if algo == 'trie':
        CHECK_EQ(True, a.info('is deterministic'))


check('trie',
      'law, q',
      r'<2>\e+<3>a+<4>b+<5>d+<6>abc+<7>abcd+<8>abdc',
      r'''context = [abcd...]? → ℚ
$ -> 0
0 -> $ <2>
0 -> 1 a
0 -> 2 b
0 -> 3 d
1 -> $ <3>
1 -> 4 b
2 -> $ <4>
3 -> $ <5>
4 -> 5 c
4 -> 7 d
5 -> $ <6>
5 -> 6 d
6 -> $ <7>
7 -> 8 c
8 -> $ <8>''')


check('cotrie',
      'law, q',
      r'<2>\e+<3>a+<4>b+<5>cba+<6>dcba+<7>cdba',
      r'''context = [abcd...]? → ℚ
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
7 -> 4 d''')


## ---------------- ##
## law x law -> Q.  ##
## ---------------- ##

check('trie',
      'lat<law, law>, q',
      '<1>one|un + <2>two|deux + <3>three|trois  + <4>four|quatre'
      + ' + <14>forteen|quatorze + <40>forty|quarante',
      r'''context = [efhnortuwy...]? × [adeinoqrstuxz...]? → ℚ
$ -> 0
0 -> 1 o|u
0 -> 4 t|d
0 -> 8 t|t
0 -> 13 f|q
1 -> 2 n|n
2 -> 3 e|\e
3 -> $
4 -> 5 w|e
5 -> 6 o|u
6 -> 7 \e|x
7 -> $ <2>
8 -> 9 h|r
9 -> 10 r|o
10 -> 11 e|i
11 -> 12 e|s
12 -> $ <3>
13 -> 14 o|u
14 -> 15 u|a
14 -> 19 r|a
15 -> 16 r|t
16 -> 17 \e|r
17 -> 18 \e|e
18 -> $ <4>
19 -> 20 t|r
19 -> 25 t|t
20 -> 21 y|a
21 -> 22 \e|n
22 -> 23 \e|t
23 -> 24 \e|e
24 -> $ <40>
25 -> 26 e|o
26 -> 27 e|r
27 -> 28 n|z
28 -> 29 \e|e
29 -> $ <14>''')


# Check that weird input is properly accepted.
t = vcsn.context('lal, q').trie('''
he's
<2>t
{}
()
""
''', format='words')
exp = r'''context = ["\'()2\<\>ehst{}...]? → ℚ
$ -> 0
0 -> $
0 -> 1 h
0 -> 5 \<
0 -> 9 {
0 -> 11 (
0 -> 13 "
1 -> 2 e
2 -> 3 \'
3 -> 4 s
4 -> $
5 -> 6 2
6 -> 7 \>
7 -> 8 t
8 -> $
9 -> 10 }
10 -> $
11 -> 12 )
12 -> $
13 -> 14 "
14 -> $'''
CHECK_EQ(exp, t.format('daut'))
