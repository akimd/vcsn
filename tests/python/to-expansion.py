#! /usr/bin/env python

from re import match
import vcsn
from test import *

c = vcsn.context("[abc] -> expressionset<[xyz] -> q>")

def check(re, exp):
    '''Check that d(re) = exp.  Also check that both derived_term
    algorithms (`derivation` and `expansion`) compute the same result.
    '''
    if not isinstance(re, vcsn.expression):
        re = c.expression(re)
    eff = re.expansion()
    print("d: {:u} => {:u}".format(re, eff));
    CHECK_EQ(exp, eff.format('text'))
    # Make sure we terminate.
    aut = re.automaton("expansion")
    # Check that if derived_term can do it, then it's the same
    # automaton.
    if match('^[^,]*wordset', re.context().format('sname')):
        XFAIL(lambda: re.automaton("derivation"),
              'derived_term: cannot use derivation on non-letterized labelsets')
    elif re.info('ldivide'):
        XFAIL(lambda: re.automaton("derivation"),
              'operator ldivide not supported')
    elif re.info('transposition'):
        XFAIL(lambda: re.automaton("derivation"),
              'operator transposition not supported')
    else:
        dt = re.automaton("derivation")
        CHECK_ISOMORPHIC(dt, aut)

##########################
## Regular derivation.  ##
##########################

# Zero, one.
check(   r'\z', r'<\z>')
check(   r'\e', r'<\e>')
check(r'<x>\e', '<x>')

# Letters.
check(   'a', r'a.[\e]')
check('<x>a', r'a.[<x>\e]')

# Sum.
check('<x>a+<y>b', r'a.[<x>\e] + b.[<y>\e]')
check('<x>a+<y>a', r'a.[<x+y>\e]')

# Prod.
check('ab', 'a.[b]')
check('(<x>a).(<y>a).(<z>a)', 'a.[<x><y>a<z>a]')

# Conjunction.
check('<x>a&<y>a&<z>a', r'a.[<xyz>\e]')
check('(<x>a+<y>b)*&(<z>b+<x>c)*', r'<\e> + b.[<yz>(<x>a+<y>b)*&(<z>b+<x>c)*]')

# Shuffle.
# FIXME: CHECK_ISOMORPHIC fails to see both results are the same:
#
#   --- expected
#   +++ effective
#   @@ -1 +1 @@
#   -<xyz+xzy+yxz+yzx+zxy+zyx>aaa
#   +<zyx+zxy+yzx+yxz+xzy+xyz>aaa
check('<x>a:<y>a:<z>a', 'a.[<z><x>a:<y>a + <y><x>a:<z>a + <x><y>a:<z>a]')
check('(<x>a<y>b)*:(<x>a<x>c)*', r'<\e> + a.[<x>(<x>a<y>b)*:<x>c(<x>a<x>c)* + <x><y>b(<x>a<y>b)*:(<x>a<x>c)*]')

# Star.
check('a*', r'<\e> + a.[a*]')
check('(<x>a)*', r'<\e> + a.[<x>(<x>a)*]')
check('<x>a*', '<x> + a.[<x>a*]')
check('<x>(<y>a)*', '<x> + a.[<xy>(<y>a)*]')
check('(<x>a)*<y>', '<y> + a.[<x>(<x>a)*<y>]')

