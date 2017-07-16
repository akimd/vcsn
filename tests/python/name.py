#! /usr/bin/env python

import vcsn
from test import *

e = vcsn.B.expression('(?<E>[ab]*)(?<A>a*)(?<F>[ab]){2}')

def check(input, **exp):
    for format, value in exp.items():
        CHECK_EQ(value, input.format(format))

check(e,
      text='EAF{2}',
      utf8='EAF²',
      latex=r'\mathsf{E} \, \mathsf{A} \, {\mathsf{F}}^{2}')

check(e.expansion(),
      text='a.[F + AF{2} + EAF{2}] + b.[F + EAF{2}]',
      utf8='a⊙[F⊕AF²⊕EAF²] ⊕ b⊙[F⊕EAF²]',
      latex=(r'a \odot \left[\mathsf{F} \oplus \mathsf{A} \, {\mathsf{F}}^{2} \oplus \mathsf{E} \, \mathsf{A} \, {\mathsf{F}}^{2}\right]'
             r' \oplus b \odot \left[\mathsf{F} \oplus \mathsf{E} \, \mathsf{A} \, {\mathsf{F}}^{2}\right]'))

check(e.derived_term(),
      dot=r'''digraph
{
  vcsn_context = "letterset<char_letters(ab)>, b"
  rankdir = LR
  edge [arrowhead = vee, arrowsize = .6]
  {
    node [shape = point, width = 0]
    I0
    F3
  }
  {
    node [shape = circle, style = rounded, width = 0.5]
    0 [label = "EAF{2}", shape = box]
    1 [label = "F", shape = box]
    2 [label = "AF{2}", shape = box]
    3 [label = "\\e", shape = box]
  }
  I0 -> 0
  0 -> 0 [label = "a, b"]
  0 -> 1 [label = "a, b"]
  0 -> 2 [label = "a"]
  1 -> 3 [label = "a, b"]
  2 -> 1 [label = "a, b"]
  2 -> 2 [label = "a"]
  3 -> F3
}''')
