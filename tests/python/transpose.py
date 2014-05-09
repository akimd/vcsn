#! /usr/bin/env python

from __future__ import print_function

import vcsn
from test import *

c = vcsn.context("lal_char(abc)_ratexpset<lal_char(xyz)_z>")

def check(re, exp):
    global ctx
    exp = ctx.ratexp(exp)
    r = ctx.ratexp(re)
    CHECK_EQ(exp, r.transpose())
    CHECK_EQ(r, exp.transpose())

## ------------------- ##
## transpose(ratexp).  ##
## ------------------- ##

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
