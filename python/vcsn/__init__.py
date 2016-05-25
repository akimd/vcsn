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

from vcsn_config import config
datadir = config['datadir']
version = config['version']

# Predefined contexts for casual users.
B = context('lal_char, b')
Z = context('lal_char, z')
Q = context('lal_char, q')
b = B
