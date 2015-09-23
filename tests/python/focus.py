#! /usr/bin/env python

import vcsn
from test import *

c = vcsn.context('lat<lal_char(abc),lal_char(efg),lal_char(xyz)>, b')
t = c.expression("((a|e|x):(b|f|y):(c|g|z))").automaton()

CHECK_EQ('(ab+ba)c+(ac+ca)b+(bc+cb)a',
         t.focus(0).expression())

CHECK_EQ('(ef+fe)g+(eg+ge)f+(fg+gf)e',
         t.focus(1).expression())

CHECK_EQ('(xy+yx)z+(xz+zx)y+(yz+zy)x',
         t.focus(2).expression())

# Info runs many algorithms, which also involves duplicating the input
# automaton, which is not trivial in the case of focus automata.
# Exercise this.
CHECK_EQ({
           'is ambiguous': False,
           'is codeterministic': True,
           'is complete': False,
           'is cycle ambiguous': False,
           'is deterministic': True,
           'is empty': False,
           'is eps-acyclic': True,
           'is normalized': True,
           'is proper': True,
           'is standard': True,
           'is synchronizing': False,
           'is trim': True,
           'is useless': False,
           'is valid': True,
           'number of accessible states': 8,
           'number of coaccessible states': 8,
           'number of codeterministic states': 8,
           'number of deterministic states': 8,
           'number of final states': 1,
           'number of initial states': 1,
           'number of spontaneous transitions': 0,
           'number of states': 8,
           'number of strongly connected components': 8,
           'number of transitions': 12,
           'number of useful states': 8,
           'number of lazy states': 0,
           'type': 'focus_automaton<0, mutable_automaton<lat<letterset<char_letters(abc)>, letterset<char_letters(efg)>, letterset<char_letters(xyz)>>, b>>',
         },
         t.focus(0).info(detailed = True))
