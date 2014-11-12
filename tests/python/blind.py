#! /usr/bin/env python

import vcsn
from test import *

c = vcsn.context('lat<lal_char(abc),lal_char(efg),lal_char(xyz)>, b')
t = c.expression("('(a,e,x)':'(b,f,y)':'(c,g,z)')").derived_term().strip()

CHECK_EQ('(ab+ba)c+(ac+ca)b+(bc+cb)a',
         t.blind(0).expression())

CHECK_EQ('(ef+fe)g+(eg+ge)f+(fg+gf)e',
         t.blind(1).expression())

CHECK_EQ('(xy+yx)z+(xz+zx)y+(yz+zy)x',
         t.blind(2).expression())
