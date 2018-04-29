#! /usr/bin/env python

import vcsn
from test import *

## -------- ##
## Labels.  ##
## -------- ##

# check CONTEXT INPUT OUTPUT [LATEX]
# ----------------------------------
def check(ctx, l, **kwargs):
    ctx = vcsn.context(ctx)
    l = ctx.label(l)
    for fmt, exp in kwargs.items():
        CHECK_EQ(exp, l.format(fmt))


# letterset.
check('[...]', 'a', utf8='a')
check('[...]', '#', utf8='#',   latex=r'\#')
check('[...]', '[', utf8=r'\[', latex='[')
check('[...]', '{', utf8='{',   latex=r'\{')
check('[...]', '$', utf8='$',   latex=r'\$')
# This is not a label, it's a word.
XFAIL(lambda: vcsn.context('[...] -> B').label('ab'),
      '''unexpected trailing characters: "b"
  while reading label: "ab"''')
CHECK_EQ(vcsn.context('[...]*').label('ab'),
         vcsn.context('[...]*').word('ab'))

# wordset.
check('[...]*', 'a',     utf8='a',       latex=r'\mathit{a}')
check('[...]*', 'ab',    utf8='ab',      latex=r'\mathit{ab}')
check('[...]*', '[#]{}', utf8='\[#\]{}', latex=r'\mathit{[\#]\{\}}')

# Trailing characters.
XFAIL(lambda: vcsn.context('[ab]* -> b').label('ab*'),
      '''[ab]: invalid letter: *
  while reading label: "ab*"''')

# tupleset.
check('[...] x [...]',   'a|x', text=  'a|x', utf8='a|x', latex=r'a|x')
check('[...] x [...]', r'\e|x', text=r'\e|x', utf8='ε|x', latex=r'\varepsilon|x')
