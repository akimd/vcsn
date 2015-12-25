#! /usr/bin/env python

import vcsn
from test import *

def check(ctx, exp):
    c = vcsn.context(ctx)
    try:
        r = c.expression(exp)
    except RuntimeError:
        FAIL("error parsing " + exp)
    else:
        CHECK_EQ(exp, r)

check('lal_char(ab), lat<q, expressionset<lal_char(xyz), b>, z>',
      'a')
check('lal_char(ab), lat<q, expressionset<lal_char(xyz), b>, z>',
      '<2/3,x,-3>a')
check('lal_char(ab), lat<q, expressionset<lal_char(xyz), b>, z>',
      '<2/3,x+y*,-3>a')
check('lal_char(ab), lat<expressionset<lal_char(xyz), lat<q, q>>, lat<q, q>>',
      '<<(1,2)>x+<(2,1/3)>y*,(2,3)>a')
