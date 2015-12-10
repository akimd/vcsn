#! /usr/bin/env python

import vcsn
from test import *

c = vcsn.context("lal_char(abc), expressionset<lal_char(xyz), q>")

def check(re, exp):
    '''Check that d(re) = exp.  Also check that both derived_term
    algorithms (`derivation` and `expansion`) compute the same result.
    '''
    if not isinstance(re, vcsn.expression):
        re = c.expression(re)
    eff = re.expansion()
    print("d: {} => {}".format(re, eff));
    CHECK_EQ(exp, str(eff))
    # Make sure we terminate.
    aut = re.automaton("expansion")
    # Check that if derived_term can do it, then it's the same
    # automaton.
    try:
        dt = re.automaton("derivation")
    except RuntimeError as e:
        SKIP(e)
    else:
        CHECK_ISOMORPHIC(dt, aut)

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
# A case where it would be easy not to terminate.
# The real value of this check is ensuring the termination of derived-term.
e = vcsn.context('lal_char(a), q').expression('((<2>a)*+(<4>aa)*){c}')
check(e, 'a.[((<2>a)*+<2>(a(<4>(aa))*)){c}]')
# About the same, but this time using polynomials as weights.
ctx = vcsn.context('lal_char(a), polynomialset<law_char(x), q>')
check(ctx.expression('((<x>a)*+(<xx>aa)*){c}'),       'a.[(<x>(<x>a)*+<xx>(a(<xx>aa)*)){c}]')
check(ctx.expression('((<<2>x>a)*+(<<4>xx>aa)*){c}'), 'a.[(<<2>x>(<<2>x>a)*+<<4>xx>(a(<<4>xx>aa)*)){c}]')

# Transposition.
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

c = vcsn.context("lan_char(abcd), expressionset<lal_char(xyz), q>")

# Lquotient with spontaneous transitions.
check('\e{\}\z', '<\z>')
check('\e{\}\e', '<\e>')
check('\e{\}abc', 'a.[bc]')
check('a{\}a', '<\e>')
check('a{\}b', '<\z>')

check('a{\}<x>a', '<x>')
check('<x>a{\}<y>a', '<x{\}y>')
check('<x>a{\}<x>a', '<x{\}x>')
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
E1t="(?@lal_char(ab), q)"+E1
check(E1t,  '<2> + a.[<1/3>a*{}] + b.[<2/3>b*{}]'.format(E1, E1))


############################################
## Conjunction and derived-term commute.  ##
############################################

def prod(kind, args):
    '''The product of `kind` (`conjunction`, `infiltration`, or `shuffle`)
    of `*args`.'''
    if isinstance(args[0], vcsn.automaton):
        if kind == 'conjunction':
            res = vcsn.automaton.conjunction(*args)
        elif kind == 'infiltration':
            res = vcsn.automaton._infiltration(list(args))
        elif kind == 'shuffle':
            res = vcsn.automaton._shuffle(list(args))
    else:
        res = args[0]
        for e in args[1:]:
            if kind == 'conjunction':
                res = res.conjunction(e)
            elif kind == 'infiltration':
                res = res.infiltration(e)
            elif kind == 'shuffle':
                res = res.shuffle(e)
    return res

# check_prod RE1 RE2...
# ---------------------
# Check derived-term(prod) = prod(derived-term) for the three products.
def check_prod(*exps, **kwargs):
    exps = [ctx.expression(r) for r in exps]
    auts = [r.automaton("expansion") for r in exps]
    for p in ['conjunction', 'infiltration', 'shuffle']:
        print("Product:", p)
        # Product of automata, trimmed.
        a1 = prod(p, auts).strip().trim().strip()
        # Automaton of product.
        a2 = prod(p, exps).automaton("expansion")
        if 'equiv' in kwargs:
            # FIXME: So far the infiltration product does not work
            # when there are one as labels.  Which is also when
            # "equiv" is passed.
            if p == 'infiltration':
                SKIP('infiltration does not work with \e as label')
            else:
                CHECK_EQUIV(a1, a2)
        else:
            CHECK_ISOMORPHIC(a1, a2)

ctx = vcsn.context('lal_char(abc), q')

check_prod('(<1/6>a*+<1/3>b*)*', 'a*')
check_prod('(<1/6>a*+<1/3>b*)*', 'b*')
check_prod('(a+b+c)*a(a+b+c)*', '(a+b+c)*b(a+b+c)*', '(a+b+c)*c(a+b+c)*')

ctx = vcsn.context('lal_char(abc), expressionset<lal_char(xyz), b>')
check_prod('<x>\e', '<y>\e')


check_prod('<x>a', '<y>a')
check_prod('<x>a', '<y>b')
check_prod('<x>a', '<y>a', '<z>a')
check_prod('<x>a', '<y>b', '<z>c')

