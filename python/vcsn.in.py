from vcsn_python import *

# Where the automaton library is stored.
datadir = '@DATADIR@'
version = '@PACKAGE_VERSION@'

def dot_to_svg(dot):
    "Return the conversion of a Dot source into SVG."
    open("/tmp/a.gv", "w").write(dot)
    from subprocess import check_output
    return check_output(['dot','-Tsvg','/tmp/a.gv'])

# FIXME: Get rid of this.
def is_equal(lhs, rhs):
    "A stupid string-based comparison.  Must be eliminated once we DRT."
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
automaton.__add__ = automaton.sum
automaton.__and__ = automaton.product
automaton.__invert__ = automaton.complement
automaton.__mul__ = automaton.concatenate
automaton.__mod__ = automaton.difference
automaton.__or__ = automaton.union
automaton.__pow__ = automaton.power
automaton.__repr__ = lambda self: self.info()['type']
automaton.__str__ = lambda self: self.format('dot')
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
        # Beware that we may display "N/A" for Boolean (e.g., "is
        # ambiguous" for non LAL), and for integers (e.g., "number of
        # deterministic states" for non LAL).
        try:
            if k.startswith('is '):
                if v in ['false', '0']:
                    v = False
                elif v in ['true', '1']:
                    v = True
            elif k.startswith('number '):
                v = int(v)
        except:
            pass
        res[k] = v
    return res
automaton.info = automaton_info

automaton.lan_to_lal = \
  lambda self: automaton(self.format('dot').replace('lan_', 'lal_'), "dot")
# Somewhat cheating: in Python, proper returns a LAL, not a LAN.
# proper_real is the genuine binding to dyn::proper.
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
ratexp.__mod__ = ratexp.difference
ratexp.__mul__ = ratexp.concatenate
ratexp.__repr__ = lambda self: self.format('text')
ratexp.__str__ = lambda self: self.format('text')
ratexp._repr_latex_ = lambda self: '$' + self.format('latex') + '$'


## -------- ##
## weight.  ##
## -------- ##

weight.__eq__ = is_equal
weight.__repr__ = lambda self: self.format('text')
weight._repr_latex_ = lambda self: '$' + self.format('latex') + '$'
