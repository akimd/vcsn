#! /usr/bin/env python
# -*- coding: utf-8 -*-

import vcsn

def check_reduce(a, exp):
    eff = a.reduce()
    CHECK_EQ(exp, eff)
    CHECK_EQ(exp, eff.reduce())
    CHECK_EQ(a.shortest(10), eff.shortest(10))

from test import *

# The following tests yield two different results, depending whether
# we work in Q/R vs. Z.  When asked whether this is expected, Sylvain
# Lombardy answers:
#
# Oui, c'est normal.
#
# Dans Q ou R, on normalise les vecteurs de la base (ceux qui
# correspondront aux états) de sorte que le pivot vaille 1, ce qu'on
# ne fait pas dans Z.  Du coup, dans Q ou R, les coefficients
# "sortent" plus vite que dans Z.
#
# Si on déroule l'algo, d'abord on a d'abord une réduction
# droite-gauche.
#
# - On part du vecteur final [22 11 11 11].
# - Le pivot choisi est l'indice 2.
# - Dans Z on garde ce vecteur et tous les vecteurs qu'on calcule en
#   sont des multiples, on obtient donc un seul état, initial de poids
#   22, une boucle 10a+5b et un poids final de 1.
# - Dans Q ou R, on normalise pour obtenir le vecteur [2 1 1 1], on
#   obtient un état de poids initial 2, de poids final 11 et une
#   boucle 10a+5b
#
# Ensuite on fait une réduction gauche-droite.
#
# - Dans Z, on a un vecteur [22] qui reste tel quel on obtient donc un
#   un état de poids initial 1 et de poids final 22 et la boucle.
# - Dans Q ou R, on a un vecteur [2] normalisé en [1] et donc un état
#   de poids initial 2 et de poids final 11 et la boucle.
#
# On pourrait imaginer qu'on normalise les vecteurs dans Z en divisant
# leurs coordonnées par le gcd de toutes les coordonnées, mais on sape
# de cette manière les arguments algébriques qui assurent la
# correction de l'algorithme (car on modifie le Z-module décrit par la
# base).
#
# Sur certains automates, on peut même obtenir des résultats
# complètement différents, je pense; c'est-à-dire des automates dont
# les supports n'ont rien à voir.

r = '<2>(<3>a+<5>b+<7>a)*<11>'

def exp(ws):
    '''The expected result for the following tests.'''
    if ws == 'z':
        return '''
  context = [abc]? → ℤ
  $ -> 0
  0 -> 0 <10>a, <5>b
  0 -> $ <22>
'''
    else:
        # Using replace() instead of format() allows to use
        # bin/update-tests.
        return '''
  context = [abc]? → ℚ
  $ -> 0 <2>
  0 -> 0 <10>a, <5>b
  0 -> $ <11>
'''.replace('ℚ', ws)

for ws in ['z', 'q', 'r']:
    ctx = vcsn.context('[abc] -> ' + ws)
    a = ctx.expression(r, 'associative').standard()
    check_reduce(a, vcsn.automaton(exp(ws)))

a = vcsn.context('[abc] x [xyz] -> z') \
    .expression("<2>(<3>(a|x)+<5>(b|y)+<7>(a|x))*<11>", 'associative') \
    .standard()
check_reduce(a, '''digraph
{
  vcsn_context = "[abc]? × [xyz]? → ℤ"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
  }
  I0 -> 0
  0 -> F0 [label = "<22>"]
  0 -> 0 [label = "<10>a|x, <5>b|y"]
}''')

## ------------------------------- ##
## Tools Sec. 3.3.1, Fig. 3.14.  ##
## ------------------------------- ##

a = vcsn.automaton('''
digraph {
    vcsn_context = "[abc]? → ℤ"
    I -> 0
    1 -> F
    0 -> 0 [label = "a, b"]
    0 -> 1 [label = "b"]
    0 -> 2 [label = "<2>b"]
    2 -> 2 [label = "<2>a, <2>b"]
    2 -> 1 [label = "<2>b"]
    1 -> 1 [label = "<4>a, <4>b"]
}
''')
check_reduce(a, '''digraph
{
  vcsn_context = "[abc]? → ℤ"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F1
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0
    1
    2
  }
  I0 -> 0
  0 -> 0 [label = "a, <9>b"]
  0 -> 1 [label = "b"]
  1 -> F1
  1 -> 0 [label = "<8>a, <-56>b"]
  1 -> 1 [label = "<4>a"]
  1 -> 2 [label = "<-1>a, <-3>b"]
  2 -> 0 [label = "<16>a, <-112>b"]
  2 -> 1 [label = "<-8>b"]
  2 -> 2 [label = "<2>a, <-2>b"]
}''')


# Make sure decorated automata work properly.
q = vcsn.context('[abc] -> q')
r = q.expression('<2>aa+<3>ab')
CHECK_EQ('⟨2⟩a(a+⟨3/2⟩b)', r.derived_term().reduce().expression('associative'))
