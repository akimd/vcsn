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
