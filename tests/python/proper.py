#! /usr/bin/env python

import vcsn
from test import *

algos = ['distance', 'inplace', 'separate']

# check_algo INPUT EXP ALGORITHM
# ------------------------------
def check_algo(i, o, algo):
    i = vcsn.automaton(i)
    o = vcsn.automaton(o)

    print("using algorithm: ", algo)
    print("checking proper")

    # We call sort().strip() everywhere to avoid seeing differences
    # caused by the different numbering of the states between the
    # algorithms.
    iprop = i.proper(algo=algo)
    CHECK_EQ(o.sort().strip(), iprop.sort().strip())

    print("checking proper cache")

    CHECK_EQ(True, iprop.info('is proper'))

    # Since we remove only states that _become_ inaccessible,
    # i.proper(prune=False).accessible() is not the same as i.proper():
    # in the former case we also removed the non-accessible states.
    print("checking proper(prune=False)")

    iprop = i.proper(prune=False, algo=algo).accessible()
    CHECK_EQ(o.accessible(), iprop)

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
## lal, r: check the computation of star.  ##
## -------------------------------------------- ##

check(metext('lan-r.in.gv'), metext('lan-r.out.gv'))

## ---------------------------------------------- ##
## lal, log: check the computation of star.  ##
## ---------------------------------------------- ##

check(metext('lan-log.in.daut'), metext('lan-log.out.daut'))


## ------------- ##
## law, b.  ##
## ------------- ##

check(metext('law-b.in.gv'), metext('law-b.out.gv'))


## ------------------------------------------------- ##
## lal, z: invalid \e-cycle (weight is not 0).  ##
## ------------------------------------------------- ##

check_fail(metext('lan-z.fail.gv'))


## ------------- ##
## lal, z.  ##
## ------------- ##

check(metext('lan-z.in.gv'), metext('lan-z.out.gv'))


## ---------------------------------- ##
## law, zmin: invalid \e-cycle.  ##
## ---------------------------------- ##

check_fail(r'''digraph
{
  vcsn_context = "[ab]* → ℤmin"
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
## lal, qr: a long cycle.  ##
## ---------------------------- ##

# FIXME(ap): with distance, weights are equivalent but not the same
check(metext('lan-qr.in.gv'), metext('lan-qr.out.gv'),
      [algo for algo in algos if algo != 'distance'])


## ----------------------------------------- ##
## lal, qr: remove now-useless states.  ##
## ----------------------------------------- ##

# Check that we remove states that _end_ without incoming transitions,
# but leave states that were inaccessible before the elimination of
# the spontaneous transitions.

# FIXME(ap): with distance, inaccessible states get pruned
check(r'''digraph
{
  vcsn_context = "[z]? → RatE[[abcdefgh]? → ℚ]"
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
  vcsn_context = "[z]? → RatE[[abcdefgh]? → ℚ]"
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
## lal, b.  ##
## ------------- ##

check(metext('lan-b.in.gv'), metext('lan-b.out.gv'))


## ---------------------------- ##
## lat<lal, lal>, b.  ##
## ---------------------------- ##

check(r'''digraph
{
  vcsn_context = "[ab]? × [xy]? → 𝔹"
  I0 -> 0
  0 -> 1 [label = "(\\e,\\e)"]
  0 -> 1 [label = "(a,x)"]
  0 -> 2 [label = "(b,\\e)"]
  1 -> F1
  1 -> 2 [label = "(\\e,y)"]
  2 -> F2
}''', r'''digraph
{
  vcsn_context = "[ab]? × [xy]? → 𝔹"
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
## lat<lal, lal>, b.  ##
## ---------------------------- ##

check(r'''digraph
{
  vcsn_context = "[ab]? × [xy]? → 𝔹"
  I0 -> 0
  0 -> 1 [label = "(a,x)"]
  0 -> 2 [label = "(b,y)"]
  1 -> F1
  1 -> 2 [label = "(\\e,y)"]
  2 -> F2
}''', r'''digraph
{
  vcsn_context = "[ab]? × [xy]? → 𝔹"
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
## lal, polynomial<law, q>.  ##
## ------------------------- ##

check(metext('lan-poly.1.in.gv'), metext('lan-poly.1.out.gv'))
check_fail(metext('lan-poly.2.fail.gv'))
check_fail(metext('lan-poly.3.fail.gv'))


## ---------------------- ##
## Forward vs. backward.  ##
## ---------------------- ##

a = vcsn.context('lal(ab), b').expression('a*').thompson()
for algo in algos:
    for dir in ['backward', 'forward']:
        CHECK_EQ(meaut('astar-' + dir, 'gv').sort().strip(),
                 a.proper(direction=dir, algo=algo).sort().strip())
