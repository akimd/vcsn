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


## ----------- ##
## automaton.  ##
## ----------- ##

def automaton_to_svg(aut):
    return dot_to_svg(str(aut))

automaton._repr_svg_ = automaton_to_svg

automaton.__mul__ = lambda lhs, rhs: lhs.product(rhs).trim()


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
