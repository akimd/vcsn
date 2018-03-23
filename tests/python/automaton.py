#! /usr/bin/env python
# -*- coding: utf-8 -*-

import vcsn
from test import *

## -------------- ##
## dot: parsing.  ##
## -------------- ##

# Check invalid input.
def xfail(a, *args):
    XFAIL(lambda: vcsn.automaton(a), *args)

# Syntax error: missing }.
xfail(r'''digraph
{
  vcsn_context = "[a]? → 𝔹"
''', r'''4.1: syntax error, unexpected end

^
  while reading automaton''')

# Syntax error: string not closed.
xfail(r'''digraph
{
  vcsn_context = "[a]?, b
}
''', r'''3.18-5.0: unexpected end of file in a string
  vcsn_context = "[a]?, b
                 ^^^^^^^^
  while reading automaton''')

# Syntax error: attributes are assignments.
xfail(r'''digraph
{
  vcsn_context = "[a]? → 𝔹"
  a [attribute]
}
''', r'''4.15: syntax error, unexpected ], expecting =
  a [attribute]
              ^
  while reading automaton''')

# Syntax error: attributes are assignments.
xfail(r'''digraph
{
  vcsn_context = "[a]? → 𝔹"
  a [attribute =]
}
''', r'''4.17: syntax error, unexpected ], expecting identifier
  a [attribute =]
                ^
  while reading automaton''')

# Syntax error: comma used after empty attribute.
xfail(r'''digraph
{
  vcsn_context = "[a]? → 𝔹"
  a [,a=a]
}
''', r'''4.6: syntax error, unexpected ",", expecting ] or identifier
  a [,a=a]
     ^
  while reading automaton''')

# Syntax error: semicolon used after empty attribute
xfail(r'''digraph
{
  vcsn_context = "[a]? → 𝔹"
  a [;a=a]
}
''', r'''4.6: syntax error, unexpected ;, expecting ] or identifier
  a [;a=a]
     ^
  while reading automaton''')

# Syntax error: semicolon used after empty attribute
xfail(r'''digraph
{
  vcsn_context = "[a]? → 𝔹"
  a [a=a,;]
}
''', r'''4.10: syntax error, unexpected ;, expecting ] or identifier
  a [a=a,;]
         ^
  while reading automaton''')

# Invalid label: letter not in alphabet.
xfail(r'''digraph
{
  vcsn_context = "[]? → 𝔹"
  0 -> 1 [label = a]
  1 -> F1
  I0 -> 0
}
''', r'''4.10-20: invalid label: unexpected a
  while reading: "a"
  0 -> 1 [label = a]
         ^^^^^^^^^^^
  while reading automaton''')

# Invalid label: aa is not valid in LAL.
xfail(r'''digraph
{
  vcsn_context = "[a]? → 𝔹"
  0 -> 1 [label = "aa"]
  1 -> F1
  I0 -> 0
}
''', r'''4.10-23: Poly[[a]? -> B]: unexpected trailing characters: "a"
  while reading: "aa"
  0 -> 1 [label = "aa"]
         ^^^^^^^^^^^^^^
  while reading automaton''')

# Invalid label: missing '>'.
xfail(r'''digraph
{
  vcsn_context = "[a]? → ℤ"
  0 -> 1 [label = "<2"]
  1 -> F1
  I0 -> 0
}
''', r'''4.10-23: missing ">" after "<2"
  while reading: "<2"
  0 -> 1 [label = "<2"]
         ^^^^^^^^^^^^^^
  while reading automaton''')

# No context defined (see the typo in vcsn_context).
xfail(r'''digraph
{
  vcsn_contxt = "[ab] -> b"
  0 -> 1 [label = a]
  1 -> F1
  I0 -> 0
}
''', r'''4.3: no vcsn_context defined
  0 -> 1 [label = a]
  ^
  while reading automaton''')


