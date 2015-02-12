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

## ----------- ##
## letterset.  ##
## ----------- ##

# An open context is not printed as open in text.
check('letterset<char_letters>, b', 'letterset<char_letters()>, b')
check('lal_char, b',                'letterset<char_letters()>, b')
# An open context is printed as open in LaTeX.
check('letterset<char_letters>, b', r'\{\ldots\}\rightarrow\mathbb{B}', 'latex')
check('lal_char, b',                r'\{\ldots\}\rightarrow\mathbb{B}', 'latex')

# letterset and different char_letters.
check('lal_char(), b',       'letterset<char_letters()>, b')
check('lal_char(ab), b',     'letterset<char_letters(ab)>, b')
check('lal_char(a-kv-z), b', 'letterset<char_letters(abcdefghijkvwxyz)>, b')
check('lal_char(-0-9), b',   'letterset<char_letters(-0123456789)>, b')
# FIXME: This is obviously wrong: we will not be able to re-read the
# result, as neither the dash nor the paren is escaped.
check(r'lal_char(\\\-\)), b',  r'letterset<char_letters()-\\)>, b')
check(r'lal_char(\--\-), b',   r'letterset<char_letters(-)>, b')

check('lal_char(ab), q', 'letterset<char_letters(ab)>, q')


## --------------- ##
## expressionset.  ##
## --------------- ##

check('expressionset<lal_char(ab), b>, b', 'expressionset<letterset<char_letters(ab)>, b>, b')
# expressions weighted by expressions.
check('expressionset<letterset<char_letters(ab)>, expressionset<letterset<char_letters(xy)>, z>>, b')


## ---------- ##
## tupleset.  ##
## ---------- ##

# Arguably useless, but stresses our tupleset implementation.
check('lat<lat<lal_char(a)>>, b',
      'lat<lat<letterset<char_letters(a)>>>, b')

check('lat<letterset<char_letters(a)>, letterset<char_letters(a)>>, b')
check('lat<lal_char(a),lal_char(a)>,b', 'lat<letterset<char_letters(a)>, letterset<char_letters(a)>>, b')
check('lal_char(ab), lat<b, z>', 'letterset<char_letters(ab)>, lat<b, z>')

# End of level bosses.
check('lat<lal_char(ba),lan<lal_char(vu)>, law_char(x-z)>, lat<expressionset<lat<lal_char(fe), lal_char(hg)>, q>, r, q>',
      'lat<letterset<char_letters(ab)>, nullableset<letterset<char_letters(uv)>>, wordset<char_letters(xyz)>>, lat<expressionset<lat<letterset<char_letters(ef)>, letterset<char_letters(gh)>>, q>, r, q>')

check('lan<lat<lal_char(ba),lat<lan<lal_char(vu)>,law_char(x-z)>>>, lat<expressionset<nullableset<lat<lan_char(fe),lan_char(hg)>>, lat<r, q>>, lat<b, z>>',
      'nullableset<lat<letterset<char_letters(ab)>, lat<nullableset<letterset<char_letters(uv)>>, wordset<char_letters(xyz)>>>>, lat<expressionset<lat<nullableset<letterset<char_letters(ef)>>, nullableset<letterset<char_letters(gh)>>>, lat<r, q>>, lat<b, z>>')


## ------- ##
## LaTeX.  ##
## ------- ##
check("lal_char(abc), b", r'\{a, b, c\}\rightarrow\mathbb{B}',
      format = "latex")
check("lal_char(), b", r'\{\}\rightarrow\mathbb{B}',
      format = "latex")
