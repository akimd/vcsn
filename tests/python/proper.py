#! /usr/bin/env python

import vcsn
from test import *

algos = ['distance', 'inplace', 'separate']

# check INPUT EXP ALGORITHM
# -------------------------
def check_algo(i, o, algo):
  i = vcsn.automaton(i)
  o = vcsn.automaton(o)

  print("using algorithm: ", algo)
  print("checking proper")

  # We call sort().strip() everywhere to avoid seeing differences
  # caused by the different numbering of the states between the
  # algorithms.
  CHECK_EQ(o.sort().strip(), i.proper(algo=algo).sort().strip())

  # Since we remove only states that _become_ inaccessible,
  # i.proper(prune=False).accessible() is not the same as i.proper():
  # in the former case we also removed the non-accessible states.
  print("checking proper(prune=False)")
  CHECK_EQ(o.accessible(),
           i.proper(prune=False, algo=algo).accessible())

  # FIXME: Because proper uses copy, state numbers are changed.
  #
  # FIXME: cannot use is_isomorphic because some of our test cases
  # have unreachable states, which is considered invalid by
  # is_isomorphic.
  print("checking idempotence")
  p = i.proper(algo=algo)
  if p.is_accessible():
    CHECK_ISOMORPHIC(p, p.proper(algo=algo))
  else:
    CHECK_EQ(p.sort().strip(),
             p.proper(algo=algo).sort().strip())

def check_fail_algo(aut, algo):
  a = vcsn.automaton(aut)
  try:
    a.proper(algo=algo)
    FAIL(r"invalid \\e-cycle not detected")
  except RuntimeError:
    PASS()

def check(i, o, algs=algos):
  for algo in algs:
    check_algo(i, o, algo)

def check_fail(i, algs=algos):
  for algo in algs:
    check_fail_algo(i, algo)


## --------------------------------------- ##
## lao, r: check the computation of star.  ##
## --------------------------------------- ##

check(metext('lao-r.in.gv'), metext('lao-r.out.gv'))

## -------------------------------------------- ##
## lan_char, r: check the computation of star.  ##
## -------------------------------------------- ##

check(metext('lan-r.in.gv'), metext('lan-r.out.gv'))

## ------------- ##
## law_char, b.  ##
## ------------- ##

check(metext('law-b.in.gv'), metext('law-b.out.gv'))


## ------------------------------------------------- ##
## lan_char, z: invalid \e-cycle (weight is not 0).  ##
## ------------------------------------------------- ##

check_fail(metext('lan-z.fail.gv'))


## ------------- ##
## lan_char, z.  ##
## ------------- ##

check(metext('lan-z.in.gv'), metext('lan-z.out.gv'))



## ---------------------------------- ##
## law_char, zmin: invalid \e-cycle.  ##
## ---------------------------------- ##

check_fail(r'''digraph
{
  vcsn_context = "law_char(ab), zmin"
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


## ---------------------------- ##
## lan_char, qr: a long cycle.  ##
## ---------------------------- ##

# FIXME(ap): with distance, weights are equivalent but not the same
check(metext('lan-qr.in.gv'), metext('lan-qr.out.gv'),
      [algo for algo in algos if algo != 'distance'])



## ----------------------------------------- ##
## lan_char, qr: remove now-useless states.  ##
## ----------------------------------------- ##

# Check that we remove states that _end_ without incoming transitions,
# but leave states that were inaccessible before the elimination of
# the spontaneous transitions.

# FIXME(ap): with distance, inaccessible states get pruned
check(r'''digraph
{
  vcsn_context = "lan_char(z), expressionset<lal_char(abcdefgh), q>"
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
  vcsn_context = "letterset<char_letters(z)>, expressionset<letterset<char_letters(abcdefgh)>, q>"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F2
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1 [color = DimGray]
    2 [color = DimGray]
  }
  I0 -> 0
  0 -> F0 [label = "<beg>"]
  2 -> F2 [label = "<fh>", color = DimGray]
}''', [algo for algo in algos if algo != 'distance'])


## ------------- ##
## lan_char, b.  ##
## ------------- ##

check(metext('lan-b.in.gv'), metext('lan-b.out.gv'))


## ---------------------------- ##
## lat<lan_char, lan_char>, b.  ##
## ---------------------------- ##

check(r'''digraph
{
  vcsn_context = "lat<lan_char(ab),lan_char(xy)>, b"
  I0 -> 0
  0 -> 1 [label = "(\\e,\\e)"]
  0 -> 1 [label = "(a,x)"]
  0 -> 2 [label = "(b,\\e)"]
  1 -> F1
  1 -> 2 [label = "(\\e,y)"]
  2 -> F2
}''', r'''digraph
{
  vcsn_context = "lat<nullableset<letterset<char_letters(ab)>>, nullableset<letterset<char_letters(xy)>>>, b"
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


## ---------------------------- ##
## lat<lan_char, lal_char>, b.  ##
## ---------------------------- ##

check(r'''digraph
{
  vcsn_context = "lat<lan_char(ab),lal_char(xy)>, b"
  I0 -> 0
  0 -> 1 [label = "(a,x)"]
  0 -> 2 [label = "(b,y)"]
  1 -> F1
  1 -> 2 [label = "(\\e,y)"]
  2 -> F2
}''', r'''digraph
{
  vcsn_context = "lat<nullableset<letterset<char_letters(ab)>>, letterset<char_letters(xy)>>, b"
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


## ------------------------- ##
## lan, polynomial<law, q>.  ##
## ------------------------- ##

check(metext('lan-poly.1.in.gv'), metext('lan-poly.1.out.gv'))
check_fail(metext('lan-poly.2.fail.gv'))
check_fail(metext('lan-poly.3.fail.gv'))


## ---------------------- ##
## Forward vs. backward.  ##
## ---------------------- ##

for algo in algos:
  a = vcsn.context('lan_char(ab), b').expression('a*').thompson()
  CHECK_EQ(vcsn.automaton(r'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, b"
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
}''').sort().strip(), a.proper(direction="backward", algo=algo).sort().strip())

  CHECK_EQ(vcsn.automaton(r'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, b"
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
}''').sort().strip(), a.proper(direction="forward", algo=algo).sort().strip())
