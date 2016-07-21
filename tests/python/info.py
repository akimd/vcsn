#! /usr/bin/env python

import vcsn
from test import *

# check OBJ EXPECTED
# ------------------
# Check that OBJ.info(True) == EXPECTED.


def check(obj, exp):
    CHECK_EQ(exp, obj.info(details=3))

## ---------------- ##
## automaton.info.  ##
## ---------------- ##

# Check that ':', which is used as a separator by info, is also
# properly treated as a letter.
check(vcsn.context('lal_char(:a-z), q')
      .expression(r"a+a\:").standard(),
      {
          'is ambiguous': False,
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
          'number of final states': 2,
          'number of initial states': 1,
          'number of lazy states': 0,
          'number of spontaneous transitions': 0,
          'number of states': 4,
          'number of strongly connected components': 4,
          'number of transitions': 3,
          'number of useful states': 4,
          'type': 'mutable_automaton<letterset<char_letters(:abcdefghijklmnopqrstuvwxyz)>, q>',
      })

check(vcsn.context('lal_char(a-z), q')
      .expression("<1>a+(<2>b<3>*<4>)<5>").standard(),
      {
          'is ambiguous': False,
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
          'number of final states': 3,
          'number of initial states': 1,
          'number of lazy states': 0,
          'number of spontaneous transitions': 0,
          'number of states': 3,
          'number of strongly connected components': 3,
          'number of transitions': 3,
          'number of useful states': 3,
          'type': 'mutable_automaton<letterset<char_letters(abcdefghijklmnopqrstuvwxyz)>, q>',
      })

# Test what happens with "N/A".
check(vcsn.context('law_char(ab), b')
      .expression('a(a+b)*').standard(),
      {
          'is ambiguous': 'N/A',
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
          'number of codeterministic states': 'N/A',
          'number of deterministic states': 'N/A',
          'number of final states': 3,
          'number of initial states': 1,
          'number of lazy states': 0,
          'number of spontaneous transitions': 0,
          'number of states': 4,
          'number of strongly connected components': 3,
          'number of transitions': 7,
          'number of useful states': 4,
          'type': 'mutable_automaton<wordset<char_letters(ab)>, b>',
      })


## ----------------- ##
## expression.info.  ##
## ----------------- ##


# expression.info does not accept a `details` parameter.
def check(obj, exp):
    CHECK_EQ(exp, obj.info())

b = vcsn.context('lal_char(abc), b')
check(b.expression('abc'),
      {
          'add': 0,
          'atom': 3,
          'complement': 0,
          'compose': 0,
          'conjunction': 0,
          'depth': 1,
          'infiltrate': 0,
          'ldivide': 0,
          'lweight': 0,
          'mul': 1,
          'one': 0,
          'rweight': 0,
          'shuffle': 0,
          'size': 5,
          'length': 5,
          'star': 0,
          'transposition': 0,
          'tuple': 0,
          'type': 'expressionset<letterset<char_letters(abc)>, b>',
          'width': 3,
          'zero': 0,
      })

check(b.expression(r'\e+bc*'),
      {
          'add': 1,
          'atom': 2,
          'complement': 0,
          'compose': 0,
          'conjunction': 0,
          'depth': 3,
          'infiltrate': 0,
          'ldivide': 0,
          'lweight': 0,
          'mul': 1,
          'one': 1,
          'rweight': 0,
          'shuffle': 0,
          'size': 6,
          'length': 6,
          'star': 1,
          'transposition': 0,
          'tuple': 0,
          'type': 'expressionset<letterset<char_letters(abc)>, b>',
          'width': 2,
          'zero': 0,
      })

q = vcsn.context('lal_char(abc), q')
check(q.expression('<2>a<3>'),
      {
          'add': 0,
          'atom': 1,
          'complement': 0,
          'compose': 0,
          'conjunction': 0,
          'depth': 1,
          'infiltrate': 0,
          'ldivide': 0,
          'lweight': 1,
          'mul': 0,
          'one': 0,
          'rweight': 0,
          'shuffle': 0,
          'size': 2,
          'length': 2,
          'star': 0,
          'transposition': 0,
          'tuple': 0,
          'type': 'expressionset<letterset<char_letters(abc)>, q>',
          'width': 1,
          'zero': 0,
      })

check(q.expression(r'(a{\}(<2>(\e+a+b)<3>)&(a:b)a*{c}{T}&:a){3}', 'associative'),
      {
          'add': 3,
          'atom': 21,
          'complement': 3,
          'compose': 0,
          'conjunction': 3,
          'depth': 7,
          'infiltrate': 3,
          'ldivide': 3,
          'lweight': 3,
          'mul': 4,
          'one': 3,
          'rweight': 3,
          'shuffle': 3,
          'size': 62,
          'length': 62,
          'star': 3,
          'transposition': 3,
          'tuple': 0,
          'type': 'expressionset<letterset<char_letters(abc)>, q>(associative)',
          'width': 21,
          'zero': 0,
      })

# This expression is \z.
check(q.expression(r'(\z<2>(\e+a+b)<3>)&(a:b)a*{c}{T}'),
      {
          'add': 0,
          'atom': 0,
          'complement': 0,
          'compose': 0,
          'conjunction': 0,
          'depth': 0,
          'infiltrate': 0,
          'ldivide': 0,
          'lweight': 0,
          'mul': 0,
          'one': 0,
          'rweight': 0,
          'shuffle': 0,
          'size': 1,
          'length': 1,
          'star': 0,
          'transposition': 0,
          'tuple': 0,
          'type': 'expressionset<letterset<char_letters(abc)>, q>',
          'width': 0,
          'zero': 1,
      })

c = vcsn.context('lat<lan_char, lan_char>, q')
check(c.expression(r'a|x + b|y* + (c|[xyz] @ [xyz]|\e)'),
      {
          'add': 3,
          'atom': 10,
          'complement': 0,
          'compose': 1,
          'conjunction': 0,
          'depth': 6,
          'infiltrate': 0,
          'ldivide': 0,
          'lweight': 0,
          'mul': 0,
          'one': 1,
          'rweight': 0,
          'shuffle': 0,
          'size': 19,
          'length': 19,
          'star': 1,
          'transposition': 0,
          'tuple': 3,
          'type': 'expressionset<lat<nullableset<letterset<char_letters(abcxyz)>>, nullableset<letterset<char_letters(xyz)>>>, q>',
          'width': 10,
          'zero': 0,
      })