# Complement.
check(r'\z{c}', r'<\e> + a.[\z{c}] + b.[\z{c}] + c.[\z{c}]')
check(r'\e{c}', r'a.[\z{c}] + b.[\z{c}] + c.[\z{c}]')
check('a{c}', r'<\e> + a.[\e{c}] + b.[\z{c}] + c.[\z{c}]')
check('(a+b){c}', r'<\e> + a.[\e{c}] + b.[\e{c}] + c.[\z{c}]')
check('(a.b){c}', r'<\e> + a.[b{c}] + b.[\z{c}] + c.[\z{c}]')
check('(a:b){c}', r'<\e> + a.[b{c}] + b.[a{c}] + c.[\z{c}]')
check('(a*&a*){c}', r'a.[(a*&a*){c}] + b.[\z{c}] + c.[\z{c}]')
check('(<x>(<y>a)*<z>){c}', r'a.[(<y>a)*{c}] + b.[\z{c}] + c.[\z{c}]')
check('a{c}{c}', r'a.[\e{c}{c}] + b.[\z{c}{c}] + c.[\z{c}{c}]')
# A case where it would be easy not to terminate.
# The real value of this check is ensuring the termination of derived-term.
e = vcsn.context('[a] -> q').expression('((<2>a)*+(<4>aa)*){c}')
check(e, 'a.[((<2>a)*+<2>(a(<4>(aa))*)){c}]')
# About the same, but this time using polynomials as weights.
ctx = vcsn.context('[a] -> polynomialset<[x]* -> q>')
check(ctx.expression('((<x>a)*+(<xx>aa)*){c}'),       'a.[(<x>(<x>a)*+<xx>(a(<xx>aa)*)){c}]')
check(ctx.expression('((<<2>x>a)*+(<<4>xx>aa)*){c}'), 'a.[(<<2>x>(<<2>x>a)*+<<4>xx>(a(<<4>xx>aa)*)){c}]')

# Transposition.
check(r'\z{T}', r'<\z>')
check(r'\e{T}', r'<\e>')
check('a{T}', r'a.[\e]')
check('(abc){T}', 'c.[(ab){T}]')
check('(abc+aabbcc){T}', 'c.[(ab){T} + (aabbc){T}]')
check('(<xy>abc<yz>){T}', 'c.[<zy>(<xy>ab){T}]')

check('(ab)*{T}', r'<\e> + b.[a(ab)*{T}]')
check('(<xy>(abc)<yz>)*{T}', r'<\e> + c.[<zy>(ab){T}<yx>(<xy>(abc)<yz>)*{T}]')



## ------------------------------ ##
## With spontaneous transitions.  ##
## ------------------------------ ##

c = vcsn.context("[abcd] -> expressionset<[xyz] -> q>")

# Ldivide with spontaneous transitions.
check(r'\e{\}\z', r'<\z>')
check(r'\e{\}\e', r'<\e>')
check(r'\e{\}abc', 'a.[bc]')
check(r'a{\}a', r'<\e>')
check(r'a{\}b', r'<\z>')

# Complement on a non-free letterized labelsets.
check('!a', r'<\e> + a.[\e{c}] + b.[\z{c}] + c.[\z{c}] + d.[\z{c}]')

check(r'a{\}<x>a', '<x>')
check(r'<x>a{\}<y>a', r'<xy>')
check(r'<x>a{\}<x>a', r'<xx>')
check(r'a{\}(<x>a)*', r'\e.[a{\}\e + <x>(<x>a)*]')
check(r'a*{\}a', r'\e.[a + a*{\}\e]')
check(r'a*{\}a*', r'<\e> + \e.[aa* + a*{\}a* + aa*{\}\e]')
check(r'(<x>a)*{\}(<y>a)*', r'<\e> + \e.[<y>a(<y>a)* + <xy>(<x>a)*{\}(<y>a)* + <x>a(<x>a)*{\}\e]')
check(r'<x>(<y>a)* {\} <z>a*', r'<xz> + \e.[<xz>aa* + <xyz>(<y>a)*{\}a* + <xyz>a(<y>a)*{\}\e]')

# Left quotient vs. conjunction.
check(r'(ab{\}ab)c&c', r'\e.[(b{\}b)c&c]')

# Right quotient with spontaneous transitions.
check(r'\z{/}\e', r'<\z>')
check(r'\e{/}\e', r'<\e>')
check('a{/}a', r'<\e>')
check('a{/}b', r'<\z>')
check(r'abcd{/}\e', 'a.[bcd]')
check('abcd{/}d', r'\e.[(abc){T}{T}]')
check('abcd{/}cd', r'\e.[(c{\}(abc){T}){T}]')
check('abcd{/}bcd', r'\e.[((bc){T}{\}(abc){T}){T}]')
check('abcd{/}abcd', r'\e.[((abc){T}{\}(abc){T}){T}]')

