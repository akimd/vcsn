from vcsn_python import *

def dot_to_svg(dot):
    open("/tmp/a.gv", "w").write(dot)
    from subprocess import check_call
    check_call(['dot','-Tsvg','/tmp/a.gv','-o','/tmp/a.svg'])
    return open("/tmp/a.svg", "r").read()

## --------- ##
## context.  ##
## --------- ##

def make_ratexp(ctx, re):
    return ratexp(ctx, re)
context.ratexp = make_ratexp
context._repr_latex_ = lambda c: '$' + c.format('latex') + '$'

## ----------- ##
## automaton.  ##
## ----------- ##

automaton._repr_svg_ = lambda a: dot_to_svg(str(a))
automaton.__mul__ = lambda lhs, rhs: lhs.product(rhs).trim()

def automaton_fst(aut, cmd):
    open("/tmp/in.efsm", "w").write(aut.format("efsm"))
    from subprocess import check_call, check_output
    check_call(['efstcompile',   '/tmp/in.efsm', '/tmp/in.fst'])
    check_call([cmd,             '/tmp/in.fst',  '/tmp/out.fst'])
    res = check_output(['efstdecompile', '/tmp/out.fst'])
    return automaton(res, "efsm")

automaton.fstdeterminize = lambda aut: automaton_fst(aut, "fstdeterminize")
automaton.fstminimize = lambda aut: automaton_fst(aut, "fstminimize")

## -------- ##
## ratexp.  ##
## -------- ##

def vcsn_ratexp__eq__(lhs, rhs):
    print isinstance(rhs, lhs.__class__)
    print str(lhs) == str(rhs)
    return isinstance(rhs, lhs.__class__) and str(lhs) == str(rhs)

ratexp.__eq__ = vcsn_ratexp__eq__


## ------------ ##
## polynomial.  ##
## ------------ ##
def vcsn_polynomial__eq__(lhs, rhs):
    print isinstance(rhs, lhs.__class__)
    print str(lhs) == str(rhs)
    return isinstance(rhs, lhs.__class__) and str(lhs) == str(rhs)

polynomial.__eq__ = vcsn_polynomial__eq__
