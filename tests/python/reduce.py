#! /usr/bin/env python
# -*- coding: utf-8 -*-

import vcsn

from test import *

# The following tests yields two different results, depending whether
# we work in Q/R vs. Z.  When asked whether this is expecting, Sylvain
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
        return '''digraph
{
  vcsn_context = "lal_char(abc)_''' + ws + '''"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F0
  }
  {
    node [shape = circle]
    0
  }
  I0 -> 0
  0 -> F0 [label = "<22>"]
  0 -> 0 [label = "<10>a, <5>b"]
}'''
    else:
        return '''digraph
{
  vcsn_context = "lal_char(abc)_''' + ws + '''"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F0
  }
  {
    node [shape = circle]
    0
  }
  I0 -> 0 [label = "<2>"]
  0 -> F0 [label = "<11>"]
  0 -> 0 [label = "<10>a, <5>b"]
}'''

for ws in ['z', 'q', 'r']:
    ctx = vcsn.context('lal_char(abc)_' + ws)
    a = ctx.ratexp(r).standard()
    eff = a.reduce()
    CHECK_EQ(exp(ws), eff)
    CHECK_EQ(exp(ws), eff.reduce())
