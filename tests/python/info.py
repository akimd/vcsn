#! /usr/bin/env python

import vcsn
from test import *

# check OBJ EXPECTED
# ------------------
# Check that OBJ.info(True) == EXPECTED.
def check(obj, exp):
    CHECK_EQ(exp, obj.info(detailed = True))

## ---------------- ##
## automaton.info.  ##
## ---------------- ##

# Check that ':', which is used as a separator by info, is also
# properly treated as a letter.
check(vcsn.context('lal_char(:a-z), q')
      .expression("a+a\:").standard(),
      {'is ambiguous': False,
       'is codeterministic': False,
       'is complete': False,
       'is cycle ambiguous': False,
       'is deterministic': False,
       'is empty': False,
       'is eps-acyclic': True,
       'is normalized': False,
       'is proper': True,
       'is standard': True,
       'is synchronizing': False,
       'is trim': True,
       'is useless': False,
       'is valid': True,
       'number of accessible states': 4,
       'number of coaccessible states': 4,
       'number of codeterministic states': 4,
       'number of deterministic states': 3,
       'number of spontaneous transitions': 0,
       'number of final states': 2,
       'number of initial states': 1,
       'number of states': 4,
       'number of strongly connected components': 4,
       'number of transitions': 3,
       'number of useful states': 4,
       'number of lazy states': 0,
       'type': 'mutable_automaton<letterset<char_letters(:abcdefghijklmnopqrstuvwxyz)>, q>'})

check(vcsn.context('lal_char(a-z), q')
      .expression("<1>a+(<2>b<3>*<4>)<5>").standard(),
      {'is ambiguous': False,
       'is codeterministic': False,
       'is complete': False,
       'is cycle ambiguous': False,
       'is deterministic': True,
       'is empty': False,
       'is eps-acyclic': True,
       'is normalized': False,
       'is proper': True,
       'is standard': True,
       'is synchronizing': False,
       'is trim': True,
       'is useless': False,
       'is valid': True,
       'number of accessible states': 3,
       'number of coaccessible states': 3,
       'number of codeterministic states': 2,
       'number of deterministic states': 3,
       'number of spontaneous transitions': 0,
       'number of final states': 3,
       'number of initial states': 1,
       'number of states': 3,
       'number of strongly connected components': 3,
       'number of transitions': 3,
       'number of useful states': 3,
       'number of lazy states': 0,
       'type': 'mutable_automaton<letterset<char_letters(abcdefghijklmnopqrstuvwxyz)>, q>'})

# Test what happens with "N/A".
check(vcsn.context('law_char(ab), b')
      .expression('a(a+b)*').standard(),
      {'is ambiguous': 'N/A',
       'is codeterministic': 'N/A',
       'is complete': 'N/A',
       'is cycle ambiguous': 'N/A',
       'is deterministic': 'N/A',
       'is empty': False,
       'is eps-acyclic': True,
       'is normalized': False,
       'is proper': True,
       'is standard': True,
       'is synchronizing': 'N/A',
       'is trim': True,
       'is useless': False,
       'is valid': True,
       'number of accessible states': 4,
       'number of coaccessible states': 4,
       'number of deterministic states': 'N/A',
       'number of codeterministic states': 'N/A',
       'number of spontaneous transitions': 0,
       'number of final states': 3,
       'number of initial states': 1,
       'number of states': 4,
       'number of strongly connected components': 3,
       'number of transitions': 7,
       'number of useful states': 4,
       'number of lazy states': 0,
       'type': 'mutable_automaton<wordset<char_letters(ab)>, b>'})


## ----------------- ##
## expression.info.  ##
## ----------------- ##

b = vcsn.context('lal_char(abc), b')
check(b.expression('abc'),
      {'type': 'expressionset<letterset<char_letters(abc)>, b>',
       'size': 5,
       'depth': 1,
       'width': 3,
       'sum': 0,
       'tuple': 0,
       'shuffle': 0,
       'conjunction': 0,
       'infiltration': 0,
       'prod': 1,
       'star': 0,
       'complement': 0,
       'zero': 0,
       'one': 0,
       'atom': 3,
       'ldiv': 0,
       'lweight': 0,
       'rweight': 0})

check(b.expression('\e+bc*'),
      {'type': 'expressionset<letterset<char_letters(abc)>, b>',
       'size': 6,
       'depth': 3,
       'width': 2,
       'sum': 1,
       'tuple': 0,
       'shuffle': 0,
       'conjunction': 0,
       'infiltration': 0,
       'prod': 1,
       'star': 1,
       'complement': 0,
       'zero': 0,
       'one': 1,
       'atom': 2,
       'ldiv': 0,
       'lweight': 0,
       'rweight': 0})

q = vcsn.context('lal_char(abc), q')
check(q.expression('<2>a<3>'),
      {'type': 'expressionset<letterset<char_letters(abc)>, q>',
       'size': 2,
       'depth': 1,
       'width': 1,
       'sum': 0,
       'tuple': 0,
       'shuffle': 0,
       'conjunction': 0,
       'infiltration': 0,
       'prod': 0,
       'star': 0,
       'complement': 0,
       'zero': 0,
       'one': 0,
       'atom': 1,
       'ldiv': 0,
       'lweight': 1,
       'rweight': 0})

check(q.expression('a{\}(<2>(\e+a+b)<3>)&(a:b)a*{c}{T}&:a', 'associative'),
      {'type': 'expressionset<letterset<char_letters(abc)>, q>(associative)',
       'size': 20,
       'depth': 6,
       'width': 7,
       'sum': 1,
       'tuple': 0,
       'shuffle': 1,
       'conjunction': 1,
       'infiltration': 1,
       'prod': 1,
       'star': 1,
       'complement': 1,
       'zero': 0,
       'one': 1,
       'atom': 7,
       'ldiv': 1,
       'lweight': 1,
       'rweight': 1})

# This expression is \z.
check(q.expression('(\z<2>(\e+a+b)<3>)&(a:b)a*{c}{T}'),
      {'type': 'expressionset<letterset<char_letters(abc)>, q>',
       'size': 1,
       'depth': 0,
       'width': 0,
       'sum': 0,
       'tuple': 0,
       'shuffle': 0,
       'conjunction': 0,
       'infiltration': 0,
       'prod': 0,
       'star': 0,
       'complement': 0,
       'zero': 1,
       'one': 0,
       'atom': 0,
       'ldiv': 0,
       'lweight': 0,
       'rweight': 0})

c = vcsn.context('lat<lan_char, lan_char>, q')
check(c.expression('a|x+b|y*+c|[xyz]'),
      {'type': 'expressionset<lat<nullableset<letterset<char_letters(abc)>>, nullableset<letterset<char_letters(xyz)>>>, q>',
       'size': 12,
       'depth': 4,
       'width': 7,
       'sum': 2,
       'tuple': 2,
       'shuffle': 0,
       'conjunction': 0,
       'infiltration': 0,
       'prod': 0,
       'star': 1,
       'complement': 0,
       'zero': 0,
       'one': 0,
       'atom': 7,
       'ldiv': 0,
       'lweight': 0,
       'rweight': 0})
