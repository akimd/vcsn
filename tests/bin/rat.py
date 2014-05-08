#! /usr/bin/env python

from __future__ import print_function

import os, re, sys, vcsn
from test import *

# The current atom kind (argument for -a).
labels = 'letters'
# The current weightset (argument for -w).
ws = 'b';
# The current context.
name = 'lal_char(abcd)_b'
ctx = vcsn.context(name)

# Compute the name of the context.
contexts = {
  'b'  : "law_char(abcd)_b",
  'br' : "law_char(abcd)_ratexpset<law_char(efgh)_b>",
  'z'  : "law_char(abcd)_z",
  'zr' : "law_char(abcd)_ratexpset<law_char(efgh)_z>",
  'zrr': "law_char(abcd)_ratexpset<law_char(efgh)_ratexpset<law_char(xyz)_z>>"
}

def context_update():
  if ws not in contexts:
    print("invalid weightset abbreviation:", ws)
  global name
  name = contexts[ws]
  if labels == 'letters':
    name = name.replace('law', 'lal')
  global ctx
  ctx = vcsn.context(name)
  print("#", ctx, "(", labels, ",", ws, "->", name, ")")


def pp(re):
  '''Parse and pretty-print.  If it fails, prepend "! " to the error
  message and return it as result.  Strip the "try -h" line.'''
  try:
    return str(ctx.ratexp(re))
  except RuntimeError:
    return "! " + str(sys.exc_info()[1])

def check_rat_exp(fname):
  file = open(fname, 'r')
  lineno = 0
  global ws, labels
  for line in file:
    lineno += 1
    loc = fname + ':' + str(lineno)

    m = re.match('#.*$|$', line)
    if m is not None:
      continue

    m = re.match('%labels: (.*)$', line)
    if m is not None:
      labels = m.group(1)
      print("# label: ", labels)
      context_update()
      continue

    m = re.match('%weight: (.*)$', line)
    if m is not None:
      ws = m.group(1)
      print("# ws: ", ws)
      context_update()
      continue

    m = re.match('%include: (.*)$', line)
    if m is not None:
      check_rat_exp(os.path.dirname(fname) + '/' + m.group(1))
      continue

    # == tests that boths are equivalent.
    # => check the actual result.
    m = re.match('(.*\S)\s*(=>|==)\s*(.*)$', line)
    if m is not None:
      l = m.group(1)
      op = m.group(2)
      r = m.group(3)
      L = pp(l)
      if op == '==':
        R = pp(r)
      else:
        R = r
      CHECK_EQ(R, L, loc)
      continue

    # !: Look for syntax errors.
    m = re.match('(.*\S)\s+(!.*)$', line)
    if m is not None:
      l = m.group(1)
      err = m.group(2)
      L = pp(l)
      CHECK_EQ(err, L, loc)
      continue

    print(loc + ":", 'invalid input:', line)

check_rat_exp(sys.argv[1])