# Invalid context.
xfail(r'''digraph
{
  vcsn_context = "unknown"
  0 -> 1 [label = a]
  1 -> F1
  I0 -> 0
}
''', r'''3.18-26: expected labelset here:
unknown
^_
  vcsn_context = "unknown"
                 ^^^^^^^^^
4.3: no vcsn_context defined
  0 -> 1 [label = a]
  ^
  while reading automaton''')


# Invalid context.
xfail(r'''digraph
{
  vcsn_context = "[...]? → unknown"
}''',
      r'''3.18-35: expected weightset here:
[...]? → unknown
         ^_
  vcsn_context = "[...]? → unknown"
                 ^^^^^^^^^^^^^^^^^^
  while reading automaton''')


# Invalid context.
xfail('context = "RatE[[...]? → 𝔹𝔹]  →  𝔹𝔹𝔹"',
      r'''1.1-37: expected ']' here:
RatE[[...]? → 𝔹𝔹]  →  𝔹𝔹𝔹
               ^_
context = "RatE[[...]? → 𝔹𝔹]  →  𝔹𝔹𝔹"
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  while reading automaton''')


# Invalid initial label.
xfail(r'''digraph
{
  vcsn_context = "[ab]? → 𝔹"
  0 -> 1 [label = a]
  1 -> F1
  I0 -> 0 [label = a]
}
''', r'''6.11-21: edit_automaton: invalid initial entry: a
  I0 -> 0 [label = a]
          ^^^^^^^^^^^
  while reading automaton''')


# Invalid final label.
xfail(r'''digraph
{
  vcsn_context = "[ab]? → 𝔹"
  0 -> 1 [label = a]
  1 -> F1 [label = a]
  I0 -> 0
}
''', r'''5.11-21: edit_automaton: invalid final entry: a
  1 -> F1 [label = a]
          ^^^^^^^^^^^
  while reading automaton''')


# There are spaces before 'digraph'.
CHECK_EQ(vcsn.automaton(r'''digraph
{
  vcsn_context = "[abcd]? → 𝔹"
  0 -> 0 [label="a, b, c, d"]
}'''),
vcsn.automaton('''
\t  digraph
\t  {
\t    vcsn_context = "[abcd]? → 𝔹"
\t    0 -> 0 [label="a, b, c, d"]
\t  }
'''))

# An open context (letters are not specified).
CHECK_EQ(vcsn.automaton(r'''digraph
{
  vcsn_context = "[abcd]? → 𝔹"
  0 -> 0 [label="a, b, c, d"]
}'''),
vcsn.automaton(r'''digraph
{
  vcsn_context = "[...]? → 𝔹"
  0 -> 0 [label="a, b, c, d"]
}'''))

# An open tuple context.
CHECK_EQ(vcsn.automaton(r'''digraph
{
  vcsn_context = "[...]? × [...]* → 𝔹"
  0 -> 0 [label="(a|x),(b|xyz),(c|\\e)"]
}'''),
vcsn.automaton(r'''digraph
{
  vcsn_context = "[abc]? × [xyz]* → 𝔹"
  0 -> 0 [label="(a,x),(b,xyz),(c,\\e)"]
}'''))

# Coverage: different rarely used features.
CHECK_EQ(vcsn.automaton(r'''digraph
{
  vcsn_context = "[]? → 𝔹"
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
  "lal(), b"
  graph [a = "graph attribute",]
  edge [a = "edge attribute";]
  node [a = "node attribute"]
  0:port:nw [a1 = a1, a2 = a2; a3 = a3 a4 = a4]
}'''))

# A context string with ".
CHECK_EQ(vcsn.automaton(r'''digraph
{
  vcsn_context = "[\"\\']? → 𝔹"
  0 -> 0 [label="\", \\'"]
}'''),
vcsn.automaton(r'''digraph
{
  vcsn_context = "[...]? → 𝔹"
  0 -> 0 [label="\", \\'"]
}'''))

