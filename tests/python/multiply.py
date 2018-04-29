#! /usr/bin/env python

import vcsn
from test import *

def check_mult(lhs, rhs):
    '''Check l * r for l in lhs, r in rhs.
    Check the different flavors: general, deterministic,
    and standard.'''

    if isinstance(lhs, list):
        for aut in lhs:
            check_mult(aut, rhs)
    elif isinstance(rhs, list):
        for aut in rhs:
            check_mult(lhs, aut)
    else:
        print("general")
        gen = lhs.multiply(rhs, "general")

        print("deterministic")
        det = lhs.multiply(rhs, "deterministic")
        CHECK(det.is_deterministic(), det, "is deterministic")
        CHECK_EQUIV(gen, det)

        print("standard")
        std = lhs.multiply(rhs, "standard")
        if (lhs.is_standard()
            and (not isinstance(rhs, vcsn.automaton)
                 or rhs.is_standard())):
            CHECK(std.is_standard(), std, " is standard")
        CHECK_EQUIV(gen, std)

ctx = vcsn.context('[...] -> Q')
auts = [ctx.expression('a').standard(),
        ctx.expression('ab').standard(),
        ctx.expression('a+b').standard(),
        ctx.expression('a<2>', identities='none').standard()]
check_mult(auts, [1, 3, (-1, 5), (2, 4), (2, -1)])

# We want the determinization to terminate.
ctx = vcsn.context('[...] -> B')
auts = [auts,
        ctx.expression('a(ba)*').automaton('derived_term'),
        ctx.expression('a+b').derived_term(breaking=True),
        ctx.expression('a*').derived_term()]
check_mult(auts, auts)

ab = vcsn.context('[ab] -> b').expression('(a+b)*')
bc = vcsn.context('[bc] -> b').expression('(b+c)*')
result = vcsn.automaton('''
digraph
{
  vcsn_context = "[abc]? → 𝔹"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F0
    F1
    F2
    F3
    F4
  }
  {
    node [shape = circle]
    0
    1
    2
    3
    4
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  0 -> 3 [label = "b"]
  0 -> 4 [label = "c"]
  1 -> F1
  1 -> 1 [label = "a"]
  1 -> 2 [label = "b"]
  1 -> 3 [label = "b"]
  1 -> 4 [label = "c"]
  2 -> F2
  2 -> 1 [label = "a"]
  2 -> 2 [label = "b"]
  2 -> 3 [label = "b"]
  2 -> 4 [label = "c"]
  3 -> F3
  3 -> 3 [label = "b"]
  3 -> 4 [label = "c"]
  4 -> F4
  4 -> 3 [label = "b"]
  4 -> 4 [label = "c"]
}
''')
CHECK_EQ(result, ab.standard().multiply(bc.standard()))

CHECK_EQ(vcsn.context('[abc] -> b').expression('[ab]*[bc]*'), ab * bc)

a = vcsn.automaton('''
digraph
{
  vcsn_context = "[ab]? → ℤ"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I
    F
  }
  {
    node [shape = circle]
    0
    1
    2
  }
  I -> 0
  0 -> 1 [label = "a, b"]
  1 -> 2 [label = "b"]
  2 -> 2 [label = "a, b"]
  2 -> F [label = "<2>"]
}
''').standard()

CHECK_EQ(vcsn.automaton('''
digraph
{
  vcsn_context = "[ab]? → ℤ"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F4
  }
  {
    node [shape = circle]
    0
    1
    2
    3
    4
  }
  I0 -> 0
  0 -> 1 [label = "a, b"]
  1 -> 2 [label = "b"]
  2 -> 2 [label = "a, b"]
  2 -> 3 [label = "<2>a, <2>b"]
  3 -> 4 [label = "b"]
  4 -> F4 [label = "<2>"]
  4 -> 4 [label = "a, b"]
}
'''), a.multiply(a))



