#! /usr/bin/env python

import vcsn
from test import *

def load(file):
    return vcsn.automaton.load(medir + "/" + file + "-det.gv")

## -------------------------------- ##
## Determinize de bruijn/ladybird.  ##
## -------------------------------- ##

# check AUT
# ---------
# Check determinization on FAMILY ("ladybird", or "de-bruijn") using
# the reference files, and check idempotency.
def check(aut, exp):
    CHECK_EQ(False, aut.is_deterministic())
    det = aut.determinize()
    # FIXME: see if we can get rid of this sort.
    exp = load(exp)
    CHECK_EQ(exp, det)
    CHECK_EQ(True, det.is_deterministic())
    # Idempotence.
    CHECK_EQ(exp, det.determinize())


ctx = vcsn.context('lal_char(ab)_b')
check(ctx.de_bruijn(3), 'de-bruijn-3')
check(ctx.de_bruijn(8), 'de-bruijn-8')

ctx = vcsn.context('lal_char(abc)_b')
check(ctx.ladybird(4), 'ladybird-4')
check(ctx.ladybird(8), 'ladybird-8')
