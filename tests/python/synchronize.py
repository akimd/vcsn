#! /usr/bin/env python

import vcsn
from test import *

def load(file):
    return open(medir + "/" + file + ".gv").read().strip()

## ------------- ##
## synchronize.  ##
## ------------- ##

def check(input, file):
    aut = load(file)
    syn = input.synchronize()
    CHECK_EQ(syn, file)
    CHECK_EQUIV(syn, input)
    CHECK(syn.is_synchronized())

ctx = vcsn.context('lat<law_char, law_char>, b')

check(ctx.expression(r"""'abc,de'('f,\e'+('g,h''i,j'*'\e,k'))'l,mn'""").standard().minimize().strip(), "bool")

ctx = vcsn.context('lat<lal_char, lan_char>, z')

check(ctx.expression(r"""'a,e'('f,\e'+('g,h''i,j'*'o,k'))'l,n'""").standard().minimize().strip(), "z")
