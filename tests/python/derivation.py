#! /usr/bin/env python

from __future__ import print_function

import vcsn
from test import *

ctx = vcsn.context("lal_char(abc)_ratexpset<lal_char(xyz)_z>")

def check(re, s, exp, breaking = False):
    "Check that d/ds(re) = exp."
    if not isinstance(re, vcsn.ratexp):
        re = ctx.ratexp(re)
    eff = re.derivation(s, breaking)
    print("d/d{}({}) = {}".format(s, re, eff));
    CHECK_EQ(exp, str(eff))


##########################
## Regular derivation.  ##
##########################



## ---------------------------- ##
## Derive wrt a single letter.  ##
## ---------------------------- ##

# Zero, one.
check(   '\z', 'a', '\z')
check(   '\e', 'a', '\z')
check('<x>\e', 'a', '\z')

# Letters.
check(   'a', 'a', '\e')
check(   'a', 'b', '\z')
check('<x>a', 'a', '<x>\e')
check('<x>a', 'b', '\z')

# Sum.
check('<x>a+<y>b', 'a', '<x>\e')
check('<x>a+<y>b', 'b', '<y>\e')
check('<x>a+<y>a', 'a', '<x+y>\e')

# Prod.
check('ab', 'a', 'b')
check('ab', 'b', '\z')
check('(<x>a).(<y>a).(<z>a)', 'a', '<x><y>a<z>a')

# Conjunction.
check('<x>a&<y>a&<z>a', 'a', '<xyz>\e')
check('<x>a&<y>a&<z>a', 'b', '\z')

check('(<x>a+<y>b)*&(<z>b+<x>c)*', 'a', '\z')
check('(<x>a+<y>b)*&(<z>b+<x>c)*', 'b', '<yz>(<x>a+<y>b)*&(<z>b+<x>c)*')
check('(<x>a+<y>b)*&(<z>b+<x>c)*', 'c', '\z')

# Shuffle.
check('<x>a:<y>a:<z>a', 'a', '<x>\e:<y>a:<z>a + <y><x>a:\e:<z>a + <z><x>a:<y>a:\e')
check('<x>a:<y>b:<z>c', 'a', '<x>\e:<y>b:<z>c')
check('<x>a:<y>b:<z>c', 'b', '<y><x>a:\e:<z>c')
check('<x>a:<y>b:<z>c', 'c', '<z><x>a:<y>b:\e')

check('(<x>a<y>b)*:(<x>a<x>c)*',
      'a', '<x>(<x>a<y>b)*:<x>c(<x>a<x>c)* + <x><y>b(<x>a<y>b)*:(<x>a<x>c)*')
check('(<x>a<y>b)*:(<x>a<x>c)*', 'b', '\z')
check('(<x>a<y>b)*:(<x>a<x>c)*', 'c', '\z')

# Star.
check('a*', 'a', 'a*')
check('a*', 'b', '\z')
check('(<x>a)*', 'a', '<x>(<x>a)*')
check('(<x>a)*', 'b', '\z')
check('<x>a*',   'a', '<x>a*')
check('<x>(<y>a)*', 'a', '<xy>(<y>a)*')
check('(<x>a)*<y>', 'a', '<x>(<x>a)*<y>')
check('(<x>a)*<y>', 'aa', '<xx>(<x>a)*<y>')


# Complement.
check('\z{c}', 'a', '\z{c}')
check('\e{c}', 'a', '\z{c}')

check('a{c}', 'a', '\e{c}')
check('a{c}', 'b', '\z{c}')

check('(a+b){c}', 'a', '\e{c}')
check('(a+b){c}', 'c', '\z{c}')

check('(a.b){c}', 'a', 'b{c}')
check('(a.b){c}', 'b', '\z{c}')

check('(a:b){c}', 'a', '(\e:b){c}')
check('(a:b){c}', 'b', '(a:\e){c}')
check('(a:b){c}', 'c', '\z{c}')

check('(a*&a*){c}', 'a', '(a*&a*){c}')
check('(a*&a*){c}', 'b', '\z{c}')

check('(<x>(<y>a)*<z>){c}', 'a', '(<y>a)*{c}')
check('(<x>(<y>a)*<z>){c}', 'b', '\z{c}')

check('a{c}{c}', 'a', '\e{c}{c}')
check('a{c}{c}', 'b', '\z{c}{c}')


## ------------------------------- ##
## Derive wrt to several letters.  ##
## ------------------------------- ##

check('(<x>a)*', 'aa',   '<xx>(<x>a)*')
check('(<x>a)*', 'aaaa', '<xxxx>(<x>a)*')
check('(<x>a)*', 'aaab', '\z')

check('(<x>a)*(<y>b)*', 'aa',   '<xx>(<x>a)*(<y>b)*')
check('(<x>a)*(<y>b)*', 'aabb', '<xxyy>(<y>b)*')


## -------------------- ##
## Classical examples.  ##
## -------------------- ##

