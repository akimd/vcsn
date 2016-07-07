import sys, traceback, inspect

from vcsn_cxx import (automaton, context, expansion, expression, label,
                      polynomial, weight)
from vcsn.automaton  import automaton
from vcsn.context    import context
from vcsn.expansion  import expansion
from vcsn.expression import expression
from vcsn.label      import label
from vcsn.polynomial import polynomial
from vcsn.weight     import weight

# Load IPython specific support if we can.
try:
    # Load only if we are running IPython.
    __IPYTHON__
    from vcsn.ipython import *
    from vcsn.score import *
except (ImportError, NameError):
    pass

from vcsn_tools.config import config # pylint: disable=wrong-import-order
datadir = config['datadir']
version = config['version']

# Predefined contexts for casual users.
B = context('lal_char, b')
Z = context('lal_char, z')
Q = context('lal_char, q')
b = B

# Cause an error to get back algorithms list
try:
    B.expression(r'\e').automaton('')
except RuntimeError as e:
    expression.automaton.algos = e.args[0].split('expected: ')[1].split()
try:
    B.expression(r'\e', '')
except RuntimeError as e:
    expression.identities_list = e.args[0].split('expected: ')[1].split()

# Traceback handler
_traceback_verbose = False

def is_vcsn(tb):
    '''Does this traceback frame contain vcsn code?'''
    try:
        return 'vcsn' == inspect.getmodule(tb.tb_frame).__package__
    except AttributeError:
        return False

def vcsn_traceback_levels(tb):
    '''Returns the amount of frames deep into the traceback
    we have to go through before we reach vcsn.'''
    length = 0
    while tb and not is_vcsn(tb):
        tb = tb.tb_next
        length += 1
    return length

def vcsn_exception_handler(type, value, tb):
    '''Prints traceback according to vcsn._traceback_verbose.'''
    if _traceback_verbose:
        traceback.print_exception(type, value, tb)
    else:
        length = vcsn_traceback_levels(tb)
        traceback.print_exception(type, value, tb, length)

sys.excepthook = vcsn_exception_handler
