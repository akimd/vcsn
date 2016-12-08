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
    f0 = fun(0)
    CHECK_EQ('(ab+ba)c+(ac+ca)b+(bc+cb)a',
             f0.expression())
    CHECK_EQ('(ef+fe)g+(eg+ge)f+(fg+gf)e',
             fun(1).expression())
    CHECK_EQ('(xy+yx)z+(xz+zx)y+(yz+zy)x',
             fun(2).expression())
    CHECK_EQ({
               'is ambiguous': False,
               'is codeterministic': True,
               'is complete': False,
               'is cycle ambiguous': False,
               'is deterministic': 'N/A',
               'is empty': False,
               'is eps-acyclic': True,
               'is normalized': True,
               'is proper': 'N/A',
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
             fun(0).info(details=3))
    # Check properties on tape.
    if function_name == 'focus':
        t_info = t.info()
        t_proper = t_info['is proper']
        t_deter = t_info['is deterministic']

        f0_info = f0.info()
        CHECK_EQ('N/A', f0_info['is proper'])
        CHECK_EQ('N/A', f0_info['is deterministic'])

        f0_proper = f0.is_proper()
        CHECK_EQ(t_proper, t.info('is proper'))
        CHECK_EQ(f0_proper, f0.info('is proper'))

        f0_deter = f0.is_deterministic()
        CHECK_EQ(t_deter, t.info('is deterministic'))
        CHECK_EQ(f0_deter, f0.info('is deterministic'))


check_aut('focus',
          'focus_automaton<0, mutable_automaton<lat<letterset<char_letters(abc)>, letterset<char_letters(efg)>, letterset<char_letters(xyz)>>, q>>')
check_aut('project',
          'mutable_automaton<letterset<char_letters(abc)>, q>')


## ------------- ##
## expressions.  ##
## ------------- ##

# Check that we do obey the identities.
check(c.expression('<2>a*|[ef]|xy + <3>a*|f|x + <4>a*|f|y'),
      '<9>a*',
      'e+<3>f',
      'x+y+xy')

# Check identities.
check(vcsn
      .context('lat<lan, lan, lan>, b')
      .expression('(a{c}|e{c}|x{c}){c}'),
      'a', 'e', 'x')

e = c.expression('<2>(ab|ef|xyz)<3>', 'associative')
check(e,
      '<2>(ab)<3>',
      '<2>(ef)<3>',
      '<2>(xyz)<3>')
CHECK_EQ('associative', e.project(0).identities())

# Composition does not make any sense, but it's not `project` that
# should know about it.  At some point, it should be an error.
for op in ['&', ':', '&:', r'{\}', '@']:
    check(c.expression('(a*|e*|x*){op}(a*|e*|x*)'.format(op=op)),
          'a*{op}a*'.format(op=op),
          'e*{op}e*'.format(op=op),
          'x*{op}x*'.format(op=op))
check(c.expression('(a*|e*|x*){2}'),
      'a*{2}', 'e*{2}', 'x*{2}')


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
