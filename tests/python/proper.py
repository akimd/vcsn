#! /usr/bin/env python

from __future__ import print_function

import vcsn
from test import *

# check INPUT EXP ALGORITHM
# -------------------------
def check(i, o, algo):
  i = vcsn.automaton(i)
  o = vcsn.automaton(o)

  print("using algorithm: ", algo)
  print("checking proper")

  # We call sort().strip() everywhere to avoid seeing differences caused by the
  # different numbering of the states between the algorithms
  CHECK_EQ(o.sort().strip(), i.proper(algo=algo).sort().strip())

  # Since we remove only states that _become_ inaccessible,
  # i.proper(prune = False).accessible() is not the same as
  # i.proper(): in the former case we also removed the non-accessible
  # states.
  print("checking proper(prune = False)")
  CHECK_EQ(o.accessible(),
           i.proper(prune=False, algo=algo).accessible())

  # FIXME: Because proper uses copy, state numbers are changed.
  #
  # FIXME: cannot use is_isomorphic because some of our test cases
  # have unreachable states, which is considered invalid by
  # is_isomorphic.
  print("checking idempotence")
  if i.proper(algo=algo).is_accessible():
    CHECK_ISOMORPHIC(i.proper(algo=algo), i.proper(algo=algo).proper(algo=algo))
  else:
    CHECK_EQ(i.proper(algo=algo).sort().strip(),
             i.proper(algo=algo).proper(algo=algo).sort().strip())

def check_fail(aut, algo):
  a = vcsn.automaton(aut)
  try:
    a.proper(algo=algo)
    FAIL(r"invalid \\e-cycle not detected")
  except RuntimeError:
    PASS()


for algo in ('inplace', 'separate'):

    ## -------------------------------------------- ##
    ## law_char, r: check the computation of star.  ##
    ## -------------------------------------------- ##

    check(r'''digraph
{
  vcsn_context = "law_char(ab), r"
  I -> 0 -> F
  0 -> 0 [label = "<.5>\\e"]
}''','''digraph
{
  vcsn_context = "wordset<char_letters(ab)>, r"
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
}''', algo)



    ## ------------ ##
    ## law_char_b.  ##
    ## ------------ ##

    check(r'''digraph
{
  vcsn_context = "law_char(ab), b"
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
  vcsn_context = "wordset<char_letters(ab)>, b"
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
}''', algo)


    ## ------------------------------------------------ ##
    ## law_char_z: invalid \e-cycle (weight is not 0).  ##
    ## ------------------------------------------------ ##

    check_fail(r'''digraph
{
  vcsn_context = "law_char(ab), z"
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
}''', algo)


    ## ------------ ##
    ## law_char_z.  ##
    ## ------------ ##

    check(r'''digraph
{
  vcsn_context = "law_char(ab), z"
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
  vcsn_context = "wordset<char_letters(ab)>, z"
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
}''', algo)



    ## --------------------------------- ##
    ## law_char_zmin: invalid \e-cycle.  ##
    ## --------------------------------- ##

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
}''', algo)


    ## --------------------------- ##
    ## lan_char_zr: a long cycle.  ##
    ## --------------------------- ##

    check(r'''digraph
{
  vcsn_context = "lan_char(z), expressionset<lal_char(abcd), z>"
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
}''', r'''digraph
{
  vcsn_context = "letterset<char_letters(z)>, expressionset<letterset<char_letters(abcd)>, z>"
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
  0 -> 1 [label = "<(abcd)*>z"]
  1 -> F1
}''', algo)



    ## ---------------------------------------- ##
    ## lan_char_zr: remove now-useless states.  ##
    ## ---------------------------------------- ##

    # Check that we remove states that _end_ without incoming transitions,
    # but leave states that were inaccessible before the elimination of
    # the spontaneous transitions.

    check(r'''digraph
{
  vcsn_context = "lan_char(z), expressionset<lal_char(abcdefgh), z>"
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
  vcsn_context = "letterset<char_letters(z)>, expressionset<letterset<char_letters(abcdefgh)>, z>"
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
}''', algo)


    ## ------------ ##
    ## lan_char_b.  ##
    ## ------------ ##

    check(r'''digraph
{
  vcsn_context = "lan_char(ab), b"
  I -> 0
  0 -> 1 [label = "\\e"]
  1 -> 0 [label = "\\e"]
  0 -> 4 [label = "a"]
  4 -> F
}''', '''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, b"
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
}''', algo)

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
}''', algo)


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
}''', algo)


    ## ---------------------- ##
    ## Forward vs. backward.  ##
    ## ---------------------- ##

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
}''').sort().strip(), a.proper(backward=True, algo=algo).sort().strip())

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
}''').sort().strip(), a.proper(backward=False, algo=algo).sort().strip())
