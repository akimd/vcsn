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
XFAIL(lambda: vcsn.context("lal_char(a)_b_z"))

# An open context is not printed as open.
check('lal_char_b', 'lal_char()_b')
check('lal_char()_b')
check('lal_char(ab)_b')
check('lal_char(a-kv-z)_b', 'lal_char(abcdefghijkvwxyz)_b')
check('lal_char(-0-9)_b', 'lal_char(-0123456789)_b')
# This is obviously wrong: we will not be able to re-read the result,
# as the backslash is not escaped.
check(r'lal_char(\\\-)_b', r'lal_char(-\)_b')
check(r'lal_char(\--\-)_b', r'lal_char(-)_b')

check('lal_char(ab)_q')
check('ratexpset<lal_char(ab)_b>_b')

# Arguably useless, but stresses our tupleset implementation.
check('lat<lat<lal_char(a)>>_b')

check('lat<lal_char(a),lal_char(a)>_b')
check('lat<lal_char(a), lal_char(a)>_b', 'lat<lal_char(a),lal_char(a)>_b')
check('lal_char(ab)_lat<b, z>', 'lal_char(ab)_lat<b,z>')

# End of level bosses.
check('lat<lal_char(ba),lan<lal_char(vu)>, law_char(x-z)>_lat<ratexpset<lat<lal_char(fe), lal_char(hg)>_q>, r, q>',
      'lat<lal_char(ab),lan<lal_char(uv)>,law_char(xyz)>_lat<ratexpset<lat<lal_char(ef),lal_char(gh)>_q>,r,q>')

check('lan<lat<lal_char(ba),lat<lan<lal_char(vu)>,law_char(x-z)>>>_lat<ratexpset<lan<lat<lan_char(fe),lan_char(hg)>>_lat<r, q>>, lat<b, z>>',
      'lan<lat<lal_char(ab),lat<lan<lal_char(uv)>,law_char(xyz)>>>_lat<ratexpset<lat<lan<lal_char(ef)>,lan<lal_char(gh)>>_lat<r,q>>,lat<b,z>>')


## ------- ##
## LaTeX.  ##
## ------- ##
check("lal_char(abc)_b", r'\{a, b, c\}\rightarrow\mathbb{B}',
      format = "latex")
check("lal_char()_b", r'\{\}\rightarrow\mathbb{B}',
      format = "latex")
