#! /usr/bin/env python

import vcsn
from test import *

c = vcsn.context("lal_char(abc)_ratexpset<lal_char(xyz)_z>")

def is_wordset(c):
    return str(c).startswith("law_")

def check(re, exp, use_spontaneous = False, no_linear = False):
    '''Check that fo(re) = exp.  Also check that linear and derived_term
    compute the same result, unless no_linear = True.  `no_linear` exists
    for a bad reason: our isomorphism check fails.
    '''
    r = c.ratexp(re)
    eff = r.first_order(use_spontaneous)
    print("d: {} => {}".format(r, eff));
    CHECK_EQ(exp, str(eff))
    # Check that if derived_term can do it, them it's the same
    # automaton.
    if not use_spontaneous and not no_linear and not is_wordset(c):
        try:
            dt = r.derived_term()
        except:
            pass
        else:
            CHECK_ISOMORPHIC(dt, r.linear())

##########################
## Regular derivation.  ##
##########################

# Zero, one.
check(   '\z', '<\z>')
check(   '\e', '<\e>')
check('<x>\e', '<x>')

# Letters.
check(   'a', 'a.[\e]')
check('<x>a', 'a.[<x>\e]')

# Sum.
check('<x>a+<y>b', 'a.[<x>\e] + b.[<y>\e]')
check('<x>a+<y>a', 'a.[<x+y>\e]')

# Prod.
check('ab', 'a.[b]')
check('(<x>a).(<y>a).(<z>a)', 'a.[<x><y>a<z>a]')

# Conjunction.
check('<x>a&<y>a&<z>a', 'a.[<xyz>\e]')
check('(<x>a+<y>b)*&(<z>b+<x>c)*', '<\e> + b.[<yz>(<x>a+<y>b)*&(<z>b+<x>c)*]')

# Shuffle.
# FIXME: CHECK_ISOMORPHIC fails to see both results are the same:
#
#   --- expected
#   +++ effective
#   @@ -1 +1 @@
#   -<xyz+xzy+yxz+yzx+zxy+zyx>aaa
#   +<zyx+zxy+yzx+yxz+xzy+xyz>aaa
check('<x>a:<y>a:<z>a', 'a.[<z><x>a:<y>a + <y><x>a:<z>a + <x><y>a:<z>a]',
      no_linear = True)
check('(<x>a<y>b)*:(<x>a<x>c)*', '<\e> + a.[<x>(<x>a<y>b)*:<x>c(<x>a<x>c)* + <x><y>b(<x>a<y>b)*:(<x>a<x>c)*]')

# Star.
check('a*', '<\e> + a.[a*]')
check('(<x>a)*', '<\e> + a.[<x>(<x>a)*]')
check('<x>a*', '<x> + a.[<x>a*]')
check('<x>(<y>a)*', '<x> + a.[<xy>(<y>a)*]')
check('(<x>a)*<y>', '<y> + a.[<x>(<x>a)*<y>]')

# Complement.
check('\z{c}', '<\e> + a.[\z{c}] + b.[\z{c}] + c.[\z{c}]')
check('\e{c}', 'a.[\z{c}] + b.[\z{c}] + c.[\z{c}]')
check('a{c}', '<\e> + a.[\e{c}] + b.[\z{c}] + c.[\z{c}]')
check('(a+b){c}', '<\e> + a.[\e{c}] + b.[\e{c}] + c.[\z{c}]')
check('(a.b){c}', '<\e> + a.[b{c}] + b.[\z{c}] + c.[\z{c}]')
check('(a:b){c}', '<\e> + a.[b{c}] + b.[a{c}] + c.[\z{c}]')
check('(a*&a*){c}', 'a.[(a*&a*){c}] + b.[\z{c}] + c.[\z{c}]')
check('(<x>(<y>a)*<z>){c}', 'a.[(<y>a)*{c}] + b.[\z{c}] + c.[\z{c}]')
check('a{c}{c}', 'a.[\e{c}{c}] + b.[\z{c}{c}] + c.[\z{c}{c}]')

# Transposition
check('\z{T}', '<\z>')
check('\e{T}', '<\e>')
check('a{T}', 'a.[\e]')
check('(abc){T}', 'c.[(ab){T}]')
check('(abc+aabbcc){T}', 'c.[(ab){T} + (aabbc){T}]')
check('(<xy>abc<yz>){T}', 'c.[<zy>(<xy>ab){T}]')

check('(ab)*{T}', '<\e> + b.[a(ab)*{T}]')
check('(<xy>(abc)<yz>)*{T}', '<\e> + c.[<zy>(ab){T}<yx>(<xy>(abc)<yz>)*{T}]')

# Lquotient
check('\e{\}\z', '<\z>')
check('\e{\}\e', '<\e>')
check('\e{\}abc', 'a.[bc]')
check('a{\}a', '<\e>')
check('a{\}b', '<\z>')

check('a{\}<x>a', '<x>')
check('<x>a{\}<y>a', '<x{\}y>')
check('a{\}(<x>a)*', '<x> + a.[<xx>(<x>a)*]')
check('a*{\}a', '<\e> + a.[\e]')
check('a*{\}a*', '<\e*> + a.[<\e*>a*]')
check('(<x>a)*{\}(<y>a)*', '<(x{\}y)*> + a.[<(x{\}y)*y>(<y>a)*]')

