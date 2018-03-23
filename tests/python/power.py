#! /usr/bin/env python

import vcsn
from test import *

z = vcsn.context('[01] -> z')

binary = load('lal_char_z/binary.gv')

# power 0.
CHECK_EQ(meaut('binary^0.gv'),
         binary & 0)

# power 1.
CHECK_EQ(meaut('binary^1.gv'),
         binary & 1)

# power 4.
binary4 = binary & 4
# 0^4 = 0.
CHECK_EQ(z.weight('0'), binary4('0'))
# 1^4 = 1.
CHECK_EQ(z.weight('1'), binary4('1'))
# 4^4 = 256.
CHECK_EQ(z.weight('256'), binary4('100'))

# Power 5.
binary5 = binary & 5
CHECK_EQ(z.weight('32'), binary5('10'))
#run 0 '' -vcsn power -o binary^5.gv -f $binary_gv 5
#run 0 32 -vcsn evaluate -f binary^5.gv 10

# Power 7.
binary7 = binary & 7
CHECK_EQ(z.weight('128'), binary7('10'))

# Check that only the accessible part is kept using binary.gv,
# modified with an inaccessible part.
inacc = binary + vcsn.automaton('''context = [...]? → ℤ
0 -> 1 0''')
for i in [0, 1, 4, 7]:
    CHECK_EQ(binary & i, (inacc & i).__value__())
