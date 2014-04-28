#! /usr/bin/env python

import vcsn
from test import *

## ------------------ ##
## Invalid automata.  ##
## ------------------ ##

# Check invalid input.
def xfail(a):
    XFAIL(lambda: vcsn.automaton(a))

# Syntax error: missing }.
xfail(r'''digraph
{
  vcsn_context = "lal_char(a)_b"
''')

# Syntax error: label not closed.
xfail(r'''digraph
{
  vcsn_context = "lal_char(a)_b
}
''')

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

# Coverage: different rarely used features.
CHECK_EQ(vcsn.automaton(r'''digraph
{
  vcsn_context = "lal_char()_b"
}'''),
vcsn.automaton(r'''digraph "a graph
                            name"
{
  vcsn_context // single line comment
  =
  /* a
     multiline
     comment.  */
  "lal_char()_b"
  graph [label = "graph attribute"]
  edge [label = "edge attribute"]
  node [label = "node attribute"]
}'''))

## --------------------------- ##
## Conversions: dot and TikZ.  ##
## --------------------------- ##

try:
    import FAdo
    has_fado = True
except ImportError:
    has_fado = False

import glob
for fn in glob.glob(os.path.join(medir, '*.in.gv')):
    a = vcsn.automaton.load(fn)

    gv   = open(fn.replace('.in.gv', '.out.gv')).read().strip()
    CHECK_EQ(gv, a.format('dot'))

    tikz = open(fn.replace('.in.gv', '.tex')).read().strip()
    CHECK_EQ(tikz, a.format('tikz'))


## ------------------- ##
## Conversions: FAdo.  ##
## ------------------- ##

try:
    import FAdo
    has_fado = True
except ImportError:
    has_fado = False

def check_fado(aut):
    '''Check that FAdo accepts aut.format('fado') as input.'''
    if has_fado:
        name = "automaton.fado"
        from FAdo import fa
        # I did not find a means to read from a string...
        with open(name, 'w') as f:
            f.write(aut.format('fado') + "\n")
        fa.readFromFile(name)
        os.remove(name)
    else:
        SKIP("FAdo not installed")

for fn in glob.glob(os.path.join(medir, '*.fado')):
    a = vcsn.automaton.load(fn, 'fado')
    exp = vcsn.automaton.load(fn.replace('.fado', '.gv'))
    # Check that we can read FAdo.
    CHECK_EQ(exp, a)

    # Check that we can print FAdo.
    fado   = open(fn).read().strip()
    CHECK_EQ(fado, a.format('fado'))
    check_fado(a)

## -------------------- ##
## Conversions: Grail.  ##
## -------------------- ##

def check_grail(aut):
    '''Check that FAdo accepts aut.format('grail') as input.'''
    if has_fado:
        name = "automaton.grail"
        from FAdo import grail
        # I did not find a means to read from a string...
        with open(name, 'w') as f:
            f.write(aut.format('grail') + "\n")
        grail.importFromGrailFile(name)
        os.remove(name)
    else:
        SKIP("FAdo not installed")

for fn in glob.glob(os.path.join(medir, '*.grail')):
    a = vcsn.automaton.load(fn.replace('.grail', '.gv'))
    # Check that we can print Grail.
    grail  = open(fn).read().strip()
    CHECK_EQ(grail, a.format('grail'))
    check_grail(a)
