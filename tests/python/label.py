#! /usr/bin/env python

import vcsn
from test import *

## -------- ##
## Labels.  ##
## -------- ##

# check CONTEXT INPUT OUTPUT [LATEX]
# ----------------------------------
def check(ctx, l, output, latex = None):
    ctx = vcsn.context(ctx)
    l = ctx.label(l)
    CHECK_EQ(output, str(l))
    if latex:
        CHECK_EQ(latex, l.format('latex'))


# letterset.
check('lal, b', 'a',   'a')
check('lal, b', '#',   '#', r'\#')
check('lal, b', '[', r'\[', '[')
check('lal, b', '{',   '{', r'\{')
check('lal, b', '$',   '$', r'\$')
# This is not a label, it's a word.
XFAIL(lambda: vcsn.context('lal, b').label('ab'),
      '''unexpected trailing characters: b
  while reading label: ab''')
CHECK_EQ(vcsn.context('law, b').label('ab'),
         vcsn.context('law, b').word('ab'))

# wordset.
check('law, b',   'a',     'a',      r'\mathit{a}')
check('law, b',  'ab',    'ab',     r'\mathit{ab}')
check('law, b', '[#]{}', '\[#\]{}', r'\mathit{[\#]\{\}}')

# Trailing characters.
XFAIL(lambda: vcsn.context('law(ab), b').label('ab*'),
      '''{ab}: invalid letter: *
  while reading label: ab*''')

# tupleset.
check('lat<lal, lal>, q',   'a|x',   'a|x', r'a|x')
check('lat<lal, lal>, q', r'\e|x', r'\e|x', r'\varepsilon|x')