# EAT, Example 4.3.
E1='(<1/6>a*+<1/3>b*)*'
# E1 typed.
E1t="(?@lal_char(ab)_q)"+E1
check(E1t,  'a',  "<1/3>a*"+E1)
check(E1t,  'b',  "<2/3>b*"+E1)
check(E1t, 'aa',  "<4/9>a*"+E1)
check(E1t, 'ab',  "<2/9>b*"+E1)
check(E1t, 'ba',  "<2/9>a*"+E1)
check(E1t, 'bb', "<10/9>b*"+E1)


###########################
## Breaking derivation.  ##
###########################

def check_br(re, letter, exp):
    check(re, letter, exp, True)

## ---------------------------- ##
## Derive wrt a single letter.  ##
## ---------------------------- ##

# Zero, one.
check_br(   '\z', 'a', '\z')
check_br(   '\e', 'a', '\z')
check_br('<x>\e', 'a', '\z')

# Letters.
check_br(   'a', 'a', '\e')
check_br(   'a', 'b', '\z')
check_br('<x>a', 'a', '<x>\e')
check_br('<x>a', 'b', '\z')

# Sum.
check_br('<x>a+<y>b', 'a', '<x>\e')
check_br('<x>a+<y>b', 'b', '<y>\e')
check_br('<x>a+<y>a', 'a', '<x+y>\e')

# Prod.
check_br('ab', 'a', 'b')
check_br('ab', 'b', '\z')
check_br('(<x>a)(<y>a)(<z>a)', 'a', '<xy>a<z>a')

# Star.
check_br('a*', 'a', 'a*')
check_br('a*', 'b', '\z')
check_br('(<x>a)*', 'a', '<x>(<x>a)*')
check_br('(<x>a)*', 'b', '\z')
check_br('<x>a*',   'a', '<x>a*')
check_br('<x>(<y>a)*', 'a', '<xy>(<y>a)*')


## --------------------- ##
## Documented examples.  ##
## --------------------- ##

# On The Number Of Broken Derived Terms Of A Rational Expression.
ctx = vcsn.context('lal_char(ab)_b')
F2 = 'a*+b*'
E2 = "({})(a({}))".format(F2, F2)
E2t = ctx.ratexp(E2)
check(E2t, 'a', "{} + a*a({})".format(F2, F2))
check(E2t, 'b', "b*a({})".format(F2))

# Example 2.
check(E2t, 'aa', "a* + a*+b* + a*a({})".format(F2))
check(E2t, 'ab', 'b*')
check(E2t, 'ba', F2)
check(E2t, 'bb', "b*a({})".format(F2))

# Example 3.
#export VCSN_ORIGINS=1
#run 0 -f $medir/e2-dt.gv -vcsn derived-term -Ee E2t
#unset VCSN_ORIGINS

# FIXME: Support for polynomials.
# Example 4.
CHECK_EQ("a*a({}) + b*a({})".format(F2, F2),
         str(ctx.ratexp(E2).split()))
CHECK_EQ("a* + b*",
         str(ctx.ratexp(F2).split()))

# Example 5.
check_br(E2t,  'a', "a* + b* + a*a({})".format(F2))
check_br(E2t,  'b', "b*a({})".format(F2))
check_br(E2t, 'aa', "a* + b* + a*a({})".format(F2))
check_br(E2t, 'ab', 'b*')
check_br(E2t, 'ba', "a* + b*")
check_br(E2t, 'bb', "b*a({})".format(F2))
#export VCSN_ORIGINS=1
#run 0 -f $medir/e2-dt-breaking.gv -vcsn derived-term -Ee E2t 1
#unset VCSN_ORIGINS

# Figure 3.
#export VCSN_ORIGINS=1
#run 0 -f $medir/h3-dt.gv -vcsn derived-term -Ee 'a(b+c+d)'
#run 0 -f $medir/h3-dt-breaking.gv -vcsn derived-term -Ee 'a(b+c+d)' 1
#unset VCSN_ORIGINS



############################################
## Conjunction and derived-term commute.  ##
############################################

# check_conjunction RE1 RE2...
# -----------------------------
# Check derived-term(conjunction) = conjunction(derived-term).
#
# Of course we'd like to use a "are-isomorphic", but we don't have one
# currently.  Meanwhile, check equality on the first accepted words,
# and one the info of both automata.
def check_conjunction(*ratexps):
    auts = []
    rat = ctx.ratexp('\e')
    for r in ratexps:
        auts += [ctx.ratexp(r).derived_term()]
    rat = ctx.ratexp('(' + ')&('.join(ratexps) + ')')
    a1 = vcsn.automaton.product_(auts)
    a2 = rat.derived_term()
    CHECK_ISOMORPHIC(a1, a2)

ctx = vcsn.context('lal_char(abc)_q')
check_conjunction('(<1/6>a*+<1/3>b*)*', 'a*')
check_conjunction('(<1/6>a*+<1/3>b*)*', 'b*')
check_conjunction('(a+b+c)*a(a+b+c)*', '(a+b+c)*b(a+b+c)*', '(a+b+c)*c(a+b+c)*')

ctx = vcsn.context('lal_char(abc)_ratexpset<lal_char(xyz)_b>')
check_conjunction('(a+b+c)*<x>a(a+b+c)*',
                  '(a+b+c)*<y>b(a+b+c)*',
                  '(a+b+c)*<z>c(a+b+c)*')
