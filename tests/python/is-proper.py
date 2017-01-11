#! /usr/bin/env python

import vcsn
import os
from test import *

# check FUNCTION_NAME EXPECTED INPUT
# ----------------------------------
# Check that vcsn FUNCTION gives EXPECTED.
def check(fname, exp, aut):
  a = vcsn.automaton(aut)
  fun = getattr(a, fname)
  # We don't check that the initial cache value is N/A as it can be computed
  # when calling info.
  CHECK_EQ(exp, fun())
  CHECK_EQ(exp, a.info(fname.replace('_', ' ')))

def check_context(fname, exp, aut, ctx):
  print('Context: {}'.format(ctx))
  check(fname, exp, aut.replace('CTX', ctx))

a = r'''
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
}'''

for ls in [("lal", True, True), ("law", True, False)]:
  for ws in ["b", "q"]:
    check_context('is_proper', ls[1], a, ls[0] + "_char(ab), " + ws)
    check_context('is_free', ls[2], a, ls[0] + "_char(ab), " + ws)

a = r'''
digraph
{
  vcsn_context = "CTX"
  I0 -> 0
  0 -> 1 [label = "\\e, a"]
  1 -> F1
}'''


for ls in ["lal", "law"]:
  for ws in ["b", "z"]:
    for f in ["is_proper", "is_free"]:
      check_context(f, False, a, ls + "_char(ab), " + ws)

# Tuples of lal
a = r'''
digraph
{
  vcsn_context = "lat<lal_char(ab), lal_char(xy)>, b"

  I0 -> 0
  0 -> 1 [label = "(a, x)"]
  1 -> 2 [label = "(\\e, y)"]
  2 -> 1 [label = "(b, \\e)"]
  1 -> F1
}'''

check('is_proper', True, a)
check('is_free', False, a)

a = r'''
digraph
{
  vcsn_context = "lat<lal_char(ab), lal_char(xy)>, b"

  I0 -> 0
  0 -> 1 [label = "(a, x)"]
  1 -> 2 [label = "(\\e, y)"]
  2 -> 1 [label = "(b, \\e)"]
  2 -> 1 [label = "(\\e, \\e)"]
  1 -> F1
}'''

check('is_proper', False, a)
check('is_free', False, a)

a = r'''
digraph
{
  vcsn_context = "lat<lal_char(ab), lal_char(xy)>, b"

  I0 -> 0
  0 -> 1 [label = "(a, x)"]
  1 -> 2 [label = "(b, y)"]
  2 -> 1 [label = "(b, \\e)"]
  1 -> F1
}'''

check('is_proper', True, a)
check('is_free', False, a)
