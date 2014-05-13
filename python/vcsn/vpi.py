## =========================== ##
## Vaucanson Python Interface  ##
## =========================== ##

# Import globals from the vcsn module
from vcsn import *

## --------------------------------------- ##
## Automaton loading/saving functionality  ##
## --------------------------------------- ##

# automaton_load is already there.
def load_automaton(filename):
    return automaton_load(filename)

def save_automaton(automaton, filename):
    open(filename, 'w').write(automaton.format('dot'))


## ----------------------------------------------- ##
## Higher-level wrapper for the dyn edit function  ##
## ----------------------------------------------- ##

# A low-level wrapper with explicit opcode and redundant parameters,
# but with reasonably named arguments and default values.
automaton.lowlevel_edit = \
  lambda self, opcode, int1=-1, int2=-1, label='', weight='': \
    wrap_minus_one_into_none(self.trivial_lowlevel_edit(opcode, int1, int2, label, weight))
def wrap_minus_one_into_none(x):
    if x == -1:
        return None
    else:
        return x

automaton.add_state = \
  lambda self: self.lowlevel_edit(0) - 2
automaton.remove_state = \
  lambda self, state: self.lowlevel_edit(1, state + 2)
automaton.add_transition = \
  lambda self, from_state, to_state, label, weight='': \
    self.lowlevel_edit(2, from_state + 2, to_state + 2, label, str(weight))
automaton.set_transition = \
  lambda self, from_state, to_state, label, weight='': \
    self.lowlevel_edit(3, from_state + 2, to_state + 2, label, str(weight))
automaton.get_transition = \
  lambda self, from_state, to_state, label: \
    self.lowlevel_edit(4, from_state + 2, to_state + 2, label)
automaton.remove_transition = \
  lambda self, transition: \
    self.lowlevel_edit(5, transition)
automaton.set_initialness = \
  lambda self, state, initialness, weight='': \
    self.lowlevel_edit(6, state + 2, 1 if initialness else 0, weight=str(weight))
automaton.set_finalness = \
  lambda self, state, finalness, weight='': \
    self.lowlevel_edit(7, state + 2, 1 if finalness else 0, weight=str(weight))
automaton.get_states = \
  lambda self: \
    self.lowlevel_edit(8)
automaton.get_transitions = \
  lambda self: \
    self.lowlevel_edit(9)

automaton.set_initial = \
  lambda self, state, weight='': self.set_initialness(state, True, weight)
automaton.set_final = \
  lambda self, state, weight='': self.set_finalness(state, True, weight)
automaton.unset_initial = \
  lambda self, state: self.set_initialness(state, False)
automaton.unset_final = \
  lambda self, state: self.set_finalness(state, False)


## ---------------------------------------------------------- ##
## Alternative definitions for Jacques: meta-level machinery  ##
## ---------------------------------------------------------- ##

import sys

def clone(a):
    return automaton(a.format("dot"), "dot");

def alias_method_as_function(method_name):
    thismodule = sys.modules[__name__]
    setattr(thismodule,
            method_name,
            lambda self, *other_args:
                 self.__getattribute__(method_name)(*other_args))

# Given a unary method m and [o1, o2, .. on] objects, return
# o1.m(o2.m(... .m(on) ...))
def variadically_call_method(method_name, *selves):
    if (len(selves) == 0):
        raise Exception("zero operands")
    else:
        res = selves[0]
        for i in range(1, len(selves)):
            res = res.__getattribute__(method_name)(selves[i])
        return res

def alias_method_as_variadic_function(method_name):
    thismodule = sys.modules[__name__]
    setattr(thismodule,
            method_name,
            lambda *args:
                 variadically_call_method(method_name, *args))


## ----------------- ##
## Weight conversion ##
## ----------------- ##

def weight(context, string):
    return context.weight(string)


## ------------------------- ##
## Method-wrapping functions ##
## ------------------------- ##

