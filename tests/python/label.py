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
check('lal_char(a), b', 'a', 'a')
check('lal_char(#), b', '#', '#', '\\#')
# This is not a label, it's a word.
XFAIL(lambda: vcsn.context('lal_char(ab), b').label('ab'))
CHECK_EQ(vcsn.context('law_char(ab), b').label('ab'),
         vcsn.context('law_char(ab), b').word('ab'))

# wordset.
check('law_char(a), b',  'a',  'a',  r'\mathit{a}')
check('law_char(ab), b', 'ab', 'ab', r'\mathit{ab}')
# Trailing characters.
XFAIL(lambda: vcsn.context('law_char(ab), b').label('ab*'))

# tupleset.
check('lat<lan, lan>, q',   'a|x',   'a|x', r'a|x')
check('lat<lan, lan>, q', r'\e|x', r'\e|x', r'\varepsilon|x')
