#! /usr/bin/env python

import vcsn
from test import *

ctxbr = vcsn.context('lal_char(a), ratexpset<lal_char(uv), b>')
ctxz = vcsn.context('lal_char(b), z')
ctxq = vcsn.context('lal_char(c), q')
ctxr = vcsn.context('lal_char(d), r')

ab = vcsn.context('lal_char(ab), b').ratexp('(a+b)*').standard()
bc = vcsn.context('lal_char(bc), b').ratexp('(b+c)*').standard()
result = '''digraph
{
  vcsn_context = "letterset<char_letters(abc)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
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
    node [shape = circle, style = rounded, width = 0.5]
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
  2 -> F2
  2 -> 1 [label = "a"]
  2 -> 2 [label = "b"]
  3 -> F3
  3 -> 3 [label = "b"]
  3 -> 4 [label = "c"]
  4 -> F4
  4 -> 3 [label = "b"]
  4 -> 4 [label = "c"]
}'''
CHECK_EQ(result, ab.sum(bc))

a = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abc), b"
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
  0 -> 1 [label = "c"]
  1 -> 2 [label = "b"]
  2 -> 2 [label = "a, b"]
  2 -> F
}
''')

b = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abc), b"
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
  }
  I -> 0
  0 -> 1 [label = "a, b"]
  0 -> 1 [label = "c"]
  1 -> 1 [label = "a, b"]
  1 -> F
}
''')

result = '''digraph
{
  vcsn_context = "letterset<char_letters(abc)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F2
    F3
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
    3
  }
  I0 -> 0
  0 -> 1 [label = "[a-c]"]
  0 -> 3 [label = "[a-c]"]
  1 -> 2 [label = "b"]
  2 -> F2
  2 -> 2 [label = "a, b"]
  3 -> F3
  3 -> 3 [label = "a, b"]
}'''
CHECK_EQ(result, a.sum(b))

# Check join of contexts.
a = vcsn.context('lal_char(a), ratexpset<lal_char(x), b>').ratexp('<x>a*').standard()
b = vcsn.context('lal_char(b), q').ratexp('<1/2>b*').standard()

result = r'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, ratexpset<letterset<char_letters(x)>, q>"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F1
    F2
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
  }
  I0 -> 0
  0 -> F0 [label = "<x+<1/2>\\e>"]
  0 -> 1 [label = "<x>a"]
  0 -> 2 [label = "<<1/2>\\e>b"]
  1 -> F1
  1 -> 1 [label = "a"]
  2 -> F2
  2 -> 2 [label = "b"]
}'''
CHECK_EQ(result, a.sum(b))


## ------------------------- ##
## polynomial + polynomial.  ##
## ------------------------- ##

br = ctxbr.polynomial('<u>a')
z = ctxz.polynomial('<2>b')
q = ctxq.polynomial('<1/3>c')
r = ctxr.polynomial('<.4>d')
CHECK_EQ('<u>a + <<2>\e>b + <<0.333333>\e>c + <<0.4>\e>d', str(br + z + q + r))


## ----------------- ##
## ratexp + ratexp.  ##
## ----------------- ##

br = ctxbr.ratexp('<u>a')
z = ctxz.ratexp('<2>b')
q = ctxq.ratexp('<1/3>c')
r = ctxr.ratexp('<.4>d')
CHECK_EQ('<u>a+<<2>\e>b+<<0.333333>\e>c+<<0.4>\e>d', str(br + z + q + r))


## ----------------- ##
## weight + weight.  ##
## ----------------- ##

br = ctxbr.weight('u')
z = ctxz.weight('2')
q = ctxq.weight('1/3')
r = ctxr.weight('.4')
CHECK_EQ('2.73333', str(z + q + r))
CHECK_EQ('u+<2>\e+<0.333333>\e+<0.4>\e', str(br + z + q + r))
