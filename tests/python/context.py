#! /usr/bin/env python

import vcsn
from test import *

def check(ctx, exp = None, format = "text"):
    c = vcsn.context(ctx)
    if exp is None:
        exp = ctx
    CHECK_EQ(exp, c.format(format))

# Invalid context: invalid weightset.
XFAIL(lambda: vcsn.context("lal_char(a)_UNKNOWN"))

# Invalid context: trailing garbage.
XFAIL(lambda: vcsn.context("lal_char(a), b_z"))

# An open context is not printed as open in text.
check('letterset<char_letters>, b', 'letterset<char_letters()>, b')
check('lal_char, b',                'letterset<char_letters()>, b')
# An open context is printed as open in LaTeX.
check('letterset<char_letters>, b', r'\{\ldots\}\rightarrow\mathbb{B}', 'latex')
check('lal_char, b',                r'\{\ldots\}\rightarrow\mathbb{B}', 'latex')


check('lal_char(), b', 'letterset<char_letters()>, b')
check('lal_char(ab), b', 'letterset<char_letters(ab)>, b')
check('lal_char(a-kv-z), b', 'letterset<char_letters(abcdefghijkvwxyz)>, b')
check('lal_char(-0-9), b', 'letterset<char_letters(-0123456789)>, b')
# FIXME: This is obviously wrong: we will not be able to re-read the
# result, as neither the dash nor the paren is escaped.
check(r'lal_char(\\\-\)), b',  r'letterset<char_letters()-\\)>, b')
check(r'lal_char(\--\-), b', r'letterset<char_letters(-)>, b')

check('lal_char(ab), q', 'letterset<char_letters(ab)>, q')
check('expressionset<lal_char(ab), b>, b', 'expressionset<letterset<char_letters(ab)>, b>, b')

# Arguably useless, but stresses our tupleset implementation.
check('lat<lat<lal_char(a)>>, b',
      'lat<lat<letterset<char_letters(a)>>>, b')

check('lat<letterset<char_letters(a)>, letterset<char_letters(a)>>, b')
check('lat<lal_char(a),lal_char(a)>,b', 'lat<letterset<char_letters(a)>, letterset<char_letters(a)>>, b')
check('lal_char(ab), lat<b, z>', 'letterset<char_letters(ab)>, lat<b, z>')

# End of level bosses.
check('lat<lal_char(ba),lan<lal_char(vu)>, law_char(x-z)>, lat<expressionset<lat<lal_char(fe), lal_char(hg)>, q>, r, q>',
      'lat<letterset<char_letters(ab)>, lan<letterset<char_letters(uv)>>, wordset<char_letters(xyz)>>, lat<expressionset<lat<letterset<char_letters(ef)>, letterset<char_letters(gh)>>, q>, r, q>')

check('lan<lat<lal_char(ba),lat<lan<lal_char(vu)>,law_char(x-z)>>>, lat<expressionset<lan<lat<lan_char(fe),lan_char(hg)>>, lat<r, q>>, lat<b, z>>',
      'lan<lat<letterset<char_letters(ab)>, lat<lan<letterset<char_letters(uv)>>, wordset<char_letters(xyz)>>>>, lat<expressionset<lat<lan<letterset<char_letters(ef)>>, lan<letterset<char_letters(gh)>>>, lat<r, q>>, lat<b, z>>')


## ------- ##
## LaTeX.  ##
## ------- ##
check("lal_char(abc), b", r'\{a, b, c\}\rightarrow\mathbb{B}',
      format = "latex")
check("lal_char(), b", r'\{\}\rightarrow\mathbb{B}',
      format = "latex")