# Don't do this on ratexp: the functional definition is already there and we don't want to break it
# Don't do this on transpose: we explicitly define it below, to work around a problem elsewhere
# Don't do this for minimize: we want to hide algorithm choice
# Yes, I did redefine eval.  Python doesn't seem to mind.
for name in ["accessible", "coaccessible", "complement", "complete", "concatenate", "determinize", "difference", "enumerate", "eval", "format", "is_accessible", "is_ambiguous", "is_coaccessible", "is_complete", "is_deterministic", "is_eps_acyclic", "is_equivalent", "is_proper", "is_standard", "is_trim", "is_useless", "is_valid", "proper", "shortest", "standard", "star", "trim", "union", "universal", "constant_term", "copy", "derivation", "derived_term", "expand", "is_equivalent", "is_valid", "split", "sort", "star_normal_form", "thompson", "transpose", "de_bruijn", "ladybird", "lan_to_lal"]:
    alias_method_as_function(name)

for name in ["infiltration", "shuffle", "sum", "union"]:
    alias_method_as_variadic_function(name)

# FIXME: didn't we use to have a nice method for product, other than
# the infix operator?  With that I could use
# alias_method_as_variadic_function and then define a trivial alias,
# insted of iterating here.
def product(*x):
    if len(x) == 0:
        raise Exception("zero operands")
    else:
        res = x[0]
        for i in range(1, len(x)):
            res = res & x[i]
        return res

def power(x, n):
    return x ** n

# Star should not be in place:
def star(a):
    return clone(a).star()

def aut_to_exp(a):
    return a.ratexp()

def are_isomorphic(a, b):
    return a.is_isomorphic(b)

def is_lan(a):
    n = automaton_to_context_name(a)
    return re.search('^lan<', n) != None

def is_lal(a):
    n = automaton_to_context_name(a)
    return re.search('^lal_char', n) != None

def lan_to_lal(a):
    if is_lan(a):
        return a.lan_to_lal()
    else:
        raise Exception("argument not lan")

def lal_to_lan(a):
    if is_lal(a):
        return automaton(a.format('dot').replace('\"lal_', '\"lan_'), "dot")
    else:
        raise Exception("argument not lal")

def make_automaton(ctx):
    a = ctx.ratexp('\z').standard()
    a.remove_state(0)
    return clone(a)

def automaton_to_context_name(a):
    import re
    s = a.format('dot')
    r = re.search('vcsn_context = \"[^\"]*\"', s).span()
    line = s[r[0]:r[1]]
    r = re.search('\"[^\"]*\"', line).span()
    return line[r[0]:r[1]][1:-1]

def automaton_to_context(a):
    return context(automaton_to_context_name(a))


# Wrapper to be able to *also* pass weights as strings.
def left_mult(a, w):
    if type(w) == str:
        return a.left_mult(a.context().weight(w))
    else:
        return a.left_mult(w)
def right_mult(a, w):
    if type(w) == str:
        return a.right_mult(a.context().weight(w))
    else:
        return a.right_mult(w)


## ------- ##
## Aliases ##
## ------- ##

def quotient(a):
    return a.minimize("weighted")

def minimize(a):
    return quotient(a)


## ----------------------------------------------------- ##
## Simpler info, "censored" so as not to scare beginners ##
## ----------------------------------------------------- ##

def info(x):
    if type(x) == ratexp:
        return x.info()
    else:
        scary = x.info()
        keys = ['number of states', 'number of initial states',
                'number of final states', 'number of transitions']
        return dict(zip(keys, [scary[k] for k in keys]))


## ----------------------------- ##
## Simple pure-Python algorithms ##
## ----------------------------- ##

def coquotient(a):
  return transpose(quotient(transpose(a)))

def codeterminize(a):
  return transpose(determinize(transpose(a)))


## -------------------------------------------- ##
## Workaround for current transpose limitations ##
## -------------------------------------------- ##

def transpose(a):
  return clone(a.transpose())


## -------------------- ##
## SVG-printing utility ##
## -------------------- ##

def display_automaton(a):
    svg_data = dot_to_svg(a.format('dot'))
    from IPython.display import display, SVG
    display(SVG(data = svg_data))


## ------------------------------ ##
## Alternative context definition ##
## ------------------------------ ##

# A few functions to define contexts without exposing beginners to
# kinds.
def context_name_2_or_3(alphabet, weightset, kind='lal'):
    kind = normalize_kind(kind)
    return kind + '_char(' + alphabet + ')_' + weightset
