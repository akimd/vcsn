#! /usr/bin/env python

import vcsn
from test import *

c = vcsn.context("lal_char(abc)_ratexpset<lal_char(xyz)_z>")

def is_wordset(c):
    return str(c).startswith("law_")

def check(re, exp, use_spontaneous = False):
    '''Check that fo(re) = exp.  Also check that both derived_term algorithms
    (`derivation` and `expansion`) compute the same result.
    '''
    r = c.ratexp(re)
    eff = r.first_order()
    print("d: {} => {}".format(r, eff));
    CHECK_EQ(exp, str(eff))
    # Check that if derived_term can do it, them it's the same
    # automaton.
    if not use_spontaneous and not is_wordset(c):
        try:
            dt = r.derived_term("derivation")
        except:
            pass
        else:
            CHECK_ISOMORPHIC(dt, r.derived_term("expansion"))

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
check('<x>a:<y>a:<z>a', 'a.[<z><x>a:<y>a + <y><x>a:<z>a + <x><y>a:<z>a]')
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



## ------------------------------ ##
## With spontaneous transitions.  ##
## ------------------------------ ##

c = vcsn.context("lan_char(abcd)_ratexpset<lal_char(xyz)_z>")

# Lquotient with spontaneous transitions.
check('\e{\}\z', '<\z>')
check('\e{\}\e', '<\e>')
check('\e{\}abc', 'a.[bc]')
check('a{\}a', '<\e>')
check('a{\}b', '<\z>')

check('a{\}<x>a', '<x>')
check('<x>a{\}<y>a', '<x{\}y>')
check('a{\}(<x>a)*', '\e.[<x>(<x>a)*]')
check('a*{\}a', '\e.[a*{\}\e] + a.[\e]')
check('a*{\}a*', '<\e> + \e.[a*{\}a*] + a.[a*]')
check('(<x>a)*{\}(<y>a)*', '<\e> + \e.[<x{\}y>(<x>a)*{\}(<y>a)*] + a.[<y>(<y>a)*]')

# Left quotient vs. conjunction.
check('(ab{\}ab)c&c', '\e.[(b{\}b)c&c]')

# Right quotient with spontaneous transitions.
check('\z{/}\e', '<\z>')
check('\e{/}\e', '<\e>')
check('a{/}a', '<\e>')
check('a{/}b', '<\z>')
check('abcd{/}\e', 'a.[bcd]')
check('abcd{/}d', '\e.[(abc){T}{T}]')
check('abcd{/}cd', '\e.[(c{\}(abc){T}){T}]')
check('abcd{/}bcd', '\e.[((bc){T}{\}(abc){T}){T}]')
check('abcd{/}abcd', '\e.[((abc){T}{\}(abc){T}){T}]')

check('(<x>a){/}a', '\e.[(<x>\e){T}]')
check('<x>a{/}<y>a', '\e.[(<y>\e{\}<x>\e){T}]')
check('a{/}(<x>a)*', '\e.[(<x>(<x>a)*{T}{\}\e){T}] + a.[\e]')
# I don't know for sure this is right :(
check('(<x>a)*{/}(a)*',
'<\e> + \e.[(a*{T}{\}<x>(<x>a)*{T}){T}] + a.[<x>(<x>a)*]')
check('a*{/}a', '\e.[a*{T}{T}]')
check('a*{/}a*', '<\e> + \e.[(a*{T}{\}a*{T}){T}] + a.[a*]')
# I don't know for sure this is right :(
check('(<x>a)*{/}(<y>a)*',
'<\e> + \e.[(<y>(<y>a)*{T}{\}<x>(<x>a)*{T}){T}] + a.[<x>(<x>a)*]')

# Right quotient vs. conjunction.
check('(ab{/}ab)c&c', '\e.[(a{\}a){T}c&c]')



## -------------------- ##
## Classical examples.  ##
## -------------------- ##

# EAT, Example 4.3.
E1='(<1/6>a*+<1/3>b*)*'
# E1 typed.
E1t="(?@lal_char(ab)_q)"+E1
check(E1t,  '<2> + a.[<1/3>a*{}] + b.[<2/3>b*{}]'.format(E1, E1))


############################################
## Conjunction and derived-term commute.  ##
############################################

# check_conjunction RE1 RE2...
# -----------------------------
# Check derived-term(conjunction) = conjunction(derived-term).
def check_conjunction(*ratexps, **kwargs):
    rat = None
    auts = []
    for r in ratexps:
        exp = ctx.ratexp(r)
        if rat is None:
            rat = exp
        else:
            rat &= exp
        auts += [exp.derived_term("expansion")]
    # Product of automata.
    a1 = vcsn.automaton._product(auts)
    # Automaton of product.
    a2 = rat.derived_term("expansion")
    if 'equiv' in kwargs:
        CHECK_EQUIV(a1, a2)
    else:
        CHECK_ISOMORPHIC(a1, a2)

ctx = vcsn.context('lal_char(abc)_q')
check_conjunction('(<1/6>a*+<1/3>b*)*', 'a*')
check_conjunction('(<1/6>a*+<1/3>b*)*', 'b*')
check_conjunction('(a+b+c)*a(a+b+c)*', '(a+b+c)*b(a+b+c)*', '(a+b+c)*c(a+b+c)*')

ctx = vcsn.context('lal_char(abc)_ratexpset<lal_char(xyz)_b>')
check_conjunction('(a+b+c)*<x>a(a+b+c)*',
                  '(a+b+c)*<y>b(a+b+c)*',
                  '(a+b+c)*<z>c(a+b+c)*')

# Use ab{\}ab to introduce expansions with the empty word as label.
ctx = vcsn.context('lan_char(abc)_q')
check_conjunction(r'(ab{\}ab)a', r'a(ab{\}ab)', equiv = True)
check_conjunction(r'(ab{\}ab)[ab]', r'a(ab{\}ab+b)', equiv = True)
check_conjunction(r'(ab{\}ab)[ab]', r'a(ab{\}ab+b)', equiv = True)
check_conjunction(r'<1/10>(ab{\}<1/2>ab+c)<2>', '<1/20>(ab{\}<1/3>ab+c)<3>',
                  equiv = True)

## ----------------- ##
## LaTeX rendering.  ##
## ----------------- ##

CHECK_EQ(r'a \odot \left[a \oplus \left\langle x\right\rangle b \, c\right] \oplus b \odot \left[\left\langle y\right\rangle c\right]',
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
