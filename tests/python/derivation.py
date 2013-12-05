#! /usr/bin/env python

import vcsn

count = 0
c = vcsn.context("lal_char(abc)_ratexpset<lal_char(xyz)_z>")

def check(re, letter, res):
    global count
    count += 1
    if re[:3] <> "(?@":
        re = "(?@lal_char(abc)_ratexpset<lal_char(xyz)_z>)" + re
    r = c.ratexp(re)
    print r, "/d",letter, " = ", r.derivation(letter)
    if str(r.derivation(letter)) == res:
        print 'ok ', count;
    else:
        print 'not ok ', count, r.derivation(letter), "!=", res;

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
check('(<x>a).(<y>a).(<z>a)', 'a', '<x><y>a.<z>a')

# Intersection.
check('<x>a&<y>a&<z>a', 'a', '<x.y.z>\e')
check('<x>a&<y>a&<z>a', 'b', '\z')

check('(<x>a+<y>b)*&(<z>b+<x>c)*', 'a', '\z')
check('(<x>a+<y>b)*&(<z>b+<x>c)*', 'b', '<y.z>(<x>a+<y>b)*&(<z>b+<x>c)*')
check('(<x>a+<y>b)*&(<z>b+<x>c)*', 'c', '\z')

# Shuffle.
check('<x>a{:}<y>a{:}<z>a', 'a', '<x>\e{:}<y>a{:}<z>a + <y><x>a{:}\e{:}<z>a + <z><x>a{:}<y>a{:}\e')
check('<x>a{:}<y>b{:}<z>c', 'a', '<x>\e{:}<y>b{:}<z>c')
check('<x>a{:}<y>b{:}<z>c', 'b', '<y><x>a{:}\e{:}<z>c')
check('<x>a{:}<y>b{:}<z>c', 'c', '<z><x>a{:}<y>b{:}\e')

check('(<x>a<y>b)*{:}(<x>a<x>c)*',
      'a', '<x><y>b.(<x>a.<y>b)*{:}(<x>a.<x>c)* + <x>(<x>a.<y>b)*{:}<x>c.(<x>a.<x>c)*')
check('(<x>a<y>b)*{:}(<x>a<x>c)*', 'b', '\z')
check('(<x>a<y>b)*{:}(<x>a<x>c)*', 'c', '\z')

# Star.
check('a*', 'a', 'a*')
check('a*', 'b', '\z')
check('(<x>a)*', 'a', '<x>(<x>a)*')
check('(<x>a)*', 'b', '\z')
check('<x>a*',   'a', '<x>a*')
check('<x>(<y>a)*', 'a', '<x.y>(<y>a)*')


## ------------------------------- ##
## Derive wrt to several letters.  ##
## ------------------------------- ##

check('(<x>a)*', 'aa',   '<x.x>(<x>a)*')
check('(<x>a)*', 'aaaa', '<x.x.x.x>(<x>a)*')
check('(<x>a)*', 'aaab', '\z')

check('(<x>a)*(<y>b)*', 'aa',   '<x.x>(<x>a)*.(<y>b)*')
check('(<x>a)*(<y>b)*', 'aabb', '<x.x.y.y>(<y>b)*')


## -------------------- ##
## Classical examples.  ##
## -------------------- ##

# EAT, Example 4.3.
E1='(<1/6>a*+<1/3>b*)*'
# E1 typed.
E1t="(?@lal_char(ab)_q)"+E1
check(E1t,  'a',  "<1/3>a*."+E1)
check(E1t,  'b',  "<2/3>b*."+E1)
check(E1t, 'aa',  "<4/9>a*."+E1)
check(E1t, 'ab',  "<2/9>b*."+E1)
check(E1t, 'ba',  "<2/9>a*."+E1)
check(E1t, 'bb', "<10/9>b*."+E1)

print '1..'+str(count)
