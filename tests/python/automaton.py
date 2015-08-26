#! /usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function
import vcsn
from test import *

## -------------- ##
## dot: parsing.  ##
## -------------- ##

# Check invalid input.
def xfail(a):
    XFAIL(lambda: vcsn.automaton(a))

# Syntax error: missing }.
xfail(r'''digraph
{
  vcsn_context = "lal_char(a), b"
''')

# Syntax error: string not closed.
xfail(r'''digraph
{
  vcsn_context = "lal_char(a), b
}
''')

# Syntax error: attributes are assignments.
xfail(r'''digraph
{
  vcsn_context = "lal_char(a), b"
  a [attribute]
}
''')

# Syntax error: attributes are assignments.
xfail(r'''digraph
{
  vcsn_context = "lal_char(a), b"
  a [attribute =]
}
''')

# Syntax error: comma used after empty attribute.
xfail(r'''digraph
{
  vcsn_context = "lal_char(a), b"
  a [,a=a]
}
''')

# Syntax error: semicolon used after empty attribute
xfail(r'''digraph
{
  vcsn_context = "lal_char(a), b"
  a [;a=a]
}
''')

# Syntax error: semicolon used after empty attribute
xfail(r'''digraph
{
  vcsn_context = "lal_char(a), b"
  a [a=a,;]
}
''')

# Invalid label: letter not in alphabet.
xfail(r'''digraph
{
  vcsn_context = "lal_char(), b"
  0 -> 1 [label = a]
  1 -> F1
  I0 -> 0
}
''')

# Invalid label: \e is not valid in LAL.
xfail(r'''digraph
{
  vcsn_context = "lal_char(a), b"
  0 -> 1 [label = "\\e"]
  1 -> F1
  I0 -> 0
}
''')

# Invalid label: aa is not valid in LAL.
xfail(r'''digraph
{
  vcsn_context = "lal_char(a), b"
  0 -> 1 [label = "aa"]
  1 -> F1
  I0 -> 0
}
''')

# Invalid label: missing '>'.
xfail(r'''digraph
{
  vcsn_context = "lal_char(a), z"
  0 -> 1 [label = "<2"]
  1 -> F1
  I0 -> 0
}
''')

# No context defined (see the typo in vcsn_context).
xfail(r'''digraph
{
  vcsn_contxt = "lal_char(ab), b"
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
  vcsn_context = "lal_char(ab), b"
  0 -> 1 [label = a]
  1 -> F1
  I0 -> 0 [label = a]
}
''')

# Invalid final label.
xfail(r'''digraph
{
  vcsn_context = "lal_char(ab), b"
  0 -> 1 [label = a]
  1 -> F1 [label = a]
  I0 -> 0
}
''')

# \e makes no sense when not in lan.
xfail(r'''digraph
{
  vcsn_context = "lal_char(\\e), b"
  0 -> 1 [label = "\\e"]
}
''')

# An open context (letters are not specified).
CHECK_EQ(vcsn.automaton(r'''digraph
{
  vcsn_context = "lal_char(abcd), b"
  0 -> 0 [label="a, b, c, d"]
}'''),
vcsn.automaton(r'''digraph
{
  vcsn_context = "lal_char, b"
  0 -> 0 [label="a, b, c, d"]
}'''))

# An open tuple context.
CHECK_EQ(vcsn.automaton(r'''digraph
{
  vcsn_context = "lat<lal_char,law_char>, b"
  0 -> 0 [label="(a,x),(b,xyz),(c,\\e)"]
}'''),
vcsn.automaton(r'''digraph
{
  vcsn_context = "lat<lal_char(abc),law_char(xyz)>, b"
  0 -> 0 [label="(a,x),(b,xyz),(c,\\e)"]
}'''))

# Coverage: different rarely used features.
CHECK_EQ(vcsn.automaton(r'''digraph
{
  vcsn_context = "lal_char(), b"
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
  "lal_char(), b"
  graph [a = "graph attribute",]
  edge [a = "edge attribute";]
  node [a = "node attribute"]
  0:port:nw [a1 = a1, a2 = a2; a3 = a3 a4 = a4]
}'''))


## ------------- ##
## dot: pretty.  ##
## ------------- ##