## --------------------- ##
## Heterogeneous input.  ##
## --------------------- ##

# check RES AUT
# -------------
def check(exp, eff):
    CHECK_EQ(exp, str(eff.expression('associative')))

# RatE and B, in both directions.
a1 = vcsn.context('[a] -> expressionset<[uv] -> b>') \
         .expression('<u>a').derived_term()
a2 = vcsn.context('[b] -> b').expression('b*').standard()
check(r'⟨u⟩a(ε+bb*)', a1*a2)
# FIXME: Why don't we get (\e+bb*)<u>a?
check('⟨u⟩a+bb*⟨u⟩a', a2*a1)

# Z, Q, R.
z = vcsn.context('[a] -> z').expression('<2>a')  .derived_term()
q = vcsn.context('[b] -> q').expression('<1/3>b').derived_term()
r = vcsn.context('[c] -> r').expression('<.4>c') .derived_term()

check('⟨2⟩a⟨1/3⟩b', z*q)
check('⟨1/3⟩b⟨2⟩a', q*z)
check('⟨2⟩a⟨1/3⟩b⟨2⟩a', z*q*z)
check('⟨2⟩a(⟨1/3⟩b)²', z*q*q)

check('⟨2⟩a⟨0.4⟩c', z*r)
check('⟨0.4⟩c⟨2⟩a', r*z)

check('⟨0.333333⟩b⟨0.4⟩c', q*r)
check('⟨0.4⟩c⟨0.333333⟩b', r*q)

## ------------------------- ##
## expression * expression.  ##
## ------------------------- ##

br = vcsn.context('[a] -> expressionset<[uv] -> b>') \
         .expression('<u>a')
z = vcsn.context('[b] -> z').expression('<2>b')
q = vcsn.context('[c] -> q').expression('<1/3>c')
r = vcsn.context('[d] -> r').expression('<.4>d')
CHECK_EQ(r'⟨u⟩a⟨⟨2⟩ε⟩b⟨⟨0.333333⟩ε⟩c⟨⟨0.4⟩ε⟩d', str(br * z * q * r))

# This is a real regression (#68): we used to interpret `<3><3>a` as
# `<27>a` because `3 * 3` in Z was intrepreted as `3 ** 3`, because `w
# * n` is interpreted as repeated multiplication, `w ** n` when n is
# an int.  Which collides with binary multiplication in Z.
zexp = vcsn.context('[...] -> Z').expression
CHECK_EQ(zexp('<9>a'), zexp('<3><3>a'))

## --------------- ##
## label * label.  ##
## --------------- ##

l = vcsn.context('[...]* -> B').label
CHECK_EQ(l('abc'),
         l('ab') * l(r'\e') * l('c'))
CHECK_EQ(l('\e'),
         l('ab') ** 0)
CHECK_EQ(l('ababab'),
         l('ab') ** 3)
c = vcsn.context('[...] -> Q')
c2 = c|c
CHECK_EQ(c2.word('abc|aBc'),
         c.word('a') * c2.word('b|B') * c.word('c'))

## ------------------------- ##
## polynomial * polynomial.  ##
## ------------------------- ##

pol = vcsn.context('[...]* -> Z').polynomial
CHECK_EQ(pol('c + <5>d + <2>ac + <10>ad + <3>bc + <15>bd'),
         pol(r'\e + <2>a + <3>b') * pol(r'\e') * pol('c + <5>d'))


## ----------------- ##
## weight * weight.  ##
## ----------------- ##

w = vcsn.context('[...] -> seriesset<[...] -> Q>').weight
CHECK_EQ(w('<4>aa+<6>ab+<6>ba+<9>bb'),
         w('<2>a+<3>b') * w('<2>a+<3>b'))
# See the comment above about #68.
w = vcsn.context('[...] -> Z').weight
CHECK_EQ(w('9'), w('3') * w('3'))
# More * and ** tests in tests/python/weight.py.