check_prod('(a+b+c)*<x>a(a+b+c)*',
           '(a+b+c)*<y>b(a+b+c)*',
           '(a+b+c)*<z>c(a+b+c)*')

# Use ab{\}ab to introduce expansions with the empty word as label.
ctx = vcsn.context('lan_char(abcd), q')
check_prod(r'(cd{\}cd)',  r'(cd{\}cd)', equiv = True)
check_prod(r'a(cd{\}cd)', r'a(cd{\}cd)', equiv = True)
check_prod(r'(cd{\}cd)a', r'(cd{\}cd)a', equiv = True)
check_prod(r'a(cd{\}cd)', r'(cd{\}cd)a', equiv = True)

check_prod(r'a(cd{\}cd)', r'b(cd{\}cd)', equiv = True)
check_prod(r'(cd{\}cd)a', r'(cd{\}cd)b', equiv = True)
check_prod(r'a(cd{\}cd)', r'(cd{\}cd)b', equiv = True)

check_prod(r'(cd{\}cd)[ab]', r'a(cd{\}cd+b)', equiv = True)
check_prod(r'(cd{\}cd)[ab]', r'a(cd{\}cd+b)', equiv = True)
check_prod(r'<1/10>(cd{\}<1/2>cd+a)<2>', '<1/20>(cd{\}<1/3>cd+a)<3>',
           equiv = True)

## ----------------- ##
## LaTeX rendering.  ##
## ----------------- ##

x = c.expression(r'aa+<x>abc+<y>bc').expansion()
CHECK_EQ(r'a \odot \left[a \oplus \left\langle x\right\rangle b \, c\right] \oplus b \odot \left[\left\langle y\right\rangle c\right]',
         x.format("latex"))

CHECK_EQ(r'a⊙[a⊕⟨x⟩bc] ⊕ b⊙[⟨y⟩c]',
         x.format("utf8"))


## ------------ ##
## On wordset.  ##
## ------------ ##

c = vcsn.context("law_char(a-z), expressionset<lal_char(xyz), q>")

# Transposition is the most risky one, as we must not forget to
# transpose the labels in the expansion.
check('\z{T}', '<\z>')
check('\e{T}', '<\e>')
check('a{T}', 'a.[\e]')
check('(abc){T}', 'cba.[\e]')
check('(abc+aabbcc){T}', 'cba.[\e] + ccbbaa.[\e]')
check('(<xy>abc<yz>){T}', 'c.[<zy>(<xy>(ab)){T}]')
check('((foo)(bar)(baz)){T}', 'zab.[((foo)(bar)){T}]')
check('(ab)*{T}', '<\e> + ba.[(ab)*{T}]')
check('(<xy>((abc)(abc))<yz>)*{T}',
      '<\e> + cba.[<zy><yx>(cba)(<xy>((abc){2})<yz>)*{T}]')
check('a*b* & (ab)*',
      '<\e> + a.[a*b*&b(ab)*]')
check('(<x>a)*(<y>b)* & (<z>ab)*',
      '<\e> + a.[<xz>(<x>a)*(<y>b)*&b(<z>(ab))*]')


## ---------- ##
## tupleset.  ##
## ---------- ##

c = vcsn.context("lat<law_char(abc), law_char(xyz)>, expressionset<lal_char(XYZ), q>")
check('(abc|xyz) & (a|xy)*(bc|z)*',
      'a|xy.[bc|z&(a|xy)*(bc|z)*]')
check('(<X>abc|xyz) & (<Y>a|xy)*(<Z>bc|z)*',
      'a|xy.[<XY>bc|z&(<Y>a|xy)*(<Z>(bc)|z)*]')

c = vcsn.context("lat<lan<char(abc)>, lan<char(xyz)>>, q")
check('\e|\e',
      '<1>')
check('a|x',
      'a|x.[\e]')
check('a*|x*',
      '<1> + \e|x.[\e|x*] + a|\e.[a*|\e] + a|x.[a*|x*]')

c = vcsn.context("lat<lal<char(abc)>, lal<char(xyz)>>, q")
check('\e|\e',
      '<1>')
check('a|x',
      'a|x.[\e]')
XFAIL(lambda: c.expression('a*|x*').expansion(),
      'to-expansion: cannot denormalize')

# Check the tupling of expansions: d(e|f) = d(e) | d(f).
c = vcsn.context('lan<char>, q')
def check(e1, e2):
    e1 = c.expression(e1)
    e2 = c.expression(e2)
    e = e1 | e2
    x1 = e1.expansion()
    x2 = e2.expansion()
    x = x1 | x2
    CHECK_EQ(x, e.expansion())
check('a', 'x')
check('a?', 'x?')
check('a*', 'x*')
check('<2>(<2>a)*', '<3>(<3>x)*')
