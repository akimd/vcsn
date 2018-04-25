#! /usr/bin/env python

import vcsn
from test import *

## ------------- ##
## synchronize.  ##
## ------------- ##

def check(input, file):
    aut = metext(file, 'gv')
    syn = input.synchronize()
    CHECK_EQ(aut, syn)
    #CHECK_EQUIV(input, syn) #free automaton for determinize?
    CHECK(syn.is_synchronized())

ctx = vcsn.context('[...]* | [...]*')

check(ctx.expression(r"""(abc|de)((f|\e)+((g|h)(i|j)*(\e|k)))(l|mn)""").standard(), "bool")
check(ctx.expression(r"""(abc|de)((f|\e)+((g|h)(i|j)*(\e|k)))(l|mn)""").standard().minimize().strip(), "bool_min")

ctx = vcsn.context('[...] x [...] -> Z')

check(ctx.expression(r"""(a|e)((f|\e)+((g|h)(i|j)*(o|k)))(l|n)""").standard(), "z")
check(ctx.expression(r"""(a|e)((f|\e)+((g|h)(i|j)*(o|k)))(l|n)""").standard().minimize().strip(), "z_min")


## ----------------- ##
## delay_automaton.  ##
## ----------------- ##

def check(input, file):
    aut = metext(file, 'gv')
    d = input.delay_automaton()
    CHECK_EQ(aut, d)

ctx = vcsn.context('[...]* | [...]*')

check(ctx.expression(r"""(abc|de)((f|\e)+((g|h)(i|j)*(\e|k)))(l|mn)""").standard(), "bool_delay")
check(ctx.expression(r"""(abc|de)((f|\e)+((g|h)(i|j)*(\e|k)))(l|mn)""").standard().minimize().strip(), "bool_min_delay")

ctx = vcsn.context('[...] x [...] -> z')

check(ctx.expression(r"""(a|e)((f|\e)+((g|h)(i|j)*(o|k)))(l|n)""").standard(), "z_delay")
check(ctx.expression(r"""(a|e)((f|\e)+((g|h)(i|j)*(o|k)))(l|n)""").standard().minimize().strip(), "z_min_delay")
