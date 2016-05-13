#! /usr/bin/env python

import os, re, sys, vcsn
from test import *

# The name of the current context.
context = 'lal_char(abcd), b'
# The current context.
ctx = vcsn.context(context)
# Whether expressions or series.
identities = "associative"

# Compute the name of the context.
contexts = {
  'b'  : "law_char(a-h), b",
  'br' : "law_char(a-h), expressionset<law_char(i-n), b>",
  'brr': "law_char(a-h), expressionset<law_char(i-n), expressionset<law_char(w-z), b>>",
  'q'  : "law_char(a-h), q",
  'qr' : "law_char(a-h), expressionset<law_char(i-n), q>",
  'qrr': "law_char(a-h), expressionset<law_char(i-n), expressionset<law_char(w-z), q>>",
  'z'  : "law_char(a-h), z",
  'zr' : "law_char(a-h), expressionset<law_char(i-n), z>",
  'zrr': "law_char(a-h), expressionset<law_char(i-n), expressionset<law_char(w-z), z>>",
}

def context_update():
  global context, ctx
  if context in contexts:
    context = contexts[context]
  ctx = vcsn.context(context)
  print("# context: {} ({})".format(context, ctx))


def expr(e):
  '''Parse.  If it fails, prepend "! " to the error
  message and return it as result.  Strip the "try -h" line.'''
  try:
    print(e, identities)
    return ctx.expression(e, identities)
  except RuntimeError:
    err = re.sub('^', '! ', str(sys.exc_info()[1]), flags=re.M)
    return r'\n'.join(err.splitlines())

def pp(re):
  '''Parse and pretty-print.  If it fails, prepend "! " to the error
  message and return it as result.  Strip the "try -h" line.'''
  return str(expr(re))

def check_rat_exp(fname):
  file = open(fname, 'r', encoding='utf-8')
  lineno = 0
  global context, identities
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

    m = re.match('%identities: (.*)$', line)
    if m is not None:
      identities = m.group(1)
      print('# %identities:', identities)
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
      if op == '==':
        # If we failed to convert l, then expr(l) is a string.  Don't
        # check a string against an expression.  This is needed for
        # xfail3.
        l = expr(l)
        r = expr(r)
        if isinstance(l, str):
          r = str(r)
        CHECK_EQ(r, l, loc)
      else:
        CHECK_EQ(r, pp(l), loc)
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
