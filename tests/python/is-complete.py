#! /usr/bin/env python

import vcsn
from test import *

a = vcsn.automaton('''
digraph {
    vcsn_context="lal_char(ab), b"
    2 -> 2 [label="a"]
}
''')
CHECK(not a.is_complete())

a = vcsn.automaton('''
digraph {
    vcsn_context="lal_char(ab), b"
    2 -> 2 [label="a, b"]
}
''')
CHECK(not a.is_complete())

a = vcsn.automaton('''
digraph {
    vcsn_context="lal_char(ab), b"
    I0 -> 2
    2 -> 2 [label="a"]
}
''')
CHECK(not a.is_complete())

a = vcsn.automaton('''
digraph {
    vcsn_context="lal_char(ab), b"
    I0 -> 2
    2 -> 2 [label="a, b"]
}
''')
CHECK(a.is_complete())
