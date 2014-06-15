#! /usr/bin/env python

import vcsn
from test import *

from vcsn.automaton import _automaton_fst, _automaton_fst_files

def check (aut, fefsm):
  'Check the conversion from Dot to FSM, and back.'
  efsm = open(medir + "/" + fefsm).read().strip()

  # Check support for EFSM I/O.
  CHECK_EQ(efsm, aut.format('efsm'))
  CHECK_EQ(efsm, vcsn.automaton(efsm, 'efsm').format('efsm'))

  # Check OpenFST's support for EFSM I/O.
  CHECK_EQ(aut, _automaton_fst('cat', aut))

a1 = load('lal_char_b/a1.gv')
check(a1, 'a1.efsm')
bin = load('lal_char_z/binary.gv')
check(bin, 'binary.efsm')

# Check the support of spontaneous transitions.
# Note that "sort" is critical here, otherwise the transitions
# are not issued in the state-order, so when we read back, the
# states numbers are different.
abs = vcsn.context('lan_char(ab)_b').ratexp('ab*').thompson().sort()
check(abs, 'abs.efsm')

# Using law_char(a-z)_b is tempting, but when reading back, we take
# the smallest possible alphabet.
str = vcsn.context('law_char(acdeghilnprt)_b').ratexp('(grand)*(parent+child)').thompson().sort()
check(str, 'str.efsm')

# Check that Open FST and V2 understand the weights the same way.
#
# c1 & 2 by v2.
c1 = load('lal_char_z/c1.gv')
c2_vcsn = c1 & 2

# c1 & c1 by OpenFST.
c2_ofst = c1.fstintersect(c1)

CHECK_EQ(c2_vcsn, c2_ofst)
# Let OpenFST compare them.
# fst 0 '' -fstequal c12.v2.ofst c12.ofst.ofst