def normalize_kind(kind):
    if kind == 'letter' or kind == 'letters' or kind == 'l':
        return 'lal'
    elif kind == 'lsp' or kind == 'lalsp':
        return 'lan'
    elif kind == 'word' or kind == 'words' or kind == 'w':
        return 'law'
    else:
        return kind
def context_name(*arguments):
    argno = len(arguments)
    if argno < 1:
        raise Exception("too few parameters")
    elif type(arguments[0]) == list:
        if argno == 2:
            return transducer_context_name(*arguments)
        else:
            raise Exception("transducer syntax requires two arguments")
    elif argno > 3:
        raise Exception("too many parameters")
    elif argno == 1:
        return arguments[0]
    else:
        return context_name_2_or_3(*arguments)
def transducer_context_name(alphabets, weightset):
    res = 'lat<'
    initial = True
    for a in alphabets:
        if not initial:
            res += ', '
        initial = False
        res += 'lan_char(' + a +  ')'
    res += '>_' + weightset
    return res

original_context = context
def context(*args):
    if len(args) == 1:
        name = args[0]
    else:
        name = context_name(*args)
    return original_context(name)


## ----------------------- ##
## Modal edition interface ##
## ----------------------- ##

# Jacques doesn't even want a stack of automata being edited: once you
# exit editing mode, you always go back to non-editing mode.
the_invalid_edited_automaton = 0
the_edited_automaton = the_invalid_edited_automaton

modal_interface_verbose = True

def in_edit_mode():
  return the_edited_automaton != the_invalid_edited_automaton

def edit(a):
  if in_edit_mode():
    #raise Exception('already in edit mode')
    print('Warning: you were already in edit mode.\n')
  global the_edited_automaton
  the_edited_automaton = a

def exit_edit():
  if not in_edit_mode():
    #raise Exception('already out of edit mode')
    print('Warning: you were already out of edit mode.\n')
  global the_edited_automaton
  the_edited_automaton = the_invalid_edited_automaton

def ensure_edit_mode():
  if not in_edit_mode():
    raise Exception('you must be in edit mode')

def print_automaton_if_verbose():
  ensure_edit_mode()
  if modal_interface_verbose:
    display_automaton(the_edited_automaton)

def set_modal_interface_verbosity(new_value):
  global modal_interface_verbose
  modal_interface_verbose = new_value

def add_state():
  ensure_edit_mode()
  res = the_edited_automaton.add_state()
  print_automaton_if_verbose()
  return res

def remove_state(s):
  ensure_edit_mode()
  the_edited_automaton.remove_state(s)
  print_automaton_if_verbose()

def add_transition(s1, s2, label, weight=''):
  ensure_edit_mode()
  # Here a return statement would be useful in programs, but absolute
  # beginners might find it confusing in interactive mode.  It's also
  # not terribly useful from Python.
  the_edited_automaton.set_transition(s1, s2, label, weight)
  print_automaton_if_verbose()

def get_transition(s1, s2, label):
  ensure_edit_mode()
  return the_edited_automaton.get_transition(s1, s2, label)

def remove_transition(s1, s2, label):
  ensure_edit_mode()
  t = get_transition(s1, s2, label)
  the_edited_automaton.remove_transition(t)
  print_automaton_if_verbose()

def set_initial(s, weight=''):
  ensure_edit_mode()
  the_edited_automaton.set_initial(s, weight)
  print_automaton_if_verbose()
def unset_initial(s):
  ensure_edit_mode()
  the_edited_automaton.unset_initial(s)
  print_automaton_if_verbose()
def set_final(s, weight=''):
  ensure_edit_mode()
  the_edited_automaton.set_final(s, weight)
  print_automaton_if_verbose()
def unset_final(s, weight=''):
  ensure_edit_mode()
  the_edited_automaton.unset_final(s)
  print_automaton_if_verbose()

def show():
  ensure_edit_mode()
  display_automaton(the_edited_automaton)


## ----- ##
## Help  ##
## ----- ##

def list_functions():
  import vcsn
  help(vcsn)

# Work around a temporary glitch:
automaton.__and__ = lambda l, r: clone(conjunction(l, r))
