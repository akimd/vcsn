#! /usr/bin/env python

## ------------ ##
## aut-to-exp.  ##
## ------------ ##

import vcsn, sys, inspect, os

count = 0
ctx = vcsn.context("lal_char(abc)_b")

medir = sys.argv[0].replace(".py", ".dir")

# check INPUT OUTPUT
# ------------------
# Helper function to format output for the testsuite
# Check that text == ref
def check(text, ref):
    global count
    count += 1
    finfo = inspect.getframeinfo(inspect.currentframe().f_back)
    here = finfo.filename + ":" + str(finfo.lineno)
    if str(text) == ref:
        print "ok", count
    else:
        msg = text + " != " + ref
        print here + ":", msg
        print 'not ok ', count, msg

# check EXP-IN EXP-OUT
# --------------------
# Check that aut-to-exp(standard(EXP-IN)) = EXP-OUT.
def check_exp(exp_in, exp_out):
    check(ctx.ratexp(exp_in).standard().ratexp(), exp_out)

check_exp('a',                            'a')
check_exp('ab',                           'ab')
check_exp('a*',                           '\e+aa*')
check_exp('a+b',                          'a+b')
check_exp('(?@lal_char(ab)_z)<2>(ab)<3>', '(<2>ab)<3>')
check_exp('(?@law_char(ab)_z)<2>(ab)<3>', '<6>ab')


check_exp('(?@lal_char(abc)_z)<2>(<3>a+<5>b)<7>c<11>', \
      '(<6>a<7>c+<10>b<7>c)<11>')

# Likewise, but check the order of the products.
check_exp('(?@lal_char(abc)_ratexpset<lal_char(vwxyz)_b>)<v>(<w>a+<x>b)<y>c<z>', \
      '(<vw>a<y>c+<vx>b<y>c)<z>')

# check_aut LIB-AUTOMATON EXP-OUT
# ---------------------------
# Check that aut-to-exp(AUTOMATON) = EXP-OUT.
def check_aut(file, ref):
    check(vcsn.automaton.load(medir + "/" + file).ratexp(), ref)

# Check the associativity of the product.
check_aut("a.gv", '<x>a(<y>b)*<z>c')

# The following tests require files from the installed libraries, given
# automatons. Tafkit uses the environment variable VCSN_DATA_PATH to locate
# these files, but python does not know about it.
# A function to load automatons from the library could be convenient,
# but for now these tests are left for tafkit to do.

#check_aut("lal_char_b/a1.gv", '(a+b)*ab(a+b)*')
#check_aut("lal_char_b/b1.gv", '(a+b)*a(a+b)*')
#check_aut("lal_char_b/evena.gv", 'b*a(b+ab*a)*')
#check_aut("lal_char_b/oddb.gv", 'a*b(a+ba*b)*')

#check_aut("lal_char_z/b1.gv", '(a+b)*a(a+b)*')
#check_aut("lal_char_z/binary.gv", '(0+1)*1(<2>0+<2>1)*')
#check_aut("lal_char_z/c1.gv", '(a+b)*b(<2>a+<2>b)*')
#check_aut("lal_char_z/d1.gv", '(a+b)*(a+<-1>b)(a+b)*')

#check_aut("lal_char_zmin/minab.gv", '(<1>a+b)*+(a+<1>b)*')
#check_aut("lal_char_zmin/minblocka.gv", '(a+b)*b(<1>a)*b(a+b)*')
#check_aut("lal_char_zmin/slowgrow.gv",
#      '(a+b)*b(<1>a)*b(a+<1>a(<1>a)*b)*')


print '1..'+str(count)
