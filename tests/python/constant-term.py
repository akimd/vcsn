#! /usr/bin/env python

import inspect
import vcsn

count = 0
ctx = vcsn.context("lal_char(abc)_ratexpset<lal_char(xyz)_z>")

# check_equal WEIGHT WEIGHT
# -------------------------
# Helper function to check that two weights are equal.
def check_equal(ref, test):
    if ref.format('text') == test.format('text'):
        return True
    finfo = inspect.getframeinfo(inspect.currentframe().f_back)
    here = finfo.filename + ":" + str(finfo.lineno)
    msg = ref.format('text') + " != " + test.format('text')
    print here + ":", msg
    print 'not ok ', count, msg
    return False

# check WEIGHT RAT-EXP
# --------------------
# Check that the constant-term of RAT-EXP is WEIGHT, and check that
# this is indeed the evaluation of the empty word on derived-term(RAT-EXP).
#
# Use a context with ratexp weights to check the order of products.
def check(weight, exp):
    global count
    count += 1
    re = ctx.ratexp(exp)
    if (check_equal(weight, re.constant_term())
        and check_equal(weight, re.derived_term().eval(''))):
        print 'ok ', count




check('\z', 'a')
check('\e', 'a*')
check('x', '<x>a*')

# Test zero.
check('\z', '<x>\z')

# Test one.
check('x', '<x>\e')

# Test add.
check('x+y', '<x>a*+<y>b*')
check('x(y+y)z', '<x>(<y>a*+<y>b*)<z>')

# Test mul.
check('xy', '<x>a*<y>b*')

# Test intersection.
check('\z', '<x>a&<y>b')
check('xy', '<x>a*&<y>b*')

# Test star.
check('\e', '(<x>a)*')
check('x*', '(<x>\e)*')

# Test left and right weight.
check('xy*z', '<x>(<y>\e)*<z>')

print '1..'+str(count)
