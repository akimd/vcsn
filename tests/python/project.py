#! /usr/bin/env python

import vcsn
from test import *

c = vcsn.context('lat<lal_char(abc), lal_char(efg), lal_char(xyz)>, q')

## ---------- ##
## automata.  ##
## ---------- ##

t = c.expression("((a|e|x):(b|f|y):(c|g|z))").automaton()

def check(function_name, type_):
    '''Check a function (`project` or `focus`).  Expect an automaton
    of type `type_`.'''
    fun = getattr(t, function_name)
    CHECK_EQ('(ab+ba)c+(ac+ca)b+(bc+cb)a',
             fun(0).expression())
    CHECK_EQ('(ef+fe)g+(eg+ge)f+(fg+gf)e',
             fun(1).expression())
    CHECK_EQ('(xy+yx)z+(xz+zx)y+(yz+zy)x',
             fun(2).expression())
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
               'type': type_,
               },
             fun(0).info(detailed = True))

check('focus',
      'focus_automaton<0, mutable_automaton<lat<letterset<char_letters(abc)>, letterset<char_letters(efg)>, letterset<char_letters(xyz)>>, q>>')
check('project',
      'mutable_automaton<letterset<char_letters(abc)>, q>')


## ---------- ##
## contexts.  ##
## ---------- ##

CHECK_EQ('{abc} -> Q', c.project(0))
CHECK_EQ('{efg} -> Q', c.project(1))
CHECK_EQ('{xyz} -> Q', c.project(2))


## ------------- ##
## polynomials.  ##
## ------------- ##

p = c.polynomial('<2>a|e|x + <3>a|f|x + <4>a|f|y')
CHECK_EQ('<9>a',        p.project(0))
CHECK_EQ('<2>e + <7>f', p.project(1))
CHECK_EQ('<5>x + <4>y', p.project(2))

## ------- ##
## label.  ##
## ------- ##

l = c.label('a|e|x')
CHECK_EQ('a', l.project(0))
CHECK_EQ('e', l.project(1))
CHECK_EQ('x', l.project(2))
