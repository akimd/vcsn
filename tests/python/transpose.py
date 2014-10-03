#! /usr/bin/env python

from __future__ import print_function

import vcsn
from test import *

## ---------------------- ##
## transpose(automaton).  ##
## ---------------------- ##

c = vcsn.context("lal_char(abc)_ratexpset<lal_char(xyz)_z>")
a = c.ratexp("(<xyz>abc)*").derived_term()
CHECK_EQ('''digraph
{
  vcsn_context = "lal_char(abc)_ratexpset<lal_char(xyz)_z>"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "(<xyz>abc)*", shape = box]
    1 [label = "bc(<xyz>abc)*", shape = box]
    2 [label = "c(<xyz>abc)*", shape = box]
  }
  I0 -> 0
  0 -> F0
  0 -> 2 [label = "c"]
  1 -> 0 [label = "<zyx>a"]
  2 -> 1 [label = "b"]
}''',
         a.transpose())
CHECK_EQ(a, a.transpose().transpose())

# a and a.transpose().transpose() must be exactly the same object, in
# particular its type should be mutable_automaton, not
# transpose_automaton<transpose_automaton<mutable_automaton>>.
CHECK_EQ(a.info(), a.transpose().transpose().info())

# Regression: we used to not transpose the number of initial and final
# transitions.
a = vcsn.context('lal_char_b').ratexp('a+b').standard()
CHECK_EQ({
           'is complete': False,
           'is codeterministic': True,
           'is deterministic': False,
           'is empty': False,
           'is eps-acyclic': True,
           'is normalized': False,
           'is proper': True,
           'is standard': False,
           'is trim': True,
           'is useless': False,
           'is valid': True,
           'number of accessible states': 3,
           'number of coaccessible states': 3,
           'number of codeterministic states': 3,
           'number of deterministic states': 3,
           'number of eps transitions': 0,
           'number of final states': 1,
           'number of initial states': 2,
           'number of states': 3,
           'number of strongly connected components': 3,
           'number of transitions': 2,
           'number of useful states': 3,
           'type': 'transpose_automaton<mutable_automaton<lal_char(ab)_b>>',
         },
         a.transpose().info())

# Stripping a transposed automaton strips the inner automaton, but not
# the transposition shell.
a = vcsn.context('lal_char_b').ratexp('ab').derived_term().determinize()
CHECK_EQ('transpose_automaton<mutable_automaton<lal_char(ab)_b>>',
         a.transpose().strip().info()['type'])

## ------------------- ##
## transpose(ratexp).  ##
## ------------------- ##

def check(re, exp):
    exp = ctx.ratexp(exp)
    r = ctx.ratexp(re)
    CHECK_EQ(exp, r.transpose())
    CHECK_EQ(r, exp.transpose())

ctx = vcsn.context('lal_char(abcd)_b')
check('\e', '\e')
check('\z', '\z')
check('a', 'a')
check('ab', 'ba')
check('abc+aba', 'cba+aba')
check('abc&aba', 'cba&aba')
check('(ab)*', '(ba)*')
check('(abcd){T}', '(abcd){T}{T}')
check('ab{\}cd', '(ab{\}cd){T}')
check('ab{/}cd', '((cd){T}{\}(ab){T})')

ctx = vcsn.context('law_char(abcd)_b')
check('\e', '\e')
check('\z', '\z')
check('a', 'a')
check('ab', 'ba')
check('abc+aba', 'cba+aba')
check('(ab)*&(ab)*', '(ba)*&(ba)*')
check('(ab)*', '(ba)*')

ctx = vcsn.context('law_char(abcd)_ratexpset<law_char(efgh)_ratexpset<law_char(xyz)_z>>')
check('<<<2>(xy)>(ef)>(abcd)', '<<<2>(yx)>(fe)>(dcba)')
check('<(ef)>(abcd)*<(gh)>', '<(hg)>(dcba)*<(fe)>')
