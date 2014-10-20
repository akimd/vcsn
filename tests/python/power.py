#! /usr/bin/env python

import vcsn
from test import *

z = vcsn.context('lal_char(01)_z')

# FIXME: check that only the accessible part is kept.  Start from
# binary.gv, modified with an inaccessible state.
binary = load('lal_char_z/binary.gv')

# power 0.
CHECK_EQ(vcsn.automaton(filename = medir + '/binary^0.gv'),
    binary ** 0)

# power 1.
CHECK_EQ(vcsn.automaton(filename = medir + '/binary^1.gv'),
    binary ** 1)

# power 4.
binary4 = binary ** 4
# 1^4 = 0.
CHECK_EQ(z.weight('0'), binary4.eval('0'))
# 1^4 = 1.
CHECK_EQ(z.weight('1'), binary4.eval('1'))
# 8^4 = 256.
CHECK_EQ(z.weight('256'), binary4.eval('100'))

# Power 5.
binary5 = binary ** 5
CHECK_EQ(z.weight('32'), binary5.eval('10'))
#run 0 '' -vcsn power -o binary^5.gv -f $binary_gv 5
#run 0 32 -vcsn evaluate -f binary^5.gv 10

# Power 7.
binary7 = binary ** 7
CHECK_EQ(z.weight('128'), binary7.eval('10'))
