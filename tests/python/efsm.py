#! /usr/bin/env python

from __future__ import print_function
import subprocess
import vcsn
from test import *

from vcsn.automaton import _automaton_fst, _automaton_fst_files

# Do we have OpenFST installed?
# Calling 'fstprint --help' is tempting, but it exits 1.
have_ofst = which('fstprint') is not None

def check(aut, fefsm):
  'Check the conversion to and from FSM.'
  print(here() + ": check:", fefsm)

  # Text in efsm format.
  efsm = open(medir + "/" + fefsm).read().strip()

  # Check output to EFSM.
  CHECK_EQ(efsm, aut.format('efsm'))

  # Check print | read | print to EFSM.
  #
  # We used to check that the output is exactly what we get when
  # reading back and printing again.  This does not work for automata
  # with several initial states or a non-one initial weight (which is
  # approximated below as !is_standard), as then we show pre as the
  # real initial state.  The pre state is displayed as a large natural
  # integer, and when read and printed back, it is renumbered as 0.
  #
  # So (read | print) is not the identity.
  aut2 = vcsn.automaton(efsm, 'efsm')
  if aut.is_standard():
    CHECK_EQ(aut, aut2)
    CHECK_EQ(efsm, aut2.format('efsm'))
  else:
    CHECK_EQUIV(aut, aut2)

  # Check that OpenFST accepts and reproduces our EFSM files.
  if have_ofst:
    aut3 = _automaton_fst('cat', aut)
    if aut.is_standard():
      CHECK_EQ(aut, aut3)
    else:
      CHECK_EQUIV(aut, aut3)
  else:
    SKIP('OpenFST is missing')

a = load('lal_char_b/a1.gv')
check(a, 'a1.efsm')
a = load('lal_char_z/binary.gv')
check(a, 'binary.efsm')

for f in ["lal-char-z", "lat-z"]:
  print("f:", f)
  a = vcsn.automaton(filename = medir + "/" + f + '.gv')
  check(a, f + '.efsm')

# Check the case of an automaton without any transition.
a = vcsn.context('lal_char(), b').ratexp('\e').standard()
check(a, 'one.efsm')

# Check the support of spontaneous transitions.
# Note that "sort" is critical here, otherwise the transitions
# are not issued in the state-order, so when we read back, the
# states numbers are different.
a = vcsn.context('lan_char(ab), b').ratexp('ab*').thompson().sort().strip()
check(a, 'abs.efsm')

# Using law_char(a-z), b is tempting, but when reading back, we take
# the smallest possible alphabet.
a = vcsn.context('law_char(acdeghilnprt), b')\
      .ratexp('(grand)*(parent+child)').thompson().sort().strip()
check(a, 'str.efsm')

# A transducer that looks like an acceptor when looking at the symbol
# numbers.
a = vcsn.context('lat<lal_char(abc),lal_char(xyz)>, b')\
      .ratexp("('(a,x)'+'(b,y)'+'(c,z)')*").standard().sort().strip()
check(a, 'a2x.efsm')

# A transducer that cannot be seen as an acceptor.
a = vcsn.context('lat<lal_char(a),lal_char(xyz)>, b')\
      .ratexp("('(a,x)'+'(a,y)'+'(a,z)')*").standard().sort().strip()
check(a, 'a2xyz.efsm')

if have_ofst:
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
else:
  SKIP('OpenFST is missing')
