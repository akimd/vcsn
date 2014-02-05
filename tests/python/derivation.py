#! /usr/bin/env python

from __future__ import print_function

import vcsn
from test import *

c = vcsn.context("lal_char(abc)_ratexpset<lal_char(xyz)_z>")

def check(re, s, exp, breaking = False):
    "Check that d/ds(re) = exp."
    r = c.ratexp(re)
    eff = r.derivation(s, breaking)
    print("d/d{}({}) = {}".format(s, r, eff));
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

# Intersection.
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

PLAN()
