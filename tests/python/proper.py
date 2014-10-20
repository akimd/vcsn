#! /usr/bin/env python

import vcsn
from test import *

# check INPUT EXP
# ---------------
def check(i, o):
  i = vcsn.automaton(i)
  CHECK_EQ(o, i._proper())
  # FIXME: Because _proper uses copy, state numbers are changed.
  #
  # FIXME: cannot use is_isomorphic because there may be unreachable
  # states.
  CHECK_EQ(i._proper().sort().strip(), i._proper()._proper().sort().strip())

def check_fail(aut):
  a = vcsn.automaton(aut)
  try:
    a.proper()
    FAIL(r"invalid \\e-cycle not detected")
  except RuntimeError:
    PASS()

# check the artificial conversion lan -> lal with proper
# ------------------------------------------------------
def check_to_lal(i, o):
  i = vcsn.automaton(i)
  CHECK_EQ(o, i.proper())


## ------------------------------------------- ##
## law_char_r: check the computation of star.  ##
## ------------------------------------------- ##

check(r'''digraph
{
  vcsn_context = "law_char(ab)_r"
  I -> 0 -> F
  0 -> 0 [label = "<.5>\\e"]
}''','''digraph
{
  vcsn_context = "law_char(ab)_r"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
  }
  I0 -> 0
  0 -> F0 [label = "<2>"]
}''')



## ------------ ##
## law_char_b.  ##
## ------------ ##

check(r'''digraph
{
  vcsn_context = "law_char(ab)_b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F1
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "\\e"]
  1 -> F1
  1 -> 0 [label = "\\e"]
  1 -> 2 [label = "a"]
  2 -> 0 [label = "a"]
  2 -> 1 [label = "\\e"]
}''', '''digraph
{
  vcsn_context = "law_char(ab)_b"
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
  0 -> F0
  0 -> 0 [label = "a"]
  0 -> 1 [label = "a"]
  0 -> 2 [label = "a"]
  1 -> F1
  1 -> 0 [label = "a"]
  1 -> 1 [label = "a"]
  1 -> 2 [label = "a"]
  2 -> F2
  2 -> 0 [label = "a"]
  2 -> 1 [label = "a"]
  2 -> 2 [label = "a"]
}''')


## ------------------------------------------------ ##
## law_char_z: invalid \e-cycle (weight is not 0).  ##
## ------------------------------------------------ ##

check_fail(r'''digraph
{
  vcsn_context = "law_char(ab)_z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F1
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
    3
  }
  I0 -> 0
  0 -> 1 [label = "<2>a"]
  0 -> 2 [label = "<-1>\\e"]
  1 -> F1
  1 -> 0 [label = "<-1>\\e"]
  2 -> 1 [label = "<-1>\\e"]
  2 -> 3 [label = "<2>a"]
  3 -> 0 [label = "<2>a"]
}''')


## ------------ ##
## law_char_z.  ##
## ------------ ##

check(r'''digraph
{
  vcsn_context = "law_char(ab)_z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F1
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
    3
  }
  I0 -> 0
  0 -> 1 [label = "<2>a"]
  0 -> 2 [label = "<-1>a"]
  1 -> F1
  1 -> 0 [label = "<-1>\\e"]
  2 -> 1 [label = "<-1>\\e"]
  2 -> 3 [label = "<2>a"]
  3 -> 0 [label = "<2>a"]
}''', '''digraph
{
  vcsn_context = "law_char(ab)_z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F1
    F2
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
    3
  }
  I0 -> 0
  0 -> 1 [label = "<2>a"]
  0 -> 2 [label = "<-1>a"]
  1 -> F1
  1 -> 1 [label = "<-2>a"]
  1 -> 2 [label = "a"]
  2 -> F2 [label = "<-1>"]
  2 -> 1 [label = "<2>a"]
  2 -> 2 [label = "<-1>a"]
  2 -> 3 [label = "<2>a"]
  3 -> 0 [label = "<2>a"]
}''')



## --------------------------------- ##
## law_char_zmin: invalid \e-cycle.  ##
## --------------------------------- ##

check_fail(r'''digraph
{
  vcsn_context = "law_char(ab)_zmin"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F1
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
  }
  I0 -> 0 [label = "<0>"]
  0 -> 1 [label = "<2>a"]
  0 -> 2 [label = "<-1>\\e"]
  1 -> F1 [label = "<0>"]
  1 -> 0 [label = "<-1>\\e"]
  1 -> 2 [label = "<2>a"]
  2 -> 0 [label = "<2>a"]
  2 -> 1 [label = "<-1>\\e"]
}''')


## --------------------------- ##
## lan_char_zr: a long cycle.  ##
## --------------------------- ##

check(r'''digraph
{
  vcsn_context = "lan_char(z)_ratexpset<lal_char(abcd)_z>"
  rankdir = LR
  node [shape = circle]
  {
    node [shape = point, width = 0]
    I
    F
  }
  { 0 1 2 3 4 }
  I -> 0
  0 -> 1 [label = "<a>\\e"]
  1 -> 2 [label = "<b>\\e"]
  2 -> 3 [label = "<c>\\e"]
  3 -> 0 [label = "<d>\\e"]
  0 -> 4 [label = "z"]
  4 -> F
}''', '''digraph
{
  vcsn_context = "lan<lal_char(z)>_ratexpset<lal_char(abcd)_z>"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F4
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    4
  }
  I0 -> 0
  0 -> 4 [label = "<(abcd)*>z"]
  4 -> F4
}''')



