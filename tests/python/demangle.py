#! /usr/bin/env python
# -*- coding: utf-8 -*-

import vcsn
from test import *
from vcsn.demangle import sugar

try:
    import regex
    has_regex = True
except ImportError:
    has_regex = False


## ------- ##
## sugar.  ##
## ------- ##

def check(input, exp_regex, exp_re):
    '''Check that `sugar(input)` is `exp1` when regex is available,
    otherwise `exp2`.'''
    exp = exp_regex if has_regex else exp_re
    CHECK_EQ(exp, sugar(input))

check('context<letterset<char_letters>, expressionset<context<letterset<char_letters>, b>>>',
      'chars → expressionset<chars → b>',
      'letterset<chars>, expressionset<context<letterset<chars>, b>>')

check('transpose_automaton<mutable_automaton<context<letterset<char_letters>, b>>>',
      'transpose<mutable<chars → b>>',
      'transpose<mutable<context<letterset<chars>, b>>>')

check('name_automaton<mutable_automaton<context<letterset<char_letters>, expressionset<context<letterset<char_letters>, b>>>>>',
      'name<mutable<chars → expressionset<chars → b>>>',
      'name<mutable<context<letterset<chars>, expressionset<context<letterset<chars>, b>>>>>')
