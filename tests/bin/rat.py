#! /usr/bin/env python

from __future__ import print_function

import os, re, sys, vcsn
from test import *

# The name of the current context.
context = 'lal_char(abcd), b'
# The current context.
ctx = vcsn.context(context)

# Compute the name of the context.
contexts = {
  'b'  : "law_char(a-h), b",
  'br' : "law_char(a-h), expressionset<law_char(i-n), b>",
  'z'  : "law_char(a-h), z",
  'zr' : "law_char(a-h), expressionset<law_char(i-n), z>",
  'zrr': "law_char(a-h), expressionset<law_char(i-n), expressionset<law_char(w-z), z>>"
}

def context_update():
  global context, ctx
  if context in contexts:
    context = contexts[context]
  ctx = vcsn.context(context)
  print("# context: {} ({})".format(context, ctx))


def pp(re):
  '''Parse and pretty-print.  If it fails, prepend "! " to the error
  message and return it as result.  Strip the "try -h" line.'''
  try:
    return str(ctx.expression(re))
  except RuntimeError:
    return "! " + str(sys.exc_info()[1])

def check_rat_exp(fname):
  file = open(fname, 'r')
  lineno = 0
  global context
  for line in file:
    lineno += 1
    loc = fname + ':' + str(lineno)

    m = re.match('#.*$|$', line)
    if m is not None:
      continue

    m = re.match('%labels?: (.*)$', line)
    if m is not None:
      labels = m.group(1)
      print('# %labels:', labels)
      if labels == "letters":
        context = context.replace('word', 'letter').replace('law', 'lal')
      elif labels == "words":
        context = context.replace('letter', 'word').replace('lal', 'law')
      else:
        context = labels + ", " + context.split(',', 2)[1]
      context_update()
      continue

    m = re.match('%weights?: (.*)$', line)
    if m is not None:
      weights = m.group(1)
      print('# %weights:', weights)
      context = context.split(',', 2)[0] + ", " + weights
      context_update()
      continue

    m = re.match('%context: (.*)$', line)
    if m is not None:
      context = m.group(1)
      print('# %context:', context)
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