## ---------------------------------------- ##
## lan_char_zr: remove now-useless states.  ##
## ---------------------------------------- ##

# Check that we remove states that _end_ without incoming transitions,
# but leave states that were inaccessible before the elimination of
# the spontaneous transitions.

check(r'''digraph
{
  vcsn_context = "lan_char(z)_ratexpset<lal_char(abcdefgh)_z>"
  rankdir = LR
  node [shape = circle]
  {
    node [shape = point, width = 0]
    I
    F
  }
  { 0 1 2 3 4 5 6 7 8 9 }
  I -> 0

  0 -> 3 [label = "<a>\\e"]
  0 -> 5 [label = "<b>\\e"]

  1 -> 2 [label = "<c>\\e"]
  3 -> 4 [label = "<d>\\e"]
  5 -> 6 [label = "<e>\\e"]
  7 -> 8 [label = "<f>\\e"]

  6 -> 9 [label = "<g>\\e"]
  8 -> 9 [label = "<h>\\e"]

  9 -> F
}''', '''digraph
{
  vcsn_context = "lan<lal_char(z)>_ratexpset<lal_char(abcdefgh)_z>"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F7
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1 [color = DimGray]
    7 [color = DimGray]
  }
  I0 -> 0
  0 -> F0 [label = "<beg>"]
  7 -> F7 [label = "<fh>", color = DimGray]
}''')


## -------------------------------------- ##
## lan_char_zr: Check conversion to lal.  ##
## -------------------------------------- ##

check_to_lal(r'''digraph
{
  vcsn_context = "lan_char(ab)_b"
  I -> 0
  0 -> 1 [label = "\\e"]
  1 -> 0 [label = "\\e"]
  0 -> 4 [label = "a"]
  4 -> F
}''', '''digraph
{
  vcsn_context = "lal_char(ab)_b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F1
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  1 -> F1
}''')

## --------------------------- ##
## lat<lan_char, lan_char>_b.  ##
## --------------------------- ##

check(r'''digraph
{
  vcsn_context = "lat<lan_char(ab),lan_char(xy)>_b"
  I0 -> 0
  0 -> 1 [label = "(\\e,\\e)"]
  0 -> 1 [label = "(a,x)"]
  0 -> 2 [label = "(b,\\e)"]
  1 -> F1
  1 -> 2 [label = "(\\e,y)"]
  2 -> F2
}''', r'''digraph
{
  vcsn_context = "lat<lan<lal_char(ab)>,lan<lal_char(xy)>>_b"
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
  0 -> F0
  0 -> 1 [label = "(a,x)"]
  0 -> 2 [label = "(\\e,y), (b,\\e)"]
  1 -> F1
  1 -> 2 [label = "(\\e,y)"]
  2 -> F2
}''')


## --------------------------- ##
## lat<lan_char, lal_char>_b.  ##
## --------------------------- ##

check(r'''digraph
{
  vcsn_context = "lat<lan_char(ab),lal_char(xy)>_b"
  I0 -> 0
  0 -> 1 [label = "(a,x)"]
  0 -> 2 [label = "(b,y)"]
  1 -> F1
  1 -> 2 [label = "(\\e,y)"]
  2 -> F2
}''', r'''digraph
{
  vcsn_context = "lat<lan<lal_char(ab)>,lal_char(xy)>_b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
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
  0 -> 1 [label = "(a,x)"]
  0 -> 2 [label = "(b,y)"]
  1 -> F1
  1 -> 2 [label = "(\\e,y)"]
  2 -> F2
}''')



## ---------------------------------------------------------------- ##
## Check that lat<lan_char, lal_char>_b is not ruined by the lan to ##
## lal transition.                                                  ##
## ---------------------------------------------------------------- ##

check_to_lal(r'''digraph
{
  vcsn_context = "lat<lan_char(ab),lal_char(xy)>_b"
  I0 -> 0
  0 -> 1 [label = "(a,x)"]
  0 -> 2 [label = "(b,y)"]
  1 -> F1
  1 -> 2 [label = "(\\e,y)"]
  2 -> F2
}''', r'''digraph
{
  vcsn_context = "lat<lan<lal_char(ab)>,lal_char(xy)>_b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
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
  0 -> 1 [label = "(a,x)"]
  0 -> 2 [label = "(b,y)"]
  1 -> F1
  1 -> 2 [label = "(\\e,y)"]
  2 -> F2
}''')


## ---------------------- ##
## Forward vs. backward.  ##
## ---------------------- ##

a = vcsn.context('lan_char(ab)_b').ratexp('a*').thompson()
CHECK_EQ(r'''digraph
{
  vcsn_context = "lal_char(ab)_b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I1
    F0
    F1
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
  }
  I1 -> 1
  0 -> F0
  0 -> 0 [label = "a"]
  1 -> F1
  1 -> 0 [label = "a"]
}''',
         a.proper(backward = True))

CHECK_EQ(r'''digraph
{
  vcsn_context = "lal_char(ab)_b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    I1
    F1
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
  }
  I0 -> 0
  I1 -> 1
  0 -> 0 [label = "a"]
  0 -> 1 [label = "a"]
  1 -> F1
}''',
         a.proper(backward = False))
