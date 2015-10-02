#! /usr/bin/env python

import vcsn
from test import *

def load_automaton_from(file):
  path = medir + "/" + file + ".gv"
  res = open(path).read().strip()
  return res

def suffix_check(i, o):
  o = load_automaton_from(o)
  CHECK_EQ(o, i.suffix())
  CHECK_EQ(o, i.suffix().suffix())

def prefix_check(i, o):
  o = load_automaton_from(o)
  CHECK_EQ(o, i.prefix())
  CHECK_EQ(o, i.prefix().prefix())

def factor_check(i, o):
  o = load_automaton_from(o)
  CHECK_EQ(o, i.factor())
  CHECK_EQ(o, i.factor().factor())

def subword_check(i, o):
  o = load_automaton_from(o)
  CHECK_EQ(o, i.subword())
  # subword is not idempotent.



# Test suffix, prefix, factor, and subword on five automata.
for i in range(1, 5):
  base = "aut{}".format(i)
  print("Working on {}".format(base))
  aut = vcsn.automaton(load_automaton_from(base))
  suffix_check(aut, base + "-suffix")
  prefix_check(aut, base + "-prefix")
  factor_check(aut, base + "-factor")
  subword_check(aut, base + "-subword")
