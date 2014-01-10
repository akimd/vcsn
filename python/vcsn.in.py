from vcsn_python import *

vcsn_datadir = '@VCSN_DATADIR@'

def dot_to_svg(dot):
    open("/tmp/a.gv", "w").write(dot)
    from subprocess import check_output
    return check_output(['dot','-Tsvg','/tmp/a.gv'])

def is_equal(lhs, rhs):
    return isinstance(rhs, lhs.__class__) and str(lhs) == str(rhs)

## --------- ##
## context.  ##
## --------- ##

context.__eq__ = is_equal
context._repr_latex_ = lambda c: '$' + c.format('latex') + '$'

def make_polynomial(ctx, p):
    return polynomial(ctx, p)
context.polynomial = make_polynomial

def make_ratexp(ctx, re):
    return ratexp(ctx, re)
context.ratexp = make_ratexp


## ----------- ##
## automaton.  ##
## ----------- ##

automaton.__eq__ = is_equal
automaton.__mul__ = lambda lhs, rhs: lhs.product(rhs).trim()
automaton._repr_svg_ = lambda a: dot_to_svg(str(a))

def automaton_load(file):
    return automaton(open(file, "r").read(), "dot")
automaton.load = staticmethod(automaton_load)

def automaton_fst(aut, cmd):
    open("/tmp/in.efsm", "w").write(aut.format("efsm"))
    from subprocess import check_call, check_output
    check_call(['efstcompile',   '/tmp/in.efsm', '/tmp/in.fst'])
    check_call([cmd,             '/tmp/in.fst',  '/tmp/out.fst'])
    res = check_output(['efstdecompile', '/tmp/out.fst'])
    return automaton(res, "efsm")

def automaton_info(aut):
    '''A dictionary of automaton properties.'''
    res = dict()
    for l in aut.format('info').splitlines():
        (k, v) = l.split(':')
        v = v.strip()
        if k.startswith('is '):
            res[k] = bool(v)
        elif k.startswith('number '):
            res[k] = int(v)
        else:
            res[k] = v
    return res
automaton.info = automaton_info

automaton.fstdeterminize = lambda aut: automaton_fst(aut, "fstdeterminize")
automaton.fstminimize = lambda aut: automaton_fst(aut, "fstminimize")

## ------------ ##
## polynomial.  ##
## ------------ ##

polynomial.__eq__ = is_equal

## -------- ##
## ratexp.  ##
## -------- ##

ratexp.__eq__ = is_equal
ratexp._repr_latex_ = lambda r: '$' + r.format('latex') + '$'
