#! /usr/bin/env python

import vcsn
from test import *

ab = vcsn.context('lal_char(ab)_b').ratexp('(a+b)*').standard()
bc = vcsn.context('lal_char(bc)_b').ratexp('(b+c)*').standard()
result = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abc)_b"
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
  2 -> F2
  2 -> 1 [label = "a"]
  2 -> 2 [label = "b"]
  3 -> F3
  3 -> 3 [label = "b"]
  3 -> 4 [label = "c"]
  4 -> F4
  4 -> 3 [label = "b"]
  4 -> 4 [label = "c"]
}
''')
CHECK_EQ(ab.sum(bc), result)

a = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abc)_b"
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
  vcsn_context = "lal_char(abc)_b"
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

result = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abc)_b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F2
    F3
  }
  {
    node [shape = circle]
    0
    1
    2
    3
  }
  I0 -> 0
  0 -> 1 [label = "[a-c]"]
  0 -> 2 [label = "[a-c]"]
  1 -> 3 [label = "b"]
  2 -> F2
  2 -> 2 [label = "a, b"]
  3 -> F3
  3 -> 3 [label = "a, b"]
}
''')
CHECK_EQ(a.sum(b).sort(), result)

# Check union of contexts.
a = vcsn.context('lal_char(a)_ratexpset<lal_char(x)_b>').ratexp('<x>a*').standard()
b = vcsn.context('lal_char(b)_ratexpset<lal_char(y)_b>').ratexp('<y>b*').standard()

result = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab)_ratexpset<lal_char(xy)_b>"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F0
    F1
    F2
  }
  {
    node [shape = circle]
    0
    1
    2
  }
  I0 -> 0
  0 -> F0 [label = "<x+y>"]
  0 -> 1 [label = "<x>a"]
  0 -> 2 [label = "<y>b"]
  1 -> F1
  1 -> 1 [label = "a"]
  2 -> F2
  2 -> 2 [label = "b"]
}
''')
CHECK_EQ(a.sum(b), result)


## ----------------- ##
## ratexp + ratexp.  ##
## ----------------- ##

br = vcsn.context('lal_char(a)_ratexpset<lal_char(uv)_b>') \
         .ratexp('<u>a')
z = vcsn.context('lal_char(b)_z').ratexp('<2>b')
q = vcsn.context('lal_char(c)_q').ratexp('<1/3>c')
r = vcsn.context('lal_char(d)_r').ratexp('<.4>d')
CHECK_EQ('<u>a+<<2>\e>b+<<0.333333>\e>c+<<0.4>\e>d', str(br + z + q + r))
