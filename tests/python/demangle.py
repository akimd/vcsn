#! /usr/bin/env python

import vcsn
from test import *
from vcsn.demangle import sugar

def check_sugar(input, exp):
    CHECK_EQ(exp, sugar(input))

check_sugar('''
context:       context<letterset<char_letters>, expressionset<context<letterset<char_letters>, b>>>
is_out_sorted: transpose_automaton<mutable_automaton<context<letterset<char_letters>, b>>>
strip:         name_automaton<mutable_automaton<context<letterset<char_letters>, expressionset<context<letterset<char_letters>, b>>>>>''',
            '''
context:       chars → expressionset<chars → b>
is_out_sorted: transpose<mutable<chars → b>>
strip:         name<mutable<chars → expressionset<chars → b>>>''')
