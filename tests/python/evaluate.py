#! /usr/bin/env python

import vcsn
from test import *

## check AUTOMATON WORD EXP
## ------------------------
def check(aut, word, exp):
    exp = ctx.weight(exp)
    CHECK_EQ(exp, aut.eval(word))
    CHECK_EQ(exp, aut(word))

## ------------- ##
## lal_char, z.  ##
## law_char, z.  ##
## ------------- ##
for c in ["lal_char(ab), z", "law_char(ab), z"]:
    ctx = vcsn.context(c)
    simple = vcsn.automaton('''
    digraph
    {
      vcsn_context = "lal_char(ab), z"
      rankdir = LR
      node [shape = circle]
      {
        node [shape = point, width = 0]
        I0
        F1
      }
      0 -> 0 [label = "a, b"]
      0 -> 1 [label = "<2>a"]
      1 -> 1 [label = "a, b"]
      1 -> F1
      I0 -> 0
    }
    ''').automaton(ctx)
    check(simple, 'aabab', '6')
    check(simple, 'aabab', '6')
    check(simple, 'abab',  '4')
    check(simple, 'aaaa',  '8')
    check(simple, 'b',     '0')
    check(simple, 'a',     '2')

    initial_weight = vcsn.automaton('''
    digraph
    {
      vcsn_context = "lal_char(ab), z"
      rankdir = LR
      node [shape = circle]
      {
        node [shape = point, width = 0]
        I0
        F1
      }
      0 -> 0 [label = "a, b"]
      0 -> 1 [label = "<2>a"]
      1 -> 1 [label = "a, b"]
      1 -> F1
      I0 -> 0 [label = "<2>"]
    }
    ''').automaton(ctx)
    check(initial_weight, 'a',      '4')
    check(initial_weight, 'abab',   '8')
    check(initial_weight, 'aabab', '12')

for c in ["lal_char(abc), z", "law_char(abc), z"]:
    ctx = vcsn.context(c)
    more_letters = vcsn.automaton('''
    digraph
    {
      vcsn_context = "lal_char(abc), z"
      rankdir = LR
      node [shape = circle]
      {
        node [shape = point, width = 0]
        I0
        F1
      }
      0 -> 0 [label = "a, b"]
      0 -> 1 [label = "<2>a"]
      0 -> 2 [label = "c"]
      1 -> 1 [label = "a, b"]
      1 -> F1
      2 -> 0 [label = "a"]
      2 -> 1 [label = "a"]
      I0 -> 0 [label = "<2>"]
    }
    ''').automaton(ctx)
    check(more_letters, 'caa', '6')

    prod = vcsn.automaton('''
    digraph
    {
      vcsn_context = "lal_char(abc), z"
      rankdir = LR
      node [shape = circle]
      {
        node [shape = point, width = 0]
        I0
        F1
      }
      0 -> 0 [label = "a, b"]
      0 -> 1 [label = "<2>a"]
      0 -> 2 [label = "c"]
      1 -> 1 [label = "a, b"]
      1 -> F1
      2 -> 0 [label = "a"]
      2 -> 1 [label = "a"]
      I0 -> 0 [label = "<2>"]
    }
    ''').automaton(ctx)
    check(prod, "aabab", '12')

    cmplex = vcsn.automaton('''
    digraph
    {
      vcsn_context = "lal_char(abc), z"
      rankdir = LR
      node [shape = circle]
      {
        node [shape = point, width = 0]
        I0
        F1
      }
      0 -> 0 [label = "a, b"]
      0 -> 1 [label = "<2>a"]
      0 -> 2 [label = "c"]
      1 -> 1 [label = "a, b"]
      1 -> F1 [label = "<3>"]
      2 -> 0 [label = "a"]
      2 -> 1 [label = "a"]
      I0 -> 0 [label = "<2>"]
    }
    ''').automaton(ctx)
    check(cmplex, 'a',     '12')
    check(cmplex, 'abab',  '24')
    check(cmplex, 'aabab', '36')


## --------------- ##
## lal_char_zmin.  ##
## law_char_zmin.  ##
## --------------- ##

for c in ['lal_char(abc), zmin','law_char(abc), zmin']:
    ctx = vcsn.context(c)
    a = ctx.expression('a').standard()
    check(a, '',   'oo')
    check(a, 'a',  '0')
    check(a, 'aa', 'oo')
    check(a, 'b',  'oo')

    minab = load('lal_char_zmin/minab.gv').automaton(ctx)
    check(minab, '',          '0')
    check(minab, 'b',         '0')
    check(minab, 'a',         '0')
    check(minab, 'ab',        '1')
    check(minab, 'abababbbb', '3')

    minblocka = load('lal_char_zmin/minblocka.gv').automaton(ctx)
    check(minblocka, '',          'oo')
    check(minblocka, 'b',         'oo')
    check(minblocka, 'a',         'oo')
    check(minblocka, 'ab',        'oo')
    check(minblocka, 'abababbbb', '0')
    check(minblocka, 'aabaaba',   '2')

    slowgrow = load('lal_char_zmin/slowgrow.gv').automaton(ctx)
    check(slowgrow, '',         'oo')
    check(slowgrow, 'b',        'oo')
    check(slowgrow, 'a',        'oo')
    check(slowgrow, 'ab',       'oo')
    check(slowgrow, 'abababb',  '0')
    check(slowgrow, 'abbaaa',   '0')
    check(slowgrow, 'abbababa', '1')
    check(slowgrow, 'baaaab',   '4')



## ------------- ##
## law_char, z.  ##
## ------------- ##
ctx = vcsn.context('law_char(abcdef), z')
a = ctx.expression('<2>(ab(<3>cd)*(ef))<5>', 'associative').automaton()
check(a, 'abef', '10')
check(a, 'abcdef', '30')
check(a, 'abcdcdef', '90')
check(a, 'abdef', '0')
check(a, 'abdcef', '0')
check(a, 'abcef', '0')
check(a, 'abc', '0')
check(a, 'abcd', '0')

a_epsilon = vcsn.automaton('''
digraph
{
  vcsn_context = "wordset<char_letters(abcdef)>, q"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F2
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
  }
  I0 -> 0
  0 -> 1 [label = "<2>ab"]
  1 -> 1 [label = "<3>cd"]
  1 -> 2 [label = "ef"]
  2 -> F2 [label = "<5>"]
}
''')
check(a_epsilon, 'abef', '10')
check(a_epsilon, 'abcdef', '30')
check(a_epsilon, 'abcdcdef', '90')
check(a_epsilon, 'abdef', '0')
check(a_epsilon, 'abdcef', '0')
check(a_epsilon, 'abcef', '0')
check(a_epsilon, 'abc', '0')
check(a_epsilon, 'abcd', '0')
