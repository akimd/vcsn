#! /usr/bin/env python

import os, re, subprocess, sys, vcsn

black = '' # `tput setaf 0`;
blue  = '' # `tput setaf 4`;
cyan  = '' # `tput setaf 6`;
green = '' # `tput setaf 2`;
red   = '' # `tput setaf 1`;
white = '' # `tput setaf 7`;
std   = '' # `tput init`;

FAIL = red + "FAIL:" + std
PASS = green + "PASS:" + std

fail = 0
count = 0

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
  if not contexts.has_key(ws):
    print "invalid weightset abbreviation:",  ws
  global name
  name = contexts[ws]
  if labels == 'letters':
    name = name.replace('law', 'lal')
  global ctx
  ctx = vcsn.context(name)
  print "#", ctx, "(", labels, ",", ws, "->", name, ")"

# sub rst_title($)
# {
#   my ($s) = @_;
#   print "\n$s\n";
#   $s =~ s/./-/g;
#   print "$s\n";
# }

#=item C<pp($in)>
#
#Parse and pretty-print.  If it fails, prepend "! " to the error
#message and return it as result.  Strip the "try -h" line.
#
#=cut

def pp(re):
  try:
    return str(ctx.ratexp(re))
  except RuntimeError:
    return "! " + str(sys.exc_info()[1])

# =item C<check_rat_exp($file)
#
# Run C<$prog> on the content of C<$file>.
#
# =cut

def check_rat_exp(fname):
  file = open(fname, 'r')
  lineno = 0
  global ws, labels, count, fail
  for line in file:
    lineno += 1

    m = re.match('#.*$|$', line)
    if m is not None:
      continue

    m = re.match('%labels: (.*)$', line)
    if m is not None:
      labels = m.group(1)
      print "# label: ", labels
      context_update()
      continue

    m = re.match('%weight: (.*)$', line)
    if m is not None:
      ws = m.group(1)
      print "# ws: ", ws
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
      global count
      count += 1
      if L == R:
        print 'ok', count, L, '==', R
      else:
        fail += 1
        print 'not ok', count, L, '!=', R
      continue

    # !: Look for syntax errors.
    m = re.match('(.*\S)\s+(!.*)$', line)
    if m is not None:
      l = m.group(1)
      err = m.group(2)
      L = pp(l)
      count += 1
      if L == err:
        print 'ok', count, l, '=>', L
      else:
        fail += 1
        print 'not ok', count, fname + ':' + str(lineno) + ':', l, '=>', L, "<>", err
      continue

    print fname + ':' + str(lineno) + ':' + 'invalid input:', line

check_rat_exp(sys.argv[1])
print '1..'+str(count)
