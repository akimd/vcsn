import hashlib
import inspect
import os
import sys
import traceback

import vcsn.python3

# By default, python will not use RTLD_GLOBAL flag in dlopen.
# vcsn_cxx symbols were not shared with other shared libraries.
# Thus, when using some cached properties on automata, we had their static
# methods' symbols not properly resolved, resulting in the existence of 2
# values for properties' indices: one in lal_(b|q|z) and one in shared
# libraries (loaded with the RTLD_GLOBAL flag, see lib/vcsn/dyn/translate.cc).
# See https://docs.python.org/3.5/library/sys.html#sys.setdlopenflags
# and dlopen(3).
oldflags = sys.getdlopenflags()
sys.setdlopenflags(os.RTLD_NOW | os.RTLD_GLOBAL)

# pylint: disable=wrong-import-position

from vcsn.automaton  import automaton
from vcsn.context    import context
from vcsn.expansion  import expansion
from vcsn.expression import expression
from vcsn.label      import label
from vcsn.polynomial import polynomial
from vcsn.weight     import weight

from vcsn.tools      import _tuple, _timeit as timeit

from vcsn_cxx import configuration as config # pylint: disable=wrong-import-order

datadir = config('configuration.datadir')
version = config('configuration.version')

sys.setdlopenflags(oldflags)

# Load IPython specific support if we can.
try:
    # Load only if we are running IPython.
    __IPYTHON__
    # pylint: disable=ungrouped-imports
    from vcsn.ipython import *

    ip = get_ipython()
    # If we are running in a notebook.
    if 'connection_file' in ip.config['IPKernelApp']:
        from vcsn.score import *
except (ImportError, NameError):
    pass

# Predefined contexts for casual users.
B = context('lal, b')
Z = context('lal, z')
Q = context('lal, q')
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


def md5(fname):
    'If fname is a file, return its MD5 sum, otherwise the empty string.'
    try:
        hash = hashlib.md5()
        with open(fname, 'rb') as f:
            for chunk in iter(lambda: f.read(4096), b''):
                hash.update(chunk)
        return hash.hexdigest()
    except OSError:
        return ''


def setenv(**vars):
    '''Set/unset environment variables for Vcsn.  Pass a list
    of assignments:

    vcsn.setenv(FOO=None, BAR=1, BAZ=0)

    is equivalent to:

    unsetenv('VCSN_FOO')
    setenv('VCSN_BAR', '1')
    setenv('VCSN_BAZ', '0')
    '''
    for k, v in vars.items():
        k = 'VCSN_' + k
        if v is None:
            if k in os.environ:
                del os.environ[k]
        else:
            os.environ[k] = str(v)

def tuple(*args):
    'Allow `vcsn.tuple(a, b, c)` rather than `a._tuple([b, c])`.'
    return _tuple(args)


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

try:
    __IPYTHON__
    # pylint: disable=wrong-import-order
    from IPython.core.ultratb import AutoFormattedTB
    ip = get_ipython()
    mode = ip.InteractiveTB.mode
    offset = ip.InteractiveTB.tb_offset
    class vcsnTB(AutoFormattedTB):
        # pylint: disable=arguments-differ
        def structured_traceback(self, type, value, tb, tb_offset=None,
                                 *args, **kwargs):
            # If an offset is given, use it, else use the default
            offset = tb_offset if tb_offset else self.tb_offset
            ftb = AutoFormattedTB.structured_traceback(self, type, value, tb,
                                                       offset, *args, **kwargs)
            if _traceback_verbose:
                return ftb
            else:
                length = vcsn_traceback_levels(tb)
                # Display the 2 header entries plus `length` entries
                # minus the entries that were offset, and the footer entry.
                return ftb[:2+length-offset] + ftb[-1:]

    ip.InteractiveTB = vcsnTB(mode=mode, tb_offset=offset)
except (ImportError, NameError):
    pass
