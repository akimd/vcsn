#! /usr/bin/env python

import vcsn
from test import *

def check(ctx, exp):
    c = vcsn.context(ctx)
    try:
        c.ratexp(exp)
    except RuntimeError:
        FAIL("error parsing " + exp)
    finally:
        PASS()

check('lal_char(ab)_lat<q, ratexpset<lal_char(xyz)_b>, z>', 'a')
check('lal_char(ab)_lat<q, ratexpset<lal_char(xyz)_b>, z>', '<(2/3, x, -3)>a')
check('lal_char(ab)_lat<q, ratexpset<lal_char(xyz)_b>, z>', '<(2/3, x*+y, -3)>a')
check('lal_char(ab)_lat<ratexpset<lal_char(xyz)_lat<q, q>>, lat<q, q>>',
      '<(<(1, 2)>x+<(2, 1/3)>y*, (2, 3))>a')
