#! /usr/bin/env python

import vcsn
from test import *


## check_enumerate RE MAX EXP
## --------------------------
## Check that EXP accepts EXP has shortest words up to size MAX.
def check_enumerate(re, size, *exp):
  r = ctx.ratexp(re)
  p = r.standard().enumerate(size)
  # It would be nicer to build a polynomial, but currently we don't
  # have a means to build the right context, with LAW, not LAL etc.
  exp = " + ".join(exp)
  CHECK_EQ(exp, str(p))

## check_shortest RE NUM EXP
## --------------------------
## Check that EXP accepts EXP has shortest word, or throws if empty.
def check_shortest(re, num, *exp):
  r = ctx.ratexp(re)
  p = r.standard().shortest(num)
  exp = " + ".join(exp)
  CHECK_EQ(exp, str(p))


## check RE MAX WORD...
## ---------------------
## For the cases where enumerate and shortest give the same answers.
def check(re, size, *exp):
  check_enumerate(re, size, *exp)
  check_shortest(re, size, *exp)


ctx = vcsn.context('lal_char(ab), b')

check('\z', 3, '\z')
check('\e', 3, '\e')
check('(a+b)', 2, 'a + b')
check('ababab', 10, 'ababab')

check_enumerate('(a+b)*', 2, '\e + a + b + aa + ab + ba + bb')
check_shortest('(a+b)*', 2, '\e', 'a')

check_enumerate('ababab', 2, '\z')
check_shortest('ababab', 2, 'ababab')


ctx = vcsn.context('lal_char(01), z')
check_enumerate('(0+1)*1(<2>0+<2>1)*', 3, \
      '1',
      '01', '<2>10', '<3>11', \
      '001', '<2>010', '<3>011', '<4>100', '<5>101', '<6>110', '<7>111')

check_shortest('(0+1)*1(<2>0+<2>1)*', 3, '1 + 01 + <2>10')

ctx = vcsn.context('lal_char(a), z')
check('a+<-1>a', 2, '\z')
