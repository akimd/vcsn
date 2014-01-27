from vcsn_python import *

# Where the automaton library is stored.
vcsn_datadir = '@DATADIR@'

def dot_to_svg(dot):
    "Return the conversion of a Dot source into SVG."
    open("/tmp/a.gv", "w").write(dot)
    from subprocess import check_output
    return check_output(['dot','-Tsvg','/tmp/a.gv'])

# FIXME: Get rid of this.
def is_equal(lhs, rhs):
    "A stupid string-based comparison.  Must be eliminated one we DRT."
    return isinstance(rhs, lhs.__class__) and str(lhs) == str(rhs)

## --------- ##
## context.  ##
## --------- ##

context.__eq__ = is_equal
context.__repr__ = lambda self: self.format('text')
context._repr_latex_ = lambda self: '$' + self.format('latex') + '$'

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
automaton.__repr__ = lambda self: self.format('dot')
automaton.__sub__ = automaton.difference
automaton._repr_svg_ = lambda self: dot_to_svg(self.format('dot'))

def automaton_load(file, format = "dot"):
    return automaton(open(file, "r").read(), format)
automaton.load = staticmethod(automaton_load)

def automaton_fst(aut, cmd):
    open("/tmp/in.efsm", "w").write(aut.format("efsm"))
    from subprocess import check_call, check_output
    check_call(['efstcompile',   '/tmp/in.efsm', '/tmp/in.fst'])
    check_call([cmd,             '/tmp/in.fst',  '/tmp/out.fst'])
    res = check_output(['efstdecompile', '/tmp/out.fst'])
    return automaton(res, "efsm")

automaton.fstdeterminize = lambda self: automaton_fst(self, "fstdeterminize")
automaton.fstminimize = lambda self: automaton_fst(self, "fstminimize")

def automaton_info(aut):
    '''A dictionary of automaton properties.'''
    res = dict()
    for l in aut.format('info').splitlines():
        (k, v) = l.split(':')
        v = v.strip()
        if k.startswith('is '):
            res[k] = v in ['true', '1']
        elif k.startswith('number '):
            res[k] = int(v)
        else:
            res[k] = v
    return res
automaton.info = automaton_info

automaton.lan_to_lal = \
  lambda self: automaton(self.format('dot').replace('lan_', 'lal_'), "dot")
automaton.proper = lambda self, prune = True: self.proper_real(prune).lan_to_lal()


## ------------ ##
## polynomial.  ##
## ------------ ##

polynomial.__eq__ = is_equal
polynomial.__repr__ = lambda self: self.format('text')
polynomial._repr_latex_ = lambda self: '$' + self.format('latex') + '$'


## -------- ##
## ratexp.  ##
## -------- ##

ratexp.__add__ = ratexp.sum
ratexp.__and__ = ratexp.intersection
ratexp.__eq__ = is_equal
ratexp.__mul__ = ratexp.concatenate
ratexp.__repr__ = lambda self: self.format('text')
ratexp._repr_latex_ = lambda self: '$' + self.format('latex') + '$'

## ------------ ##
## polynomial.  ##
## ------------ ##

polynomial.__eq__ = is_equal
polynomial.__repr__ = lambda self: self.format('text')
polynomial._repr_latex_ = lambda self: '$' + self.format('latex') + '$'


## -------- ##
## weight.  ##
## -------- ##

weight.__eq__ = is_equal
weight.__repr__ = lambda self: self.format('text')
weight._repr_latex_ = lambda self: '$' + self.format('latex') + '$'
