import functools
import os
import sys

from subprocess import Popen, check_call

from vcsn_cxx import *
from vcsn_config import *
datadir = config['datadir']
version = config['version']

from vcsn.automaton  import *
from vcsn.context    import *
from vcsn.expansion  import *
from vcsn.expression import *
from vcsn.label      import *
from vcsn.polynomial import *
from vcsn.weight     import *

# Load IPython specific support if we can.
try:
    # Load only if we are running IPython.
    __IPYTHON__
    from vcsn.ipython import *
    from vcsn.score import *
except (ImportError, NameError):
    pass

# Predefined contexts for casual users.
B = context('lal_char, b')
Z = context('lal_char, z')
Q = context('lal_char, q')
b = B
