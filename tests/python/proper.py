#! /usr/bin/env python
import vcsn
from test import *

# check complete algorithm
# ------------------------
def check(i, o):
  i = vcsn.automaton(i)
  o = vcsn.automaton(o)
  CHECK_EQ(o.sort(), i.proper_real().sort())
  # Idempotence.
  CHECK_EQ(o.sort(), o.proper_real().sort())

def check_fail(aut):
    a = vcsn.automaton(aut)
    try:
        a.proper()
        FAIL("invalid \\\\e-cycle not detected")
    except RuntimeError:
        PASS()

## ------------------------------------------- ##
## law_char_q: check the computation of star.  ##
## ------------------------------------------- ##

check('''
digraph
{
  vcsn_context = "law_char(ab)_q"
  I -> 0 -> F
  0 -> 0 [label = "<1/3>\\\\e"]
}''','''
digraph
{
  vcsn_context = "law_char(ab)_q"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F0
  }
  {
    node [shape = circle]
    0
  }
  I0 -> 0
  0 -> F0 [label = "<3/2>"]
}''')

## ------------------------------------------- ##
## law_char_r: check the computation of star.  ##
## ------------------------------------------- ##

check('''
digraph
{
  vcsn_context = "law_char(ab)_r"
  I -> 0 -> F
  0 -> 0 [label = "<.5>\\\\e"]
}''','''
digraph
{
  vcsn_context = "law_char(ab)_r"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F0
  }
  {
    node [shape = circle]
    0
  }
  I0 -> 0
  0 -> F0 [label = "<2>"]
}''')



## ------------ ##
## law_char_b.  ##
## ------------ ##

check('''
digraph
{
  vcsn_context = "law_char(ab)_b"
  I0 -> -1
  -1 -> 0 [label="a"]
  0 -> 1 [label="a"]
  1 -> -1 [label="a"]
  -1 -> 1 [label="\\\\e"]
  0 -> -1 [label="\\\\e"]
  1 -> 0 [label="\\\\e"]
  0 -> F1
}''', '''
digraph
{
  vcsn_context = "law_char(ab)_b"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
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

check_fail('''
digraph
{
  vcsn_context = "law_char(ab)_z"
  I0 -> -1
  -1 -> 0 [label="<2>a"]
  1 -> 2 [label="<2>a"]
  2 -> -1 [label="<2>a"]
  -1 -> 1 [label="<-1>"]
  0 -> -1 [label="<-1>"]
  1 -> 0 [label="<-1>"]
  0 -> F1
}''')


## ------------ ##
## law_char_z.  ##
## ------------ ##

check('''
digraph
{
  vcsn_context = "law_char(ab)_z"
  I0 -> -1
  -1 -> 0 [label="<2>a"]
  1 -> 2 [label="<2>a"]
  2 -> -1 [label="<2>a"]
  -1 -> 1 [label="<-1>a"]
  0 -> -1 [label="<-1>"]
  1 -> 0 [label="<-1>"]
  0 -> F1
}''', '''
digraph
{
  vcsn_context = "law_char(ab)_z"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F1
    F2
  }
  {
    node [shape = circle]
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



## ------------------------------ ##
## law_char_z: invalid \e-cycle.  ##
## ------------------------------ ##

check_fail('''
digraph
{
  vcsn_context = "law_char(ab)_zmin"
  I0 -> -1
  -1 -> 0 [label="<2>a"]
  0 -> 1 [label="<2>a"]
  1 -> -1 [label="<2>a"]
  -1 -> 1 [label="<-1>"]
  0 -> -1 [label="<-1>"]
  1 -> 0 [label="<-1>"]
  0 -> F1
}''')


## --------------------------- ##
## lan_char_zr: a long cycle.  ##
## --------------------------- ##

check('''
digraph
{
  vcsn_context = "lan_char(z)_ratexpset<lal_char(abcd)_z>"
  rankdir = LR
  node [shape = circle]
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I
    F
  }
  { 0 1 2 3 4 }
  I -> 0
  0 -> 1 [label = "<a>\\\\e"]
  1 -> 2 [label = "<b>\\\\e"]
  2 -> 3 [label = "<c>\\\\e"]
  3 -> 0 [label = "<d>\\\\e"]
  0 -> 4 [label = "z"]
  4 -> F
}''', '''
digraph
{
  vcsn_context = "lan_char(z)_ratexpset<lal_char(abcd)_z>"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F1
  }
  {
    node [shape = circle]
    0
    1
  }
  I0 -> 0
  0 -> 1 [label = "<(abcd)*>z"]
  1 -> F1
}''')



## ---------------------------------------- ##
## lan_char_zr: remove now-useless states.  ##
## ---------------------------------------- ##

# Check that we remove states that _end_ without incoming transitions,
# but leave states that were inaccessible before the elimination of
# the spontaneous transitions.

check('''
digraph
{
  vcsn_context = "lan_char(z)_ratexpset<lal_char(abcdefgh)_z>"
  rankdir = LR
  node [shape = circle]
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I
    F
  }
  { 0 1 2 3 4 5 6 7 8 9 }
  I -> 0

  0 -> 3 [label = "<a>\\\\e"]
  0 -> 5 [label = "<b>\\\\e"]

  1 -> 2 [label = "<c>\\\\e"]
  3 -> 4 [label = "<d>\\\\e"]
  5 -> 6 [label = "<e>\\\\e"]
  7 -> 8 [label = "<f>\\\\e"]

  6 -> 9 [label = "<g>\\\\e"]
  8 -> 9 [label = "<h>\\\\e"]

  9 -> F
}''', '''
digraph
{
  vcsn_context = "lan_char(z)_ratexpset<lal_char(abcdefgh)_z>"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F0
    F2
  }
  {
    node [shape = circle]
    0
    1 [color = DimGray]
    2 [color = DimGray]
  }
  I0 -> 0
  0 -> F0 [label = "<beg>"]
  2 -> F2 [label = "<fh>", color = DimGray]
}''')

PLAN()
