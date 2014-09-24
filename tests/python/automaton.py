#! /usr/bin/env python
# -*- coding: utf-8 -*-

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

# Syntax error: string not closed.
xfail(r'''digraph
{
  vcsn_context = "lal_char(a)_b
}
''')

# Syntax error: attributes are assignments.
xfail(r'''digraph
{
  vcsn_context = "lal_char(a)_b"
  a [attribute]
}
''')

# Syntax error: attributes are assignments.
xfail(r'''digraph
{
  vcsn_context = "lal_char(a)_b"
  a [attribute =]
}
''')

# Syntax error: comma used after empty attribute.
xfail(r'''digraph
{
  vcsn_context = "lal_char(a)_b"
  a [,a=a]
}
''')

# Syntax error: semicolon used after empty attribute
xfail(r'''digraph
{
  vcsn_context = "lal_char(a)_b"
  a [;a=a]
}
''')

# Syntax error: semicolon used after empty attribute
xfail(r'''digraph
{
  vcsn_context = "lal_char(a)_b"
  a [a=a,;]
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

# An open context (letters are not specified).
CHECK_EQ(vcsn.automaton(r'''digraph
{
  vcsn_context = "lal_char(abcd)_b"
  0 -> 0 [label="a, b, c, d"]
}'''),
vcsn.automaton(r'''digraph
{
  vcsn_context = "lal_char_b"
  0 -> 0 [label="a, b, c, d"]
}'''))

# An open tuple context.
CHECK_EQ(vcsn.automaton(r'''digraph
{
  vcsn_context = "lat<lal_char,law_char>_b"
  0 -> 0 [label="(a,x),(b,xyz),(c,\\e)"]
}'''),
vcsn.automaton(r'''digraph
{
  vcsn_context = "lat<lal_char(abc),law_char(xyz)>_b"
  0 -> 0 [label="(a,x),(b,xyz),(c,\\e)"]
}'''))

# Coverage: different rarely used features.
CHECK_EQ(vcsn.automaton(r'''digraph
{
  vcsn_context = "lal_char()_b"
  {
    node [shape = circle]
    0 [color = DimGray]
  }
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
  graph [a = "graph attribute",]
  edge [a = "edge attribute";]
  node [a = "node attribute"]
  0:port:nw [a1 = a1, a2 = a2; a3 = a3 a4 = a4]
}'''))


## ------------ ##
## Pretty dot.  ##
## ------------ ##

from vcsn.dot import _dot_pretty
# Make sure to check the rendering useful/useless named/nameless
# states, weights, and spontaneous transitions.
c = vcsn.context('lan_char(ab)_z')
a = c.ratexp('<2>a+<2>b').thompson()
CHECK_EQ('''digraph
{
  vcsn_context = "lan<lal_char(ab)>_z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F1
  }
  {
    node [fillcolor = cadetblue1, shape = circle, style = "filled,rounded", width = 0.5]
    0
    1
    2
    3
    4
    5
  }
  I0 -> 0
  0 -> 2 [label = "ε"]
  0 -> 4 [label = "ε"]
  1 -> F1
  2 -> 3 [label = "⟨2⟩a"]
  3 -> 1 [label = "ε"]
  4 -> 5 [label = "⟨2⟩b"]
  5 -> 1 [label = "ε"]
}''',
         _dot_pretty(a.dot()))

CHECK_EQ('''digraph
{
  vcsn_context = "lan<lal_char(ab)>_z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F11
  }
  {
    node [fillcolor = cadetblue1, shape = circle, style = "filled,rounded", width = 0.5]
    0 [label = "0, 0", shape = box]
    1 [label = "2, 0", shape = box]
    2 [label = "4, 0", shape = box]
    3 [label = "2, 2", shape = box]
    4 [label = "2, 4", shape = box, fillcolor = lightgray]
    5 [label = "4, 2", shape = box, fillcolor = lightgray]
    6 [label = "4, 4", shape = box]
    7 [label = "3, 3", shape = box]
    8 [label = "5, 5", shape = box]
    9 [label = "1, 3", shape = box]
    10 [label = "1, 5", shape = box]
    11 [label = "1, 1", shape = box]
  }
  I0 -> 0
  0 -> 1 [label = "ε"]
  0 -> 2 [label = "ε"]
  1 -> 3 [label = "ε"]
  1 -> 4 [label = "ε", color = DimGray]
  2 -> 5 [label = "ε", color = DimGray]
  2 -> 6 [label = "ε"]
  3 -> 7 [label = "⟨4⟩a"]
  6 -> 8 [label = "⟨4⟩b"]
  7 -> 9 [label = "ε"]
  8 -> 10 [label = "ε"]
  9 -> 11 [label = "ε"]
  10 -> 11 [label = "ε"]
  11 -> F11
}''',
         _dot_pretty((a&a).dot()))

## ------------------------------- ##
## Output: dot, dot2tex and TikZ.  ##
## ------------------------------- ##

import glob
for fn in glob.glob(os.path.join(medir, '*.in.gv')):
    print(fn)
    a = vcsn.automaton.load(fn)

    exp = open(fn.replace('.in.gv', '.out.gv')).read().strip()
    CHECK_EQ(exp, a.format('dot'))

    exp  = open(fn.replace('.in.gv', '.tex.gv')).read().strip()
    CHECK_EQ(exp, a.format('dot2tex'))

    exp = open(fn.replace('.in.gv', '.tex')).read().strip()
    CHECK_EQ(exp, a.format('tikz'))


# Check state names in TikZ.
a = vcsn.context('lal_char_b').ratexp('\e+a').derived_term()
exp = open(os.path.join(medir, 'derived-term.tex')).read().strip()
CHECK_EQ(exp, a.format('tikz'))


## ----------- ##
## I/O: Daut.  ##
## ----------- ##

from vcsn.dot import to_dot, from_dot
for fn in glob.glob(os.path.join(medir, '*.in.gv')):
    a = vcsn.automaton.load(fn)

    # Check output.
    daut = a.format('daut')
    exp = open(fn.replace('.in.gv', '.daut')).read().strip()
    CHECK_EQ(exp, daut)

    # Check input.
    CHECK_EQ(a, vcsn.automaton(daut, 'daut'))


## ----------- ##
## I/O: FAdo.  ##
## ----------- ##

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

## --------------- ##
## Output: Grail.  ##
## --------------- ##

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
