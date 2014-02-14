#! /usr/bin/env python

import vcsn
from test import *

a = vcsn.automaton('''
digraph {
    vcsn_context="lal_char(ab)_b"
    2 -> 2 [label="a"]
}
''')
CHECK_EQ(False, a.is_complete())

a = vcsn.automaton('''
digraph {
    vcsn_context="lal_char(ab)_b"
    2 -> 2 [label="a, b"]
}
''')
CHECK_EQ(False, a.is_complete())

a = vcsn.automaton('''
digraph {
    vcsn_context="lal_char(ab)_b"
    I0 -> 2
    2 -> 2 [label="a"]
}
''')
CHECK_EQ(False, a.is_complete())

a = vcsn.automaton('''
digraph {
    vcsn_context="lal_char(ab)_b"
    I0 -> 2
    2 -> 2 [label="a, b"]
}
''')
CHECK_EQ(True, a.is_complete())

PLAN()
