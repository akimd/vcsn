#! /usr/bin/env python

import vcsn
from test import *

# check EXPECTED INPUT
# --------------------------
# Check that vcsn is-proper gives EXPECTED.
def check(exp, aut):
  eff = vcsn.automaton(aut).is_proper()
  if eff == exp:
     PASS()
  else:
     FAIL(exp + " != " + eff)

def check_context(exp, aut, ctx):
  check(exp, aut.replace('ctx', ctx))

a = """
digraph
{
  vcsn_context = "lal_char(ab)_b"

  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> 0 [label = "b"]
  2 -> 1 [label = "a"]
  2 -> 2 [label = "b"]
  1 -> F1
}"""

for ls in ["lal", "lan", "law"]:
  for ws in ["b", "z"]:
    check_context(True, a, ls + "_char(ab)_" + ws)

a = """
digraph
{
  vcsn_context = "lan_char(ab)_b"
  I0 -> 0
  0 -> 1 [label = "\\\\e, a"]
  1 -> F1
}"""

for ls in ["lan", "law"]:
  for ws in ["b", "z"]:
    check_context(False, a, ls + "_char(ab)_" + ws)


PLAN()
