#! /usr/bin/env python
# -*- coding: utf-8 -*-

import vcsn
from test import *
from vcsn.demangle import demangle, sugar

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

check('context<nullableset<letterset<char_letters>>, b>',
      '(chars)? → b',
      'nullableset<letterset<chars>>, b')

check('context<letterset<char_letters>, expressionset<context<letterset<char_letters>, b>>>',
      'chars → expressionset<chars → b>',
      'letterset<chars>, expressionset<context<letterset<chars>, b>>')

check('transpose_automaton<mutable_automaton<context<letterset<char_letters>, b>>>',
      'transpose<mutable<chars → b>>',
      'transpose<mutable<context<letterset<chars>, b>>>')

check('name_automaton<mutable_automaton<context<letterset<char_letters>, expressionset<context<letterset<char_letters>, b>>>>>',
      'name<mutable<chars → expressionset<chars → b>>>',
      'name<mutable<context<letterset<chars>, expressionset<context<letterset<chars>, b>>>>>')


## ---------- ##
## demangle.  ##
## ---------- ##

def check(input, exp_regex, exp_re = None):
    '''Check that `demangle(input)` is `exp1` when regex is available,
    otherwise `exp2`.'''
    if exp_re == None:
        exp_re = input
    exp = exp_regex if has_regex else exp_re
    CHECK_EQ(exp, demangle(input, color="never"))

check('index_t_impl<vcsn::detail::mutable_automaton_impl<vcsn::context<vcsn::letterset<vcsn::set_alphabet<vcsn::char_letters> >, vcsn::weightset_mixin<vcsn::detail::b_impl> > >::state_tag>',
      'vcsn::detail::mutable_automaton_impl<context<lal_char, b > >::state_t',
      'index_t_impl<vcsn::detail::mutable_automaton_impl<vcsn::context<lal_char, b > >::state_tag>')

check('''
vcsn/algos/pair.hh:229:21: error: no viable conversion from 'index_t_impl<vcsn::detail::mutable_automaton_impl<vcsn::context<vcsn::letterset<vcsn::set_alphabet<vcsn::char_letters> >, vcsn::weightset_mixin<vcsn::detail::b_impl> > >::state_tag>' to 'index_t_impl<vcsn::detail::mutable_automaton_impl<vcsn::context<vcsn::weightset_mixin<vcsn::detail::tupleset_impl<vcsn::letterset<vcsn::set_alphabet<vcsn::char_letters> >, vcsn::letterset<vcsn::set_alphabet<vcsn::char_letters> >, vcsn::letterset<vcsn::set_alphabet<vcsn::char_letters> > > >, vcsn::weightset_mixin<vcsn::detail::b_impl> > >::state_tag>'
      state_t q0_ = this->null_state();
                    ^~~~~~~~~~~~~~~~~~
''',
      '''
vcsn/algos/pair.hh:229:21: error: no viable conversion from 'vcsn::detail::mutable_automaton_impl<context<lal_char, b > >::state_t' to 'vcsn::detail::mutable_automaton_impl<context<tupleset<lal_char, lal_char, lal_char >, b > >::state_t'
      state_t q0_ = this->null_state();
                    ^~~~~~~~~~~~~~~~~~
''',
      '''
vcsn/algos/pair.hh:229:21: error: no viable conversion from 'index_t_impl<vcsn::detail::mutable_automaton_impl<vcsn::context<lal_char, b > >::state_tag>' to 'index_t_impl<vcsn::detail::mutable_automaton_impl<vcsn::context<vcsn::weightset_mixin<vcsn::detail::tupleset_impl<lal_char, lal_char, lal_char > >, b > >::state_tag>'
      state_t q0_ = this->null_state();
                    ^~~~~~~~~~~~~~~~~~
''')

check('''
vcsn/misc/pair.hh:36:12: error: no viable conversion from 'pair<typename __make_pair_return<index_t_impl<state_tag> &>::type, typename __make_pair_return<index_t_impl<state_tag> &>::type>' to 'pair<vcsn::detail::index_t_impl<vcsn::detail::mutable_automaton_impl<vcsn::context<vcsn::weightset_mixin<vcsn::detail::tupleset_impl<vcsn::letterset<vcsn::set_alphabet<vcsn::char_letters> >, vcsn::letterset<vcsn::set_alphabet<vcsn::char_letters> >, vcsn::letterset<vcsn::set_alphabet<vcsn::char_letters> > > >, vcsn::weightset_mixin<vcsn::detail::b_impl> > >::state_tag> &, vcsn::detail::index_t_impl<vcsn::detail::mutable_automaton_impl<vcsn::context<vcsn::weightset_mixin<vcsn::detail::tupleset_impl<vcsn::letterset<vcsn::set_alphabet<vcsn::char_letters> >, vcsn::letterset<vcsn::set_alphabet<vcsn::char_letters> >, vcsn::letterset<vcsn::set_alphabet<vcsn::char_letters> > > >, vcsn::weightset_mixin<vcsn::detail::b_impl> > >::state_tag> &>'
''',
      '''
vcsn/misc/pair.hh:36:12: error: no viable conversion from 'pair<typename __make_pair_return<index_t_impl<state_tag> &>::type, typename __make_pair_return<index_t_impl<state_tag> &>::type>' to 'pair<vcsn::detail::mutable_automaton_impl<context<tupleset<lal_char, lal_char, lal_char >, b > >::state_t &, vcsn::detail::mutable_automaton_impl<context<tupleset<lal_char, lal_char, lal_char >, b > >::state_t &>'
''',
      '''
vcsn/misc/pair.hh:36:12: error: no viable conversion from 'pair<typename __make_pair_return<index_t_impl<state_tag> &>::type, typename __make_pair_return<index_t_impl<state_tag> &>::type>' to 'pair<vcsn::detail::index_t_impl<vcsn::detail::mutable_automaton_impl<vcsn::context<vcsn::weightset_mixin<vcsn::detail::tupleset_impl<lal_char, lal_char, lal_char > >, b > >::state_tag> &, vcsn::detail::index_t_impl<vcsn::detail::mutable_automaton_impl<vcsn::context<vcsn::weightset_mixin<vcsn::detail::tupleset_impl<lal_char, lal_char, lal_char > >, b > >::state_tag> &>'
''')
