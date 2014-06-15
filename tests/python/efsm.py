#! /usr/bin/env python

import vcsn
from test import *

from vcsn.automaton import _automaton_fst, _automaton_fst_files

# check GV EFSM
# -------------
# Check the conversion from Dot to FSM, and back.
def check (fgv, fefsm):
  gv = open(vcsn.datadir + "/" + fgv).read().strip()
  efsm = open(medir + "/" + fefsm).read().strip()

  # Check support for EFSM I/O.
  CHECK_EQ(efsm, vcsn.automaton(gv, 'dot').format('efsm'))
  CHECK_EQ(efsm, vcsn.automaton(efsm, 'efsm').format('efsm'))

  # Check OpenFST's support for EFSM I/O.
  aut = vcsn.automaton(gv)
  CHECK_EQ(aut, _automaton_fst('cat', aut))

check('lal_char_b/a1.gv', 'a1.efsm')
check('lal_char_z/binary.gv', 'binary.efsm')


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
