#! /usr/bin/env python

import vcsn
import os
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
  check(exp, aut.replace('CTX', ctx))

a = """
digraph
{
  vcsn_context = "CTX"

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
  vcsn_context = "CTX"
  I0 -> 0
  0 -> 1 [label = "\\\\e, a"]
  1 -> F1
}"""


for ls in ["lan", "law"]:
  for ws in ["b", "z"]:
    check_context(False, a, ls + "_char(ab)_" + ws)

if os.environ.get('VCSN_HAVE_CORRECT_LIST_INITIALIZER_ORDER') != None:
    # Tuples of lan
    a = """
    digraph
    {
      vcsn_context = "lat<lan_char(ab), lan_char(xy)>_b"

      I0 -> 0
      0 -> 1 [label = "(a, x)"]
      1 -> 2 [label = "(\\\\e, y)"]
      2 -> 1 [label = "(b, \\\\e)"]
      1 -> F1
    }"""

    check(True, a)

    a = """
    digraph
    {
      vcsn_context = "lat<lan_char(ab), lan_char(xy)>_b"

      I0 -> 0
      0 -> 1 [label = "(a, x)"]
      1 -> 2 [label = "(\\\\e, y)"]
      2 -> 1 [label = "(b, \\\\e)"]
      2 -> 1 [label = "(\\\\e, \\\\e)"]
      1 -> F1
    }"""

    check(False, a)

    # Tuple of lal x lan
    a = """
    digraph
    {
      vcsn_context = "lat<lal_char(ab), lan_char(xy)>_b"

      I0 -> 0
      0 -> 1 [label = "(a, x)"]
      1 -> 2 [label = "(b, y)"]
      2 -> 1 [label = "(b, \\\\e)"]
      1 -> F1
    }"""

    check(True, a)
