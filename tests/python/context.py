#! /usr/bin/env python

import vcsn
from test import *

def check(ctx, exp = None):
    c = vcsn.context(ctx)
    if exp is None:
        exp = ctx
    eff = str(c)
    if eff == exp:
        PASS()
    else:
        FAIL(exp + " != " + eff)

check('lal_char(ab)_b')
check('lal_char(a-kv-z)_b', 'lal_char(abcdefghijkvwxyz)_b')
check('lal_char(-0-9)_b', 'lal_char(-0123456789)_b')
check(r'lal_char(\\\-)_b', r'lal_char(-\)_b')
check(r'lal_char(\--\-)_b', r'lal_char(-)_b')

check('lal_char(ab)_q')

check('lat<lal_char(a),lal_char(a)>_b')
check('lat<lal_char(a), lal_char(a)>_b', 'lat<lal_char(a),lal_char(a)>_b')
check('lal_char(ab)_lat<b, z>', 'lal_char(ab)_lat<b,z>')

# End of level boss.
check('lat<lal_char(ab),lan_char(uv), law_char(xyz)>_lat<ratexpset<lat<lal_char(ef), lal_char(gh)>_q>, r, q>')

PLAN()