from vcsn.dot import _dot_pretty
# Make sure to check the rendering useful/useless named/nameless
# states, weights, and spontaneous transitions.
c = vcsn.context('lan_char(ab), z')
a = c.expression('<2>a+<2>b').thompson()
CHECK_EQ('''digraph
{
  vcsn_context = "nullableset<letterset<char_letters(ab)>>, z"
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

# conjunction: state names, and useless states, etc.
CHECK_EQ('''digraph
{
  vcsn_context = "nullableset<letterset<char_letters(ab)>>, z"
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

# Tooltip.
CHECK_EQ('''digraph
{
  vcsn_context = "nullableset<letterset<char_letters(ab)>>, z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F11
  }
  {
    node [fillcolor = cadetblue1, shape = circle, style = "filled,rounded", width = 0.5]
    0 [tooltip = "0, 0"]
    1 [tooltip = "2, 0"]
    2 [tooltip = "4, 0"]
    3 [tooltip = "2, 2"]
    4 [tooltip = "2, 4", fillcolor = lightgray]
    5 [tooltip = "4, 2", fillcolor = lightgray]
    6 [tooltip = "4, 4"]
    7 [tooltip = "3, 3"]
    8 [tooltip = "5, 5"]
    9 [tooltip = "1, 3"]
    10 [tooltip = "1, 5"]
    11 [tooltip = "1, 1"]
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
         _dot_pretty((a&a).dot(), "tooltip"))

# Transitions.
CHECK_EQ('''digraph
{
  vcsn_context = "nullableset<letterset<char_letters(ab)>>, z"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F11
  }
  {
    node [fillcolor = cadetblue1, shape = circle, style = "filled,rounded", width = 0.5]
    0 [label = "0, 0"]
    1 [label = "2, 0"]
    2 [label = "4, 0"]
    3 [label = "2, 2"]
    4 [label = "2, 4", fillcolor = lightgray]
    5 [label = "4, 2", fillcolor = lightgray]
    6 [label = "4, 4"]
    7 [label = "3, 3"]
    8 [label = "5, 5"]
    9 [label = "1, 3"]
    10 [label = "1, 5"]
    11 [label = "1, 1"]
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
         _dot_pretty((a&a).dot(), "transitions"))


# Empty set.
CHECK_EQ('''digraph
{
  vcsn_context = "letterset<char_letters()>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F0
  }
  {
    node [fillcolor = cadetblue1, shape = circle, style = "filled,rounded", width = 0.5]
    0 [label = "∅{c}", shape = box]
  }
  I0 -> 0
  0 -> F0
}''',
         _dot_pretty(vcsn.context('lal_char, b').expression('\z{c}').derived_term().dot()))

## ------------------------------- ##
## Output: dot, dot2tex and TikZ.  ##
## ------------------------------- ##

import glob
for fn in glob.glob(os.path.join(medir, '*.in.gv')):
    print("Checking: ", fn)
    a = vcsn.automaton(filename = fn)

    exp = open(fn.replace('.in.gv', '.out.gv')).read().strip()
    CHECK_EQ(exp, a.format('dot'))

    exp  = open(fn.replace('.in.gv', '.tex.gv')).read().strip()
    CHECK_EQ(exp, a.format('dot2tex'))

    exp = open(fn.replace('.in.gv', '.tex')).read().strip()
    CHECK_EQ(exp, a.format('tikz'))


# Check state names in TikZ.
a = vcsn.context('lal_char, b').expression('\e+a').derived_term()
exp = open(os.path.join(medir, 'derived-term.tex')).read().strip()
CHECK_EQ(exp, a.format('tikz'))


## ----------- ##
## I/O: Daut.  ##
## ----------- ##

for fn in glob.glob(os.path.join(medir, '*.in.gv')):
    a = vcsn.automaton(filename = fn)

    # Check output.
    daut = a.format('daut')
    exp = open(fn.replace('.in.gv', '.daut')).read().strip()
    CHECK_EQ(exp, daut)

    # Check input.
    CHECK_EQ(a, vcsn.automaton(daut, 'auto'))
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
    exp = vcsn.automaton(filename = fn.replace('.fado', '.gv'))
    # Check that we can read FAdo.
    CHECK_EQ(exp, vcsn.automaton(filename = fn, format = 'fado'))
    CHECK_EQ(exp, vcsn.automaton(filename = fn, format = 'auto'))

    # Check that we can print FAdo.
    fado   = open(fn).read().strip()
    CHECK_EQ(fado, exp.format('fado'))
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
    a = vcsn.automaton(filename = fn.replace('.grail', '.gv'))
    # Check that we can print Grail.
    grail  = open(fn).read().strip()
    CHECK_EQ(grail, a.format('grail'))
    check_grail(a)


## ------------ ##
## Conversion.  ##
## ------------ ##

# Convert an automaton from lal_char, b to law_char, z.
CHECK_EQ(vcsn.automaton('''context = "law_char, z"
  $ -> 0
  0 -> 1 a, b
  1 -> 1 c
  1 -> $''', 'daut'),
         vcsn.automaton('''context = "lal_char(abc), b"
  $ -> 0
  0 -> 1 a, b
  1 -> 1 c
  1 -> $''', 'daut').automaton(vcsn.context("law_char(abc), z")))

# Convert an automaton to a smaller, valid, alphabet.
CHECK_EQ(vcsn.automaton('''context = "law_char(abc), z"
  0 -> 1 a, b''', 'daut'),
         vcsn.automaton('''context = "lal_char(a-z), b"
  0 -> 1 a, b''', 'daut').automaton(vcsn.context("law_char(abc), z")))

# Convert an automaton to a smaller, invalid, alphabet.
XFAIL(lambda: vcsn.automaton('''context = "lal_char(abc), b"
  0 -> 1 a, b''', 'daut').automaton(vcsn.context("law_char(xy), z")))

# Convert to an invalid smaller weightset.
XFAIL(lambda: vcsn.automaton('''context = "lal_char(abc), z"
  0 -> 1 <3>a, b''', 'daut').automaton(vcsn.context("lal_char(xy), b")))
