#! /usr/bin/env python

import vcsn
from test import *

c = vcsn.context("lal_char(abc)_ratexpset<lal_char(xyz)_z>")

def check(re, exp, use_spontaneous = False):
    "Check that fo(re) = exp."
    r = c.ratexp(re)
    eff = r.first_order(use_spontaneous)
    print("first_order({}) = {}".format(r, eff));
    CHECK_EQ(exp, str(eff))


##########################
## Regular derivation.  ##
##########################

# Zero, one.
check(   '\z', '\z')
check(   '\e', '\e')
check('<x>\e', '<x>\e')

# Letters.
check(   'a', 'a')
check('<x>a', '<x>a')

# Sum.
check('<x>a+<y>b', '<x>a + <y>b')
check('<x>a+<y>a', '<x+y>a')

# Prod.
check('ab', 'ab')
check('(<x>a).(<y>a).(<z>a)', 'a<x>(<y>a<z>a)')

# Intersection.
check('<x>a&<y>a&<z>a', '<xyz>a')
check('(<x>a+<y>b)*&(<z>b+<x>c)*', '\e + b<yz>((<x>a+<y>b)*&(<z>b+<x>c)*)')

# Shuffle.
check('<x>a:<y>a:<z>a', 'a(<z>(<x>a:<y>a)+<y>(<x>a:<z>a)+<x>(<y>a:<z>a))')
check('(<x>a<y>b)*:(<x>a<x>c)*', '\e + a(<x>((<x>a<y>b)*:<x>c(<x>a<x>c)*)+<x>(<y>b(<x>a<y>b)*:(<x>a<x>c)*))')

# Star.
check('a*', '\e + aa*')
check('(<x>a)*', '\e + a<x>(<x>a)*')
check('<x>a*', '<x>\e + a<x>a*')
check('<x>(<y>a)*', '<x>\e + a<xy>(<y>a)*')
check('(<x>a)*<y>', '<y>\e + a<x>(<x>a)*<y>')

# Complement.
check('\z{c}', '\e + a\z{c} + b\z{c} + c\z{c}')
check('\e{c}', 'a\z{c} + b\z{c} + c\z{c}')
check('a{c}', '\e + a\e{c} + b\z{c} + c\z{c}')
check('(a+b){c}', '\e + a\e{c} + b\e{c} + c\z{c}')
check('(a.b){c}', '\e + ab{c} + b\z{c} + c\z{c}')
check('(a:b){c}', '\e + ab{c} + ba{c} + c\z{c}')
check('(a*&a*){c}', 'b\z{c} + c\z{c} + a(a*&a*){c}')
check('(<x>(<y>a)*<z>){c}', 'b\z{c} + c\z{c} + a(<y>a)*{c}')
check('a{c}{c}', 'a\e{c}{c} + b\z{c}{c} + c\z{c}{c}')

# Transposition
check('\z{T}', '\z')
check('\e{T}', '\e')
check('a{T}', 'a')
check('(abc){T}', 'cba')
check('(abc+aabbcc){T}', 'c(ba+c(aabb){T})')
check('(<xy>abc<yz>){T}', 'c<zy>(ba)<yx>')

check('(ab)*{T}', '\e + ba(ab)*{T}')
check('(<xy>abc<yz>)*{T}', '\e + c<zy>((ba)<yx>(<xy>(abc)<yz>)*{T})')

# Lquotient
check('\e{\}\z', '\z')
check('\e{\}\e', '\e')
check('\e{\}abc', 'abc')
check('a{\}a', '\e')
check('a{\}b', '\z')

check('a{\}<x>a', '<x>\e')
check('<x>a{\}<y>a', '<x{\}y>\e')
check('a{\}(<x>a)*', '<x>\e + a<xx>(<x>a)*')
check('a*{\}a', '\e + a')
check('a*{\}a*', '<\e*>\e + a<\e*>a*')
check('(<x>a)*{\}(<y>a)*', '<(x{\}y)*>\e + a<(x{\}y)*y>(<y>a)*')

# Right quotient.
check('\z{/}\e', '\z')
check('\e{/}\e', '\e')
check('abc{/}\e', 'abc')
check('a{/}a', '\e')
check('a{/}b', '\z')

check('(<x>a){/}a', '<x>\e')
check('<x>a{/}<y>a', '<y{\}x>\e')
check('a{/}(<x>a)*', '<x{\}\e>\e + a')
# I don't know for sure this is right :(
check('(<x>a)*{/}(a)*',
'<x*>\e + a<x>((<x>a){T}*{T}(a{\}(<x>a){T})*{T})')
check('a*{/}a', '\e + aa*')
check('a*{/}a*', '<\e*>\e + aa*{T}(a{\}a)*{T}')
# I don't know for sure this is right :(
check('(<x>a)*{/}(<y>a)*',
'<(y{\}x){T}*>\e + a<x>((<x>a){T}*{T}((<y>a){T}{\}(<x>a){T})*{T})')


## ------------------------------ ##
## With spontaneous transitions.  ##
## ------------------------------ ##

c = vcsn.context("lan_char(abc)_ratexpset<lal_char(xyz)_z>")

# Lquotient with spontaneous transitions.
check('\e{\}\z', '\z', True)
check('\e{\}\e', '\e', True)
check('\e{\}abc', 'abc', True)
check('a{\}a', '\e', True)
check('a{\}b', '\z', True)

check('a{\}<x>a', '<x>\e', True)
check('<x>a{\}<y>a', '<x{\}y>\e', True)
check('a{\}(<x>a)*', '<x>(<x>a)*', True)
check('a*{\}a', 'a + a*{\}\e', True)
check('a*{\}a*', '\e + aa* + a*{\}a*', True)
check('(<x>a)*{\}(<y>a)*', '\e + a<y>(<y>a)* + <x{\}y>((<x>a)*{\}(<y>a)*)', True)

# Right quotient with spontaneous transitions.
check('\z{/}\e', '\z', True)
check('\e{/}\e', '\e', True)
check('abc{/}\e', 'abc', True)
check('a{/}a', '\e', True)
check('a{/}b', '\z', True)

check('(<x>a){/}a', '<x>\e', True)
check('<x>a{/}<y>a', '<y>\e{\}<x>\e', True)
check('a{/}(<x>a)*', 'a + <x>(<x>a)*{T}{\}\e', True)
# I don't know for sure this is right :(
check('(<x>a)*{/}(a)*',
'\e + a<x>(<x>a)* + a*{T}{\}<x>(<x>a)*{T}', True)
check('a*{/}a', 'a*{T}', True)
check('a*{/}a*', '\e + aa* + a*{T}{\}a*{T}', True)
# I don't know for sure this is right :(
check('(<x>a)*{/}(<y>a)*',
'\e + a<x>(<x>a)* + <y>(<y>a)*{T}{\}<x>(<x>a)*{T}', True)


## -------------------- ##
## Classical examples.  ##
## -------------------- ##

# EAT, Example 4.3.
E1='(<1/6>a*+<1/3>b*)*'
# E1 typed.
E1t="(?@lal_char(ab)_q)"+E1
check(E1t,  '<2>\e + a<1/3>(a*{}) + b<2/3>(b*{})'.format(E1, E1))
