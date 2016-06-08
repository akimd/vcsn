#! /usr/bin/env python

import vcsn
from test import *
from re import split


## check_enumerate RE MAX EXP
## --------------------------
## Check that EXP accepts EXP has shortest words up to size MAX.
def check_enumerate(re, len, *exp):
  # It would be nicer to build a polynomial, but currently we don't
  # have a means to build the right context, with LAW, not LAL etc.
  exp = " + ".join(exp)
  re = ctx.expression(re)
  CHECK_EQ(exp, re.standard().shortest(len = len))
  CHECK_EQ(exp, re.shortest(len = len))

## check_shortest RE NUM EXP
## --------------------------
## Check that EXP accepts EXP has shortest word, or throws if empty.
def check_shortest(re, num, *exp):
  exp = " + ".join(exp)
  if num != 1:
      check_shortest(re, 1, split('[ +]+', exp)[0])
  re = ctx.expression(re)
  CHECK_EQ(exp, re.standard().shortest(num = num))
  CHECK_EQ(exp, re.shortest(num = num))


## check RE MAX WORD...
## ---------------------
## For the cases where enumerate and shortest give the same answers.
def check(re, size, *exp):
  check_enumerate(re, size, *exp)
  check_shortest(re, size, *exp)


ctx = vcsn.context('lal_char(ab), b')

check(r'\z', 3, r'\z')
check(r'\e', 3, r'\e')
check('a+b', 2, 'a + b')
check('ababab', 10, 'ababab')

check_enumerate('(a+b)*', 2, r'\e + a + b + aa + ab + ba + bb')
check_shortest('(a+b)*', 2, r'\e', 'a')

check_enumerate('ababab', 2, r'\z')
check_shortest('ababab', 2, 'ababab')


ctx = vcsn.context('lal_char(01), q')
check_enumerate('(0+1)*1(<2>0+<2>1)*', 3, \
      '1',
      '01', '<2>10', '<3>11', \
      '001', '<2>010', '<3>011', '<4>100', '<5>101', '<6>110', '<7>111')

check_shortest('(0+1)*1(<2>0+<2>1)*', 3, '1 + 01 + <2>10')

ctx = vcsn.context('lal_char(a), q')
check('a+<-1>a', 2, r'\z')

# Wordset.
ctx = vcsn.context('law_char(a), q')
check_shortest('(a+aaa)*', 7, r'\e + a + aa + <2>aaa + <3>aaaa + <4>aaaaa + <6>aaaaaa')

# LAN x LAN
ctx = vcsn.context('lat<lan_char(a), lan_char(x)>, q')
check_shortest(r'(\e|x + a|\e)*', 9,
               r'\e|\e + \e|x + a|\e + <2>a|x + \e|xx + <3>a|xx + aa|\e + <3>aa|x + <6>aa|xx')
check_enumerate(r'(\e|x + a|\e)*', 2,
                r'\e|\e + \e|x + a|\e + <2>a|x + \e|xx + <3>a|xx + aa|\e + <3>aa|x + <6>aa|xx')
