#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context('lal_char, q')

def check_mult(lhs, rhs, algo="standard"):
    if isinstance(lhs, list):
        for aut in lhs:
            check_mult(aut, rhs)
    elif isinstance(rhs, list):
        for aut in rhs:
            check_mult(lhs, aut)
    else:
        gen = lhs.multiply(rhs, "general")
        aut = lhs.multiply(rhs, algo)
        CHECK_EQUIV(gen, aut)

def check_mult_deter(lhs, rhs):
    if isinstance(lhs, list):
        for aut in lhs:
            check_mult(aut, rhs)
    elif isinstance(rhs, list):
        for aut in rhs:
            check_mult(lhs, aut)
    else:
        gen = lhs.multiply(rhs, "general")
        det = lhs.multiply(rhs, "deterministic")
        CHECK_EQUIV(gen, det)

auts = [ctx.expression('a').standard(),
        ctx.expression('ab').standard(),
        ctx.expression('a+b').standard(),
        ctx.expression('a<2>', identities='none').standard()]
check_mult(auts, [1, 3, (-1, 5), (2, 4), (2, -1)])

auts = [auts,
        ctx.expression('a(ba)*').automaton('derived_term'),
        ctx.expression('a+b').derived_term(breaking=True),
        ctx.expression('a*').derived_term()]
check_mult(auts, auts, "deterministic")

auts = [auts,
        ctx.expression('a*').derived_term()]
check_mult_deter(auts, auts)

ab = vcsn.context('lal_char(ab), b').expression('(a+b)*')
bc = vcsn.context('lal_char(bc), b').expression('(b+c)*')
result = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abc), b"
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

CHECK_EQ(vcsn.context('lal_char(abc), b').expression('[ab]*[bc]*'), ab * bc)

a = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab), z"
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
  vcsn_context = "lal_char(ab), z"
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
a1 = vcsn.context('lal_char(a), expressionset<lal_char(uv), b>') \
         .expression('<u>a').derived_term()
a2 = vcsn.context('lal_char(b), b').expression('b*').standard()
check('<u>a(\e+bb*)', a1*a2)
# FIXME: Why don't we get (\e+bb*)<u>a?
check('<u>a+bb*<u>a', a2*a1)

# Z, Q, R.
z = vcsn.context('lal_char(a), z').expression('<2>a')  .derived_term()
q = vcsn.context('lal_char(b), q').expression('<1/3>b').derived_term()
r = vcsn.context('lal_char(c), r').expression('<.4>c') .derived_term()

check('<2>a<1/3>b', z*q)
check('<1/3>b<2>a', q*z)
check('<2>a<1/3>b<2>a', z*q*z)
check('<2>a(<1/3>b){2}', z*q*q)

check('<2>a<0.4>c', z*r)
check('<0.4>c<2>a', r*z)

check('<0.333333>b<0.4>c', q*r)
check('<0.4>c<0.333333>b', r*q)

## ------------------------- ##
## expression * expression.  ##
## ------------------------- ##

br = vcsn.context('lal_char(a), expressionset<lal_char(uv), b>') \
         .expression('<u>a')
z = vcsn.context('lal_char(b), z').expression('<2>b')
q = vcsn.context('lal_char(c), q').expression('<1/3>c')
r = vcsn.context('lal_char(d), r').expression('<.4>d')
CHECK_EQ('<u>a<<2>\e>b<<0.333333>\e>c<<0.4>\e>d', str(br * z * q * r))

## --------------- ##
## label * label.  ##
## --------------- ##

c = vcsn.context('law_char, b')
CHECK_EQ(c.label('abc'),
         c.label('ab') * c.label('\e') * c.label('c'))


## ------------------------- ##
## polynomial * polynomial.  ##
## ------------------------- ##

c = vcsn.context('law_char, z')
CHECK_EQ(c.polynomial('c + <5>d + <2>ac + <10>ad + <3>bc + <15>bd'),
         c.polynomial('\e + <2>a + <3>b') * c.polynomial('\e') * c.polynomial('c + <5>d'))


## ----------------- ##
## weight * weight.  ##
## ----------------- ##

c = vcsn.context('lal_char, seriesset<lal_char, q>')
CHECK_EQ(c.weight('<4>aa+<6>ab+<6>ba+<9>bb'),
         c.weight('<2>a+<3>b') * c.weight('<2>a+<3>b'))