check('(<x>a){/}a', r'\e.[(<x>\e){T}]')
check('<x>a{/}<y>a', r'\e.[(<y>\e{\}<x>\e){T}]')
check('a{/}(<x>a)*', r'\e.[a + (<x>(<x>a)*{T}{\}\e){T}]')
# I don't know for sure this is right :(
check('(<x>a)*{/}(a)*',
r'<\e> + \e.[(a<x>(<x>a)*{T}){T} + (aa*{T}{\}\e){T} + (a*{T}{\}<x>(<x>a)*{T}){T}]')
check('a*{/}a', r'\e.[(a{\}\e){T} + a*{T}{T}]')
check('a*{/}a*', r'<\e> + \e.[(aa*{T}){T} + (a*{T}{\}a*{T}){T} + (aa*{T}{\}\e){T}]')
# I don't know for sure this is right :(
check('(<x>a)*{/}(<y>a)*',
r'<\e> + \e.[(a<x>(<x>a)*{T}){T} + (a<y>(<y>a)*{T}{\}\e){T} + (<y>(<y>a)*{T}{\}<x>(<x>a)*{T}){T}]')

# Right quotient vs. conjunction.
check('(ab{/}ab)c&c', r'\e.[(a{\}a){T}c&c]')



## -------------------- ##
## Classical examples.  ##
## -------------------- ##

# EAT, Example 4.3.
E1='(<1/6>a*+<1/3>b*)*'
# E1 typed.
E1t="(?@[ab] -> q)"+E1
check(E1t, '<2> + a.[<1/3>a*{}] + b.[<2/3>b*{}]'.format(E1, E1))


############################################
## Conjunction and derived-term commute.  ##
############################################

def prod(kind, args):
    '''The product of `kind` (`conjunction`, `infiltrate`, or `shuffle`)
    of `*args`.'''
    if isinstance(args[0], vcsn.automaton):
        if kind == 'conjunction':
            res = vcsn.automaton.conjunction(*args)
        elif kind == 'infiltrate':
            res = vcsn.automaton._infiltrate(list(args))
        elif kind == 'shuffle':
            res = vcsn.automaton._shuffle(list(args))
    else:
        res = args[0]
        for e in args[1:]:
            if kind == 'conjunction':
                res = res.conjunction(e)
            elif kind == 'infiltrate':
                res = res.infiltrate(e)
            elif kind == 'shuffle':
                res = res.shuffle(e)
    return res

# check_prod RE1 RE2...
# ---------------------
# Check derived-term(prod) = prod(derived-term) for the three products.
def check_prod(*exps, **kwargs):
    exps = [ctx.expression(r) for r in exps]
    auts = [r.automaton("expansion") for r in exps]
    for p in ['conjunction', 'infiltrate', 'shuffle']:
        print("Product:", p)
        # Product of automata, trimmed.
        a1 = prod(p, auts).strip().trim().strip()
        # Automaton of product.
        a2 = prod(p, exps).automaton("expansion")
        if 'equiv' in kwargs:
            # FIXME: So far infiltrate does not work when there are
            # one as labels.  Which is also when "equiv" is passed.
            if p == 'infiltrate':
                SKIP(r'infiltrate does not work with \e as label')
            else:
                CHECK_EQUIV(a1, a2)
        else:
            CHECK_ISOMORPHIC(a1, a2)

ctx = vcsn.context('[abc] -> q')

check_prod('(<1/6>a*+<1/3>b*)*', 'a*')
check_prod('(<1/6>a*+<1/3>b*)*', 'b*')
check_prod('(a+b+c)*a(a+b+c)*', '(a+b+c)*b(a+b+c)*', '(a+b+c)*c(a+b+c)*')

ctx = vcsn.context('[abc] -> expressionset<[xyz] -> b>')
check_prod(r'<x>\e', r'<y>\e')


check_prod('<x>a', '<y>a')
check_prod('<x>a', '<y>b')
check_prod('<x>a', '<y>a', '<z>a')
check_prod('<x>a', '<y>b', '<z>c')

check_prod('(a+b+c)*<x>a(a+b+c)*',
           '(a+b+c)*<y>b(a+b+c)*',
           '(a+b+c)*<z>c(a+b+c)*')

