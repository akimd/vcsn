#! /usr/bin/env python

import vcsn
from test import *

# Not deterministic, yet not ambiguous.
a = vcsn.automaton('''
digraph
{
  vcsn_context="lal_char(ab)_b"
  I -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "a"]
  1 -> F
}
''')
CHECK_EQ(False, a.is_ambiguous())
XFAIL(lambda: a.ambiguous_word(), "automaton is unambiguous")
CHECK_EQ(False, a.is_deterministic())

# Not deterministic, and ambiguous.
a = vcsn.automaton('''
digraph
{
  vcsn_context="lal_char(ab)_b"
  I -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "a"]
  1 -> F
  2 -> F
}
''')
CHECK_EQ(True, a.is_ambiguous())
CHECK_EQ("a", a.ambiguous_word())
CHECK_EQ(False, a.is_deterministic())

# Likewise, but with a non-commutative product.
a = vcsn.automaton('''
digraph
{
  vcsn_context="lal_char(ab)_ratexpset<lal_char(xy)_b>"
  I -> 0
  0 -> 1 [label = "<x>a"]
  0 -> 2 [label = "<y>a"]
  1 -> F
  2 -> F
}
''')
CHECK_EQ(True, a.is_ambiguous())
CHECK_EQ("a", a.ambiguous_word())
CHECK_EQ(False, a.is_deterministic())

# deterministic, and unambiguous (obviously).
a = vcsn.automaton('''
digraph
{
  vcsn_context="lal_char(ab)_b"
  I -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> F
  2 -> F
}
''')
CHECK_EQ(False, a.is_ambiguous())
XFAIL(lambda: a.ambiguous_word(), "automaton is unambiguous")
CHECK_EQ(True, a.is_deterministic())