# Right quotient.
check('\z{/}\e', '<\z>')
check('\e{/}\e', '<\e>')
check('abc{/}\e', 'a.[bc]')
check('a{/}a', '<\e>')
check('a{/}b', '<\z>')

check('(<x>a){/}a', '<x>')
check('<x>a{/}<y>a', '<y{\}x>')
check('a{/}(<x>a)*', '<x{\}\e> + a.[\e]')
# I don't know for sure this is right :(
check('(<x>a)*{/}(a)*',
'<x*> + a.[<x>(<x>a){T}*{T}(a{\}(<x>a){T})*{T}]')
check('a*{/}a', '<\e> + a.[a*]')
check('a*{/}a*', '<\e*> + a.[a*{T}(a{\}a)*{T}]')
# I don't know for sure this is right :(
check('(<x>a)*{/}(<y>a)*',
'<(y{\}x){T}*> + a.[<x>(<x>a){T}*{T}((<y>a){T}{\}(<x>a){T})*{T}]')


## ------------------------------ ##
## With spontaneous transitions.  ##
## ------------------------------ ##

c = vcsn.context("lan_char(abc)_ratexpset<lal_char(xyz)_z>")

# Lquotient with spontaneous transitions.
check('\e{\}\z', '<\z>', True)
check('\e{\}\e', '<\e>', True)
check('\e{\}abc', 'a.[bc]', True)
check('a{\}a', '\e.[\e]', True)
check('a{\}b', '<\z>', True)

check('a{\}<x>a', '\e.[<x>\e]', True)
check('<x>a{\}<y>a', '\e.[<x{\}y>\e]', True)
check('a{\}(<x>a)*', '\e.[<x>(<x>a)*]', True)
check('a*{\}a', '\e.[a*{\}\e] + a.[\e]', True)
check('a*{\}a*', '<\e> + \e.[a*{\}a*] + a.[a*]', True)
check('(<x>a)*{\}(<y>a)*', '<\e> + \e.[<x{\}y>(<x>a)*{\}(<y>a)*] + a.[<y>(<y>a)*]', True)

# Right quotient with spontaneous transitions.
check('\z{/}\e', '<\z>', True)
check('\e{/}\e', '<\e>', True)
check('abc{/}\e', 'a.[bc]', True)
check('a{/}a', '\e.[\e]', True)
check('a{/}b', '<\z>', True)

check('(<x>a){/}a', '\e.[<x>\e]', True)
check('<x>a{/}<y>a', '\e.[<y>\e{\}<x>\e]', True)
check('a{/}(<x>a)*', '\e.[<x>(<x>a)*{T}{\}\e] + a.[\e]', True)
# I don't know for sure this is right :(
check('(<x>a)*{/}(a)*',
'<\e> + \e.[a*{T}{\}<x>(<x>a)*{T}] + a.[<x>(<x>a)*]', True)
check('a*{/}a', '\e.[a*{T}]', True)
check('a*{/}a*', '<\e> + \e.[a*{T}{\}a*{T}] + a.[a*]', True)
# I don't know for sure this is right :(
check('(<x>a)*{/}(<y>a)*',
'<\e> + \e.[<y>(<y>a)*{T}{\}<x>(<x>a)*{T}] + a.[<x>(<x>a)*]', True)


## -------------------- ##
## Classical examples.  ##
## -------------------- ##

# EAT, Example 4.3.
E1='(<1/6>a*+<1/3>b*)*'
# E1 typed.
E1t="(?@lal_char(ab)_q)"+E1
check(E1t,  '<2> + a.[<1/3>a*{}] + b.[<2/3>b*{}]'.format(E1, E1))


## ----------------- ##
## LaTeX rendering.  ##
## ----------------- ##

CHECK_EQ(r'a \odot \left[a \oplus \langle x\rangle b \, c\right] \oplus b \odot \left[\langle y\rangle c\right]',
         c.ratexp(r'aa+<x>abc+<y>bc').first_order().format("latex"))


## ------------ ##
## On wordset.  ##
## ------------ ##

c = vcsn.context("law_char(a-z)_ratexpset<lal_char(xyz)_z>")

# Transposition is the most risky one, as we must not forget to
# transpose the labels in the expansion.
check('\z{T}', '<\z>')
check('\e{T}', '<\e>')
check('a{T}', 'a.[\e]')
check('(abc){T}', 'cba.[\e]')
check('(abc+aabbcc){T}', 'cba.[\e] + ccbbaa.[\e]')
check('(<xy>abc<yz>){T}', 'c.[<zy>(<xy>ab){T}]')
check('((foo)(bar)(baz)){T}', 'zab.[((foo)(bar)){T}]')
check('(ab)*{T}', '<\e> + ba.[(ab)*{T}]')
check('(<xy>((abc)(abc))<yz>)*{T}',
      '<\e> + cba.[<zy><yx>(cba)(<xy>((abc)(abc))<yz>)*{T}]')
