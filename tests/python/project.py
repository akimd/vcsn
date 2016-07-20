#! /usr/bin/env python

import vcsn
from test import *

# Tests are sorted per dependency: dependant types come after needed
# ones.


# Nullable labelsets to please expansions.
c = vcsn.context('lat<lan(abc), lan(efg), lan(xyz)>, q')

def check(v, *p):
    for i in range(3):
        CHECK_EQ(p[i], v.project(i))


## ---------- ##
## contexts.  ##
## ---------- ##

check(c,
      '{abc}? -> Q',
      '{efg}? -> Q',
      '{xyz}? -> Q')


## ------- ##
## label.  ##
## ------- ##

check(c.label('a|e|x'),
      'a',
      'e',
      'x')

## ---------- ##
## automata.  ##
## ---------- ##

# We do not support properly focus automata on LAN.
c2 = vcsn.context('lat<lal(abc), lal(efg), lal(xyz)>, q')
t = c2.expression("(a|e|x) : (b|f|y) : (c|g|z)").automaton()

def check_aut(function_name, type_):
    '''Check a function (`project` or `focus`).  Expect an automaton
    of type `type_`.'''
    print("Checking:", function_name)
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
             fun(0).info(detailed=True))

check_aut('focus',
          'focus_automaton<0, mutable_automaton<lat<letterset<char_letters(abc)>, letterset<char_letters(efg)>, letterset<char_letters(xyz)>>, q>>')
check_aut('project',
          'mutable_automaton<letterset<char_letters(abc)>, q>')


## ------------- ##
## expressions.  ##
## ------------- ##

check(c.expression('<2>a*|[ef]|xy + <3>a*|f|x + <4>a*|f|y'),
      '<9>a*',
      'e+<3>f',
      'x+y+xy')

# Check identities.
e = c.expression('<2>(ab|ef|xyz)<3>', 'associative')
check(e,
      '<2>(ab)<3>',
      '<2>(ef)<3>',
      '<2>(xyz)<3>')
CHECK_EQ('associative', e.project(0).identities())


## ------------- ##
## polynomials.  ##
## ------------- ##

check(c.polynomial('<2>a|e|x + <3>a|f|x + <4>a|f|y'),
      '<9>a',
      '<2>e + <7>f',
      '<5>x + <4>y')


## ------------ ##
## expansions.  ##
## ------------ ##

# Obviously, `expansion` and `project` don't commute.  For instance,
# `a|[xy]` projected on 0 gives `a`, while expanded and projected
# gives `<2>a`.
e = c.expression('<2>a*|[ef]|xy + <3>a*|f|x + <4>a*|f|y')
check(e.expansion(),
      '<11> + a.[<11>a*]',
      r'e.[<4>\e] + f.[<18>\e]',
      r'x.[<6>\e + <8>y] + y.[<8>\e]')
