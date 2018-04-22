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
check('[...] -> B', 'a',   'a')
check('[...] -> B', '#',   '#', r'\#')
check('[...] -> B', '[', r'\[', '[')
check('[...] -> B', '{',   '{', r'\{')
check('[...] -> B', '$',   '$', r'\$')
# This is not a label, it's a word.
XFAIL(lambda: vcsn.context('[...] -> B').label('ab'),
      '''unexpected trailing characters: "b"
  while reading label: "ab"''')
CHECK_EQ(vcsn.context('[...]* -> B').label('ab'),
         vcsn.context('[...]* -> B').word('ab'))

# wordset.
check('[...]* -> B',   'a',     'a',      r'\mathit{a}')
check('[...]* -> B',  'ab',    'ab',     r'\mathit{ab}')
check('[...]* -> B', '[#]{}', '\[#\]{}', r'\mathit{[\#]\{\}}')

# Trailing characters.
XFAIL(lambda: vcsn.context('[ab]* -> b').label('ab*'),
      '''[ab]: invalid letter: *
  while reading label: "ab*"''')

# tupleset.
check('[...] x [...] -> q',   'a|x',   'a|x', r'a|x')
check('[...] x [...] -> q', r'\e|x', r'\e|x', r'\varepsilon|x')
