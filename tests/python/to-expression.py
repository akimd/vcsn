#! /usr/bin/env python

import vcsn
from test import *

## -------------------- ##
## automaton.ratexp().  ##
## -------------------- ##

# check_aut AUTOMATON EXP-OUT
# ---------------------------
# Check that to-expression(AUTOMATON) = EXP-OUT.
def check_aut(aut, exp):
    CHECK_EQ(exp, str(aut.ratexp()))

# Check the associativity of the product.
check_aut(vcsn.automaton(filename = medir + "/a.gv"), '<x>a(<y>b)*<z>c')

check_aut(load('lal_char_b/a1.gv'), '(a+b)*ab(a+b)*')
check_aut(load('lal_char_b/b1.gv'), '(a+b)*a(a+b)*')
check_aut(load('lal_char_b/evena.gv'), 'b*a(b+ab*a)*')
check_aut(load('lal_char_b/oddb.gv'), 'a*b(a+ba*b)*')

check_aut(load('lal_char_z/b1.gv'), '(a+b)*a(a+b)*')
check_aut(load('lal_char_z/binary.gv'), '(0+1)*1(<2>0+<2>1)*')
check_aut(load('lal_char_z/c1.gv'), '(a+b)*b(<2>a+<2>b)*')
check_aut(load('lal_char_z/d1.gv'), '(a+b)*(a+<-1>b)(a+b)*')

check_aut(load('lal_char_zmin/minab.gv'), '(<1>a+b)*+(a+<1>b)*')
check_aut(load('lal_char_zmin/minblocka.gv'), '(a+b)*b(<1>a)*b(a+b)*')
check_aut(load('lal_char_zmin/slowgrow.gv'),
          '(a+b)*b(<1>a)*b(a+<1>a(<1>a)*b)*')


## ---------------------------- ##
## ratexp.standard().ratexp().  ##
## ---------------------------- ##

ctx = vcsn.context("lal_char(abc), b")

# check_exp RAT EXP-OUT
# ---------------------
# Check that to-expression(standard(RAT)) = EXP-OUT.
def check_exp(rat, exp):
    check_aut(ctx.ratexp(rat).standard(), exp)

check_exp('a',                            'a')
check_exp('ab',                           'ab')
check_exp('a*',                           '\e+aa*')
check_exp('a+b',                          'a+b')
check_exp('(?@lal_char(ab), z)<2>(ab)<3>', '(<2>ab)<3>')
check_exp('(?@law_char(ab), z)<2>(ab)<3>', '<6>(ab)')


check_exp('(?@lal_char(abc), z)(<2>(<3>a+<5>b)<7>c)<11>', \
      '(<6>a<7>c+<10>b<7>c)<11>')

# Likewise, but check the order of the products.
check_exp('(?@lal_char(abc), ratexpset<lal_char(vwxyz), b>)(<v>(<w>a+<x>b)<y>c)<z>', \
      '(<vw>a<y>c+<vx>b<y>c)<z>')
