#! /usr/bin/env python

import vcsn
from test import *

## ------------------ ##
## Invalid automata.  ##
## ------------------ ##

# Check invalid input.
def xfail(a):
    XFAIL(lambda: vcsn.automaton(a))

# Invalid context: missing parens.
xfail(r'''digraph
{
  vcsn_context = "lal_char_b"
  I0 -> 0 -> F0
}
''')

# Invalid label: letter not in alphabet.
xfail(r'''digraph
{
  vcsn_context = "lal_char()_b"
  0 -> 1 [label = a]
  1 -> F1
  I0 -> 0
}
''')

# Invalid label: \e is not valid in LAL.
xfail(r'''digraph
{
  vcsn_context = "lal_char(a)_b"
  0 -> 1 [label = "\\e"]
  1 -> F1
  I0 -> 0
}
''')

# Invalid label: aa is not valid in LAL.
xfail(r'''digraph
{
  vcsn_context = "lal_char(a)_b"
  0 -> 1 [label = "aa"]
  1 -> F1
  I0 -> 0
}
''')

# Invalid label: missing '>'.
xfail(r'''digraph
{
  vcsn_context = "lal_char(a)_z"
  0 -> 1 [label = "<2"]
  1 -> F1
  I0 -> 0
}
''')

# No context defined (see the typo in vcsn_context).
xfail(r'''digraph
{
  vcsn_contxt = "lal_char(ab)_b"
  0 -> 1 [label = a]
  1 -> F1
  I0 -> 0
}
''')

# Invalid context.
xfail(r'''digraph
{
  vcsn_context = "unknown"
  0 -> 1 [label = a]
  1 -> F1
  I0 -> 0
}
''')

# Invalid initial label.
xfail(r'''digraph
{
  vcsn_context = "lal_char(ab)_b"
  0 -> 1 [label = a]
  1 -> F1
  I0 -> 0 [label = a]
}
''')

# Invalid final label.
xfail(r'''digraph
{
  vcsn_context = "lal_char(ab)_b"
  0 -> 1 [label = a]
  1 -> F1 [label = a]
  I0 -> 0
}
''')


## --------------------------- ##
## Conversions: dot and TikZ.  ##
## --------------------------- ##

import glob
for fn in glob.glob(os.path.join(medir, '*.in.gv')):
    a = vcsn.automaton.load(fn)
    gv   = open(fn.replace('.in.gv', '.out.gv')).read().strip()
    tikz = open(fn.replace('.in.gv', '.tex')).read().strip()
    CHECK_EQ(gv,   a.format('dot'))
    CHECK_EQ(tikz, a.format('tikz'))