# Use ab{\}ab to introduce expansions with the empty word as label.
ctx = vcsn.context('[abcd] -> q')
# FIXME: See issues #86 and #87: we cannot trust these computations.
# They appear to work, but more by luck than correctness.  Reenable
# once it works for real.
#
# check_prod(r'(cd{\}cd)',  r'(cd{\}cd)', equiv=True)
# check_prod(r'a(cd{\}cd)', r'a(cd{\}cd)', equiv=True)
# check_prod(r'(cd{\}cd)a', r'(cd{\}cd)a', equiv=True)
# check_prod(r'a(cd{\}cd)', r'(cd{\}cd)a', equiv=True)
#
# check_prod(r'a(cd{\}cd)', r'b(cd{\}cd)', equiv=True)
# check_prod(r'(cd{\}cd)a', r'(cd{\}cd)b', equiv=True)
# check_prod(r'a(cd{\}cd)', r'(cd{\}cd)b', equiv=True)
#
# check_prod(r'(cd{\}cd)[ab]', r'a(cd{\}cd+b)', equiv=True)
# check_prod(r'(cd{\}cd)[ab]', r'a(cd{\}cd+b)', equiv=True)
# check_prod(r'<1/10>(cd{\}<1/2>cd+a)<2>', r'<1/20>(cd{\}<1/3>cd+a)<3>',
#            equiv=True)

## ----------------- ##
## LaTeX rendering.  ##
## ----------------- ##

x = c.expression(r'aa+<x>abc+<y>bc').expansion()
CHECK_EQ(r'a \odot \left[a \oplus \left\langle x\right\rangle b \, c\right] \oplus b \odot \left[\left\langle y\right\rangle c\right]',
         x.format("latex"))

CHECK_EQ(r'a⊙[a ⊕ ⟨x⟩bc] ⊕ b⊙[⟨y⟩c]',
         x.format("utf8"))


## ------------ ##
## On wordset.  ##
## ------------ ##

c = vcsn.context("[a-z]* -> RatE[[xyz] -> Q]")

# Transposition is the most risky one, as we must not forget to
# transpose the labels in the expansion.
check(r'\z{T}', r'<\z>')
check(r'\e{T}', r'<\e>')
check('a{T}', r'a.[\e]')
check('(abc){T}', r'cba.[\e]')
check('(abc+aabbcc){T}', r'cba.[\e] + ccbbaa.[\e]')
check('(<xy>abc<yz>){T}', 'c.[<zy>(<xy>(ab)){T}]')
check('((foo)(bar)(baz)){T}', 'zab.[((foo)(bar)){T}]')
check('(ab)*{T}', r'<\e> + ba.[(ab)*{T}]')
check('(<xy>((abc)(abc))<yz>)*{T}',
      r'<\e> + cba.[<zy><yx>(cba)(<xy>((abc){2})<yz>)*{T}]')
check('a*b* & (ab)*',
      r'<\e> + a.[a*b*&b(ab)*]')
check('(<x>a)*(<y>b)* & (<z>ab)*',
      r'<\e> + a.[<xz>(<x>a)*(<y>b)*&b(<z>(ab))*]')


## ---------- ##
## tupleset.  ##
## ---------- ##

c = vcsn.context("[abc]* | [xyz]* -> RatE[[XYZ] -> q]")
check('(abc|xyz) & (a|xy)*(bc|z)*',
      'a|xy.[bc|z&(a|xy)*(bc|z)*]')
check('(<X>abc|xyz) & (<Y>a|xy)*(<Z>bc|z)*',
      'a|xy.[<XY>bc|z&(<Y>a|xy)*(<Z>(bc)|z)*]')

c = vcsn.context("[abc] | [xyz] -> Q")
check(r'\e|\e',
      '<1>')
check('a|x',
      r'a|x.[\e]')
check('a*|x*',
      r'<1> + \e|x.[\e|x*] + a|\e.[a*|\e] + a|x.[a*|x*]')

c = vcsn.context("[abc] | [xyz] -> Q")
check(r'\e|\e',
      '<1>')
check('a|x',
      r'a|x.[\e]')

# Check the tupling of expansions: d(e|f) = d(e) | d(f).
c = vcsn.context('[...], q')
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
