#! /usr/bin/env python

import vcsn
import os
from test import *

# check EXPECTED INPUT
# --------------------
# Check that vcsn is-proper gives EXPECTED.
def check(exp, aut):
  a = vcsn.automaton(aut)
  CHECK_EQ('N/A', a.info('is proper'))
  CHECK_EQ(exp, a.is_proper())
  CHECK_EQ(exp, a.info('is proper'))

def check_context(exp, aut, ctx):
  print('Context: {}'.format(ctx))
  check(exp, aut.replace('CTX', ctx))

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

for ls in ["lal", "lan", "law"]:
  for ws in ["b", "q"]:
    check_context(True, a, ls + "_char(ab), " + ws)

a = r'''
digraph
{
  vcsn_context = "CTX"
  I0 -> 0
  0 -> 1 [label = "\\e, a"]
  1 -> F1
}'''


for ls in ["lan", "law"]:
  for ws in ["b", "z"]:
    check_context(False, a, ls + "_char(ab), " + ws)

# Tuples of lan
a = r'''
digraph
{
  vcsn_context = "lat<lan_char(ab), lan_char(xy)>, b"

  I0 -> 0
  0 -> 1 [label = "(a, x)"]
  1 -> 2 [label = "(\\e, y)"]
  2 -> 1 [label = "(b, \\e)"]
  1 -> F1
}'''

check(True, a)

a = r'''
digraph
{
  vcsn_context = "lat<lan_char(ab), lan_char(xy)>, b"

  I0 -> 0
  0 -> 1 [label = "(a, x)"]
  1 -> 2 [label = "(\\e, y)"]
  2 -> 1 [label = "(b, \\e)"]
  2 -> 1 [label = "(\\e, \\e)"]
  1 -> F1
}'''

check(False, a)

# Tuple of lal x lan
a = r'''
digraph
{
  vcsn_context = "lat<lal_char(ab), lan_char(xy)>, b"

  I0 -> 0
  0 -> 1 [label = "(a, x)"]
  1 -> 2 [label = "(b, y)"]
  2 -> 1 [label = "(b, \\e)"]
  1 -> F1
}'''

check(True, a)
