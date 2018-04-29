#! /usr/bin/env python

import vcsn
from test import *
from re import split


## check_enumerate RE MAX EXP
## --------------------------
## Check that RE accepts EXP has shortest words up to size MAX.
def check_enumerate(re, len, *exp):
  # It would be nicer to build a polynomial, but currently we don't
  # have a means to build the right context, with LAW, not LAL etc.
  exp = " ⊕ ".join(exp)
  re = ctx.expression(re)
  CHECK_EQ(exp, re.standard().shortest(len=len))
  CHECK_EQ(exp, re.shortest(len=len))

def check_one(aut, num, exp):
  CHECK_EQ(exp, aut.shortest(num=num))

## check_shortest RE NUM EXP
## --------------------------
## Check that RE accepts EXP has shortest word, or throws if empty.
def check_shortest(re, num, *exp):
  exp = " ⊕ ".join(exp)
  if num != 1:
      check_shortest(re, 1, split('[ +]+', exp)[0])
  re = ctx.expression(re)
  check_one(re.standard(), num, exp)
  check_one(re, num, exp)


## check RE MAX WORD...
## ---------------------
## For the cases where enumerate and shortest give the same answers.
def check(re, size, *exp):
  check_enumerate(re, size, *exp)
  check_shortest(re, size, *exp)


ctx = vcsn.context('[ab] -> b')

check(r'\z', 3, r'∅')
check(r'\e', 3, r'ε')
check('a+b', 2, 'a ⊕ b')
check('ababab', 10, 'ababab')

check_enumerate('(a+b)*', 2, r'ε ⊕ a ⊕ b ⊕ aa ⊕ ab ⊕ ba ⊕ bb')
check_shortest('(a+b)*', 2, r'ε', 'a')

check_enumerate('ababab', 2, r'∅')
check_shortest('ababab', 2, 'ababab')


ctx = vcsn.context('[01] -> q')
check_enumerate('(0+1)*1(<2>0+<2>1)*', 3, \
      '1',
      '01', '⟨2⟩10', '⟨3⟩11', \
      '001', '⟨2⟩010', '⟨3⟩011', '⟨4⟩100', '⟨5⟩101', '⟨6⟩110', '⟨7⟩111')

check_shortest('(0+1)*1(<2>0+<2>1)*', 3, '1 ⊕ 01 ⊕ ⟨2⟩10')

ctx = vcsn.context('[a] -> q')
check('a+<-1>a', 2, r'∅')

# Wordset.
ctx = vcsn.context('[a]* -> q')
check_shortest('(a+aaa)*', 7, r'ε ⊕ a ⊕ aa ⊕ ⟨2⟩aaa ⊕ ⟨3⟩aaaa ⊕ ⟨4⟩aaaaa ⊕ ⟨6⟩aaaaaa')

# LAN x LAN
ctx = vcsn.context('[a] x [x] -> q')
check_shortest(r'(\e|x + a|\e)*', 9,
               r'ε|ε ⊕ ε|x ⊕ a|ε ⊕ ⟨2⟩a|x ⊕ ε|xx ⊕ ⟨3⟩a|xx ⊕ aa|ε ⊕ ⟨3⟩aa|x ⊕ ⟨6⟩aa|xx')
check_enumerate(r'(\e|x + a|\e)*', 2,
                r'ε|ε ⊕ ε|x ⊕ a|ε ⊕ ⟨2⟩a|x ⊕ ε|xx ⊕ ⟨3⟩a|xx ⊕ aa|ε ⊕ ⟨3⟩aa|x ⊕ ⟨6⟩aa|xx')

aut = vcsn.automaton('''
context = [a]? → ℚ
$ -> 0
0 -> 1 <2>a
0 -> 2 <-1>a
0 -> 3 <-1>a
1 -> $
2 -> $
3 -> $
''')
check_one(aut, 1, '∅')

aut = vcsn.automaton('''
context = [ab]? → ℚ
$ -> 0
0 -> 1 <2>a
0 -> 2 <-1>a
0 -> 3 <-1>a
0 -> 4 b
1 -> $
2 -> $
3 -> $
4 -> $
''')
check_one(aut, 1, 'b')
