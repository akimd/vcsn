#! /usr/bin/env python
# -*- coding: utf-8 -*-

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
  0 -> 0 [label="(a|x),(b|xyz),(c|\\e)"]
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

# A context string with ".
CHECK_EQ(vcsn.automaton(r'''digraph
{
  vcsn_context = "lal_char(\"\\'), b"
  0 -> 0 [label="\", \\'"]
}'''),
vcsn.automaton(r'''digraph
{
  vcsn_context = "lal_char, b"
  0 -> 0 [label="\", \\'"]
}'''))

# A dot file which uses the HTML strings.  And a subgraph.
CHECK_EQ(r'''context = "nullableset<letterset<char_letters(ab)>>, b"
$ -> 0
$ -> 3
0 -> 1 a, b
1 -> $
2 -> 1 a
3 -> 2 b''',
         vcsn.automaton(filename=medir + '/html.gv').format('daut'))


## --------------- ##
## automaton.dot.  ##
## --------------- ##

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
    node [fontsize = 12, fillcolor = cadetblue1, shape = circle, style = "filled,rounded", height = 0.4, width = 0.4, fixedsize = true]
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
         a.dot())

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
    node [fontsize = 12, fillcolor = cadetblue1, shape = circle, style = "filled,rounded", height = 0.4, width = 0.4, fixedsize = true]
    0 [label = "0, (0, !ε)", shape = box, fixedsize = false]
    1 [label = "2, (0, !ε)", shape = box, fixedsize = false]
    2 [label = "4, (0, !ε)", shape = box, fixedsize = false]
    3 [label = "2, (2, ε)", shape = box, fixedsize = false]
    4 [label = "2, (4, ε)", shape = box, fixedsize = false, fillcolor = lightgray]
    5 [label = "4, (2, ε)", shape = box, fixedsize = false, fillcolor = lightgray]
    6 [label = "4, (4, ε)", shape = box, fixedsize = false]
    7 [label = "3, (3, !ε)", shape = box, fixedsize = false]
    8 [label = "5, (5, !ε)", shape = box, fixedsize = false]
    9 [label = "1, (3, !ε)", shape = box, fixedsize = false]
    10 [label = "1, (5, !ε)", shape = box, fixedsize = false]
    11 [label = "1, (1, ε)", shape = box, fixedsize = false]
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
         (a & a).dot())

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
    node [fontsize = 12, fillcolor = cadetblue1, shape = circle, style = "filled,rounded", height = 0.4, width = 0.4, fixedsize = true]
    0 [tooltip = "0, (0, !ε)"]
    1 [tooltip = "2, (0, !ε)"]
    2 [tooltip = "4, (0, !ε)"]
    3 [tooltip = "2, (2, ε)"]
    4 [tooltip = "2, (4, ε)", fillcolor = lightgray]
    5 [tooltip = "4, (2, ε)", fillcolor = lightgray]
    6 [tooltip = "4, (4, ε)"]
    7 [tooltip = "3, (3, !ε)"]
    8 [tooltip = "5, (5, !ε)"]
    9 [tooltip = "1, (3, !ε)"]
    10 [tooltip = "1, (5, !ε)"]
    11 [tooltip = "1, (1, ε)"]
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
         (a & a).dot("tooltip"))

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
    node [shape = point, width = 0]
    0 [label = "0, (0, !ε)"]
    1 [label = "2, (0, !ε)"]
    2 [label = "4, (0, !ε)"]
    3 [label = "2, (2, ε)"]
    4 [label = "2, (4, ε)", fillcolor = lightgray]
    5 [label = "4, (2, ε)", fillcolor = lightgray]
    6 [label = "4, (4, ε)"]
    7 [label = "3, (3, !ε)"]
    8 [label = "5, (5, !ε)"]
    9 [label = "1, (3, !ε)"]
    10 [label = "1, (5, !ε)"]
    11 [label = "1, (1, ε)"]
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
         (a & a).dot("transitions"))


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
    node [fontsize = 12, fillcolor = cadetblue1, shape = circle, style = "filled,rounded", height = 0.4, width = 0.4, fixedsize = true]
    0 [label = "∅ᶜ", shape = box, fixedsize = false]
  }
  I0 -> 0
  0 -> F0
}''',
         vcsn.context('lal_char, b').expression('\z{c}').derived_term().dot())

## ------------- ##
## dot: simple.  ##
## ------------- ##

ctx = vcsn.context('lal<string>, b')
e = ctx.expression("'🍺':'🍾':'☕️':'🍷' & [^]*'🍺'[^]*'☕️'[^]* & ([^]*'🍷''🍾'[^]*){c}")
CHECK_EQ(open(medir + '/drinks-simple.gv').read().strip(),
         e.automaton().minimize().dot('simple'))


## ------------------------------- ##
## Output: dot, dot2tex and TikZ.  ##
## ------------------------------- ##

import glob
for fn in glob.glob(os.path.join(medir, '*.in.gv')):
    print("Checking: ", fn)
    a = vcsn.automaton(filename=fn)

    exp = open(fn.replace('.in.gv', '.out.gv')).read().strip()
    CHECK_EQ(exp, a.format('dot'))

    exp = open(fn.replace('.in.gv', '.tex.gv')).read().strip()
    CHECK_EQ(exp, a.format('dot,latex'))

    exp = open(fn.replace('.in.gv', '.tex')).read().strip()
    CHECK_EQ(exp, a.format('tikz'))


# Check state names in TikZ.
a = vcsn.context('lal_char, b').expression('\e+a').derived_term()
exp = open(os.path.join(medir, 'derived-term.tex')).read().strip()
CHECK_EQ(exp, a.format('tikz'))


## ------ ##
## Lazy.  ##
## ------ ##

# Check the case of a lazy automaton.
ctx = vcsn.context('lal_char, q')
a = ctx.expression('a*+(<2>a)*').derived_term(deterministic=True, lazy=True)
a('aaa')

exp = open(os.path.join(medir, 'lazy.out.gv')).read().strip()
CHECK_EQ(exp, a.format('dot'))

exp = open(os.path.join(medir, 'lazy.tex')).read().strip()
CHECK_EQ(exp, a.format('tikz'))


## ----------- ##
## I/O: Daut.  ##
## ----------- ##

for fn in glob.glob(os.path.join(medir, '*.in.gv')):
    a = vcsn.automaton(filename=fn)

    # Check output.
    daut = a.format('daut')
    exp = open(fn.replace('.in.gv', '.daut')).read().strip()
    CHECK_EQ(exp, daut)

    # Check input: make sure we can read it.
    CHECK_EQ(a, vcsn.automaton(exp, 'daut'))
    CHECK_EQ(a, vcsn.automaton(exp, 'auto'))
    CHECK_EQ(a, vcsn.automaton(exp))


# A daut file whose names have quotes: beware of building "Ifoo" and
# "Ffoo", not I"foo" and F"foo".
CHECK_EQ(r'''digraph
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
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "foo", shape = box]
  }
  I0 -> 0
  0 -> F0
}''',
         vcsn.automaton('''$ -> "foo"
         "foo" -> $''', strip=False))

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
    exp = vcsn.automaton(filename=fn.replace('.fado', '.gv'))
    # Check that we can read FAdo.
    CHECK_EQ(exp, vcsn.automaton(filename=fn, format='fado'))
    CHECK_EQ(exp, vcsn.automaton(filename=fn, format='auto'))

    # Check that we can print FAdo.
    fado = open(fn).read().strip()
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
    a = vcsn.automaton(filename=fn.replace('.grail', '.gv'))
    # Check that we can print Grail.
    grail = open(fn).read().strip()
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
