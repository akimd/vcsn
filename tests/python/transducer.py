#! /usr/bin/env python

import vcsn
from test import *

## ---------------- ##
## LAN x LAN -> Q.  ##
## ---------------- ##
c = vcsn.context("[abc] x [xyz] -> q")

# Make sure we can copy/convert a k-tape expression.
e = c.expression('a|x')
CHECK_EQ(e, e.expression())

# Check that a|x is understood as a single two-tape label, not as a
# two-tuple of single-tape labels.  Since standard does not support
# the tuple operator, that suffices.
e = c.expression('a|x')
CHECK_EQ(e.standard(), e.derived_term().strip())

# Likewise with a|\e.
e = c.expression(r'a|\e')
CHECK_EQ(e.standard(), e.derived_term().strip())

e = c.expression('[ab]|x')
CHECK_EQ('(a+b)|x', e)
CHECK_EQ(r'''a|x⊙[ε] ⊕ b|x⊙[ε]''', e.expansion())
CHECK_EQ(r'''digraph
{
  vcsn_context = "[abc]? × [xyz]? → ℚ"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F1
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "(a+b)|x", shape = box]
    1 [label = "\\e", shape = box]
  }
  I0 -> 0
  0 -> 1 [label = "a|x, b|x"]
  1 -> F1
}''',
         e.derived_term())

## ---------------------- ##
## LAN x LAN x LAN -> Q.  ##
## ---------------------- ##

# Using tuple.
exp = lambda e: vcsn.context("[...] -> Q").expression(e)
f = vcsn.tuple(exp('(<2>a)*'), exp('(<3>b)*'), exp('(<5>c)*'))
CHECK_EQ('''(⟨2⟩a)*|(⟨3⟩b)*|(⟨5⟩c)*''', f)

# Using the operator |.
c = vcsn.context("[a] x [b] x [c] -> q")
e = c.expression('(⟨2⟩a)*|(⟨3⟩b)*|(⟨5⟩c)*')
CHECK_EQ(f, e)
CHECK_EQ('''(⟨2⟩a)*|(⟨3⟩b)*|(⟨5⟩c)*''', e)
CHECK_EQ(r'''⟨1⟩ ⊕ ε|ε|c⊙[⟨5⟩ε|ε|(⟨5⟩c)*] ⊕ ε|b|ε⊙[⟨3⟩ε|(⟨3⟩b)*|ε] ⊕ ε|b|c⊙[⟨15⟩ε|(⟨3⟩b)*|(⟨5⟩c)*] ⊕ a|ε|ε⊙[⟨2⟩(⟨2⟩a)*|ε|ε] ⊕ a|ε|c⊙[⟨10⟩(⟨2⟩a)*|ε|(⟨5⟩c)*] ⊕ a|b|ε⊙[⟨6⟩(⟨2⟩a)*|(⟨3⟩b)*|ε] ⊕ a|b|c⊙[⟨30⟩(⟨2⟩a)*|(⟨3⟩b)*|(⟨5⟩c)*]''', e.expansion())
CHECK_EQ(r'''\left\langle 1\right\rangle  \oplus \varepsilon|\varepsilon|c \odot \left[\left\langle 5\right\rangle  \left. \varepsilon \middle| \varepsilon \middle| \left( \left\langle 5 \right\rangle \,c\right)^{*} \right. \right] \oplus \varepsilon|b|\varepsilon \odot \left[\left\langle 3\right\rangle  \left. \varepsilon \middle| \left( \left\langle 3 \right\rangle \,b\right)^{*} \middle| \varepsilon \right. \right] \oplus \varepsilon|b|c \odot \left[\left\langle 15\right\rangle  \left. \varepsilon \middle| \left( \left\langle 3 \right\rangle \,b\right)^{*} \middle| \left( \left\langle 5 \right\rangle \,c\right)^{*} \right. \right] \oplus a|\varepsilon|\varepsilon \odot \left[\left\langle 2\right\rangle  \left. \left( \left\langle 2 \right\rangle \,a\right)^{*} \middle| \varepsilon \middle| \varepsilon \right. \right] \oplus a|\varepsilon|c \odot \left[\left\langle 10\right\rangle  \left. \left( \left\langle 2 \right\rangle \,a\right)^{*} \middle| \varepsilon \middle| \left( \left\langle 5 \right\rangle \,c\right)^{*} \right. \right] \oplus a|b|\varepsilon \odot \left[\left\langle 6\right\rangle  \left. \left( \left\langle 2 \right\rangle \,a\right)^{*} \middle| \left( \left\langle 3 \right\rangle \,b\right)^{*} \middle| \varepsilon \right. \right] \oplus a|b|c \odot \left[\left\langle 30\right\rangle  \left. \left( \left\langle 2 \right\rangle \,a\right)^{*} \middle| \left( \left\langle 3 \right\rangle \,b\right)^{*} \middle| \left( \left\langle 5 \right\rangle \,c\right)^{*} \right. \right]''', e.expansion().format('latex'))
CHECK_EQ(r'''digraph
{
  vcsn_context = "[a]? × [b]? × [c]? → ℚ"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
    F1
    F2
    F3
    F4
    F5
    F6
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "(<2>a)*|(<3>b)*|(<5>c)*", shape = box]
    1 [label = "\\e|\\e|(<5>c)*", shape = box]
    2 [label = "\\e|(<3>b)*|\\e", shape = box]
    3 [label = "\\e|(<3>b)*|(<5>c)*", shape = box]
    4 [label = "(<2>a)*|\\e|\\e", shape = box]
    5 [label = "(<2>a)*|\\e|(<5>c)*", shape = box]
    6 [label = "(<2>a)*|(<3>b)*|\\e", shape = box]
  }
  I0 -> 0
  0 -> F0
  0 -> 0 [label = "<30>a|b|c"]
  0 -> 1 [label = "<5>\\e|\\e|c"]
  0 -> 2 [label = "<3>\\e|b|\\e"]
  0 -> 3 [label = "<15>\\e|b|c"]
  0 -> 4 [label = "<2>a|\\e|\\e"]
  0 -> 5 [label = "<10>a|\\e|c"]
  0 -> 6 [label = "<6>a|b|\\e"]
  1 -> F1
  1 -> 1 [label = "<5>\\e|\\e|c"]
  2 -> F2
  2 -> 2 [label = "<3>\\e|b|\\e"]
  3 -> F3
  3 -> 1 [label = "<5>\\e|\\e|c"]
  3 -> 2 [label = "<3>\\e|b|\\e"]
  3 -> 3 [label = "<15>\\e|b|c"]
  4 -> F4
  4 -> 4 [label = "<2>a|\\e|\\e"]
  5 -> F5
  5 -> 1 [label = "<5>\\e|\\e|c"]
  5 -> 4 [label = "<2>a|\\e|\\e"]
  5 -> 5 [label = "<10>a|\\e|c"]
  6 -> F6
  6 -> 2 [label = "<3>\\e|b|\\e"]
  6 -> 4 [label = "<2>a|\\e|\\e"]
  6 -> 6 [label = "<6>a|b|\\e"]
}''',
         e.derived_term())
