#! /usr/bin/env python

import vcsn
from test import *

def union(*auts):
    res = auts[0]
    for a in auts[1:]:
        res = res.add(a, "general")
    return res

ab = vcsn.context('[ab] -> b').expression('(a+b)*').standard()
bc = vcsn.context('[bc] -> b').expression('(b+c)*').standard()
CHECK_EQ(meaut('abc.gv'), union(ab, bc))

## ------------ ##
## lal_char_z.  ##
## ------------ ##

# <2>(a*b*a*)
a = vcsn.automaton('''
digraph
{
  vcsn_context = "[abc]? → ℤ"
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
  I0 -> 0 [label = "<2>"]
  0 -> F0
  0 -> 0 [label = "a"]
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> F1
  1 -> 1 [label = "a"]
  2 -> F2
  2 -> 1 [label = "a"]
  2 -> 2 [label = "b"]
}
''')

# (<3>(ab))*
b = vcsn.automaton('''
digraph
{
  vcsn_context = "[abc]? → ℤ"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F0
  }
  {
    node [shape = circle]
    0
    1
  }
  I0 -> 0 [label = "<3>"]
  0 -> F0
  0 -> 1 [label = "a"]
  1 -> 0 [label = "<3>b"]
}
''')

exp = vcsn.automaton('''
digraph
{
  vcsn_context = "[abc]? → ℤ"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    I3
    F0
    F1
    F2
    F3
  }
  {
    node [shape = circle]
    0
    1
    2
    3
    4
  }
  I0 -> 0 [label = "<2>"]
  I3 -> 3 [label = "<3>"]
  0 -> F0
  0 -> 0 [label = "a"]
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> F1
  1 -> 1 [label = "a"]
  2 -> F2
  2 -> 1 [label = "a"]
  2 -> 2 [label = "b"]
  3 -> F3
  3 -> 4 [label = "a"]
  4 -> 3 [label = "<3>b"]
}
''')
CHECK_EQ(exp, a + b)

# Check union of contexts.
a1 = vcsn.context('[a] -> expressionset<[x] -> b>').expression('<x>a*').standard()
a2 = vcsn.context('[b] -> expressionset<[y] -> b>').expression('<y>b*').standard()
exp = vcsn.automaton('''
digraph
{
  vcsn_context = "[ab]? → RatE[[xy]? → 𝔹]"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    I2
    F0
    F1
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
  I2 -> 2
  0 -> F0 [label = "<x>"]
  0 -> 1 [label = "<x>a"]
  1 -> F1
  1 -> 1 [label = "a"]
  2 -> F2 [label = "<y>"]
  2 -> 3 [label = "<y>b"]
  3 -> F3
  3 -> 3 [label = "b"]
}
''')

CHECK_EQ(exp, union(a1, a2))

## --------------------- ##
## Heterogeneous input.  ##
## --------------------- ##

# check RES AUT
# -------------
def check(exp, *auts):
    CHECK_EQ(exp, str(union(*auts).expression()))

# RatE and B, in both directions.
a1 = vcsn.context('[a] -> expressionset<[uv] -> b>') \
         .expression('<u>a').derived_term().strip()
a2 = vcsn.context('[b] -> b').expression('b*').derived_term().strip()
check('⟨u⟩a+b*', a1, a2)
check('⟨u⟩a+b*', a2, a1)

# Z, Q, R.
z = vcsn.context('[a] -> z').expression('<2>a')  .automaton()
q = vcsn.context('[b] -> q').expression('<1/3>b').automaton()
r = vcsn.context('[c] -> r').expression('<.4>c') .automaton()

check('⟨2⟩a+⟨1/3⟩b', z, q)
check('⟨2⟩a+⟨1/3⟩b', q, z)
check('⟨4⟩a+⟨1/3⟩b', z, q, z)
check('⟨2⟩a+⟨2/3⟩b', z, q, q)

check('⟨2⟩a+⟨0.4⟩c', z, r)
check('⟨2⟩a+⟨0.4⟩c', r, z)

check('⟨0.333333⟩b+⟨0.4⟩c', q, r)
check('⟨0.333333⟩b+⟨0.4⟩c', r, q)