# A dot file which uses the HTML strings.  And a subgraph.
CHECK_EQ(r'''context = [ab]? → ℚ
$ -> 0
$ -> 3
0 -> 1 a, b
1 -> $
2 -> 1 <123>a
3 -> 2 b''',
         meaut('html.gv').format('daut'))


## --------------- ##
## automaton.dot.  ##
## --------------- ##

# Make sure to check the rendering useful/useless named/nameless
# states, weights, and spontaneous transitions.
c = vcsn.context('[ab] -> z')
a = c.expression('<2>a+<2>b').thompson()
CHECK_EQ('''digraph
{
  vcsn_context = "[ab]? → ℤ"
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
  vcsn_context = "[ab]? → ℤ"
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
  vcsn_context = "[ab]? → ℤ"
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
  vcsn_context = "[ab]? → ℤ"
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
  vcsn_context = "[]? → 𝔹"
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
         vcsn.context('lal, b').expression(r'\z{c}').derived_term().dot())

## ------------- ##
## dot: simple.  ##
## ------------- ##

ctx = vcsn.context('lal<string>, b')
e = ctx.expression("'🍺':'🍾':'☕️':'🍷' & [^]*'🍺'[^]*'☕️'[^]* & ([^]*'🍷''🍾'[^]*){c}")
CHECK_EQ(metext('drinks-simple.gv'),
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
a = vcsn.context('lal, b').expression(r'\e+a').derived_term()
exp = metext('derived-term.tex')
CHECK_EQ(exp, a.format('tikz'))


## ------ ##
## Lazy.  ##
## ------ ##

# Check the case of a lazy automaton.
ctx = vcsn.context('lal, q')
a = ctx.expression('a*+(<2>a)*').derived_term(deterministic=True, lazy=True)
a('aaa')

exp = metext('lazy.out.gv')
CHECK_EQ(exp, a.format('dot'))

exp = metext('lazy.tex')
CHECK_EQ(exp, a.format('tikz'))


## ----------- ##
## I/O: Daut.  ##
## ----------- ##

for fn in glob.glob(os.path.join(medir, '*.in.gv')):
    print('Checking:', fn)
    a = vcsn.automaton(filename=fn)

    # Check daut output.
    daut = a.format('daut')
    ref = open(fn.replace('.in.gv', '.daut')).read().strip()
    CHECK_EQ(ref, daut)

    # Check daut input: make sure we can read it.
    CHECK_EQ(a, vcsn.automaton(ref, 'daut'))
    CHECK_EQ(a, vcsn.automaton(ref, 'auto'))
    CHECK_EQ(a, vcsn.automaton(ref))


# Invalid context.  It took me quite a while to acknowlegde that the
# following is correct: if you feel that the number of carets is
# incorrect, it might be because of the font!  Check from the terminal
# for instance.
xfail('context = "RatE[[...]? → 𝔹𝔹]  →  𝔹𝔹𝔹"',
      r'''1.1-37: expected ']' here:
RatE[[...]? → 𝔹𝔹]  →  𝔹𝔹𝔹
               ^_
context = "RatE[[...]? → 𝔹𝔹]  →  𝔹𝔹𝔹"
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  while reading automaton''')


# A daut file whose names have quotes: beware of building "Ifoo" and
# "Ffoo", not I"foo" and F"foo".
CHECK_EQ(r'''digraph
{
  vcsn_context = "[]? → 𝔹"
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

# Check that "->" behaves as a keyword.
CHECK_EQ(r'''digraph
{
  vcsn_context = "[a]? → 𝔹"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F1
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "0-0", shape = box]
    1 [label = "1-1", shape = box]
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  1 -> F1
}''',
         vcsn.automaton('''$-> 0-0
0-0->1-1 a // Make sure this is not seen as a state "0-0->1-1".
1-1 ->$''', strip=False))

# Invalid transitions
XFAIL(lambda: vcsn.automaton('''context = [abc]? → ℚ
$ -> 0 <a>
0 -> $ <1/2>'''),
      '''2.1-10: Q: invalid numerator: "a"
  while reading: "a"
  while reading: "<a>"
$ -> 0 <a>
^^^^^^^^^^
  while reading automaton''')

XFAIL(lambda: vcsn.automaton('''context = [abc]? → ℚ
$ -> 0 <1/2>
0 -> $ <a>'''),
      '''3.1-10: Q: invalid numerator: "a"
  while reading: "a"
  while reading: "<a>"
0 -> $ <a>
^^^^^^^^^^
  while reading automaton''')

XFAIL(lambda: vcsn.automaton('''context = [abc]? → ℚ
$ -> 0 <1/2>
0 -> 1 <2/a>a
1 -> $ <2>'''),
      '''3.1-13: Q: invalid denominator: "a"
  while reading: "2/a"
  while reading: "<2/a>a"
0 -> 1 <2/a>a
^^^^^^^^^^^^^
  while reading automaton''')


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
    print("Checking {}".format(fn))
    ref = vcsn.automaton(filename=fn.replace('.fado', '.gv'))
    # Check that we can read FAdo.
    CHECK_EQ(ref, vcsn.automaton(filename=fn, format='fado'))
    CHECK_EQ(ref, vcsn.automaton(filename=fn, format='auto'))

    # Check that we can print FAdo.
    fado = open(fn).read().strip()
    CHECK_EQ(fado, ref.format('fado'))
    check_fado(ref)

# Invalid kind
XFAIL(lambda: vcsn.automaton('''@GFA 0 1 * 2 3
  2 a 0
  3 b 1''', 'fado'),
      'fado: bad automaton kind in first line: @GFA')

# Invalid initial states in DFA
XFAIL(lambda: vcsn.automaton('''@DFA 0 1 * 2 3
  2 a 0
  3 b 1''', 'fado'),
      'fado: invalid "*" for DFA in first line')

# Multiple '*' in NFA
XFAIL(lambda: vcsn.automaton('''@NFA 0 1 * 2 3 * 5 2
  2 a 0
  3 b 1''', 'fado'),
      'fado: multiple "*" in first line')

# Trailing characters in acceptor
XFAIL(lambda: vcsn.automaton('''@NFA 0 1 * 2 3
  2 a 0
  3 b 1 c''', 'fado'),
      'fado: unexpected trailing characters after: 3 b 1')

# Epsilon in DFA
XFAIL(lambda: vcsn.automaton('''@DFA 0 1
  2 @epsilon 0
  3 b 1''', 'fado'),
      'fado: unexpected "@epsilon" in DFA, in: 2 @epsilon 0')


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

# Convert an automaton from lal, b to law, z.
CHECK_EQ(vcsn.automaton('''context = [...]* → ℤ
  $ -> 0
  0 -> 1 a, b
  1 -> 1 c
  1 -> $'''),
         vcsn.automaton('''context = [abc]? → 𝔹
  $ -> 0
  0 -> 1 a, b
  1 -> 1 c
  1 -> $''').automaton(vcsn.context("law(abc), z")))

# Convert an automaton to a smaller, valid, alphabet.
CHECK_EQ(vcsn.automaton('''context = [abc]* → ℤ
  0 -> 1 a, b'''),
         vcsn.automaton('''context = [a-z]? → 𝔹
  0 -> 1 a, b''').automaton(vcsn.context("law(abc), z")))

# Convert an automaton to a smaller, invalid, alphabet.
XFAIL(lambda: vcsn.automaton('''context = [abc]? → 𝔹
  0 -> 1 a, b''').automaton(vcsn.context("law(xy), z")))

# Convert to an invalid smaller weightset.
XFAIL(lambda: vcsn.automaton('''context = [abc]? → ℤ
  0 -> 1 <3>a, b''').automaton(vcsn.context("[xy] -> b")))
