#! /usr/bin/env python

import os
import re
import sys
from test import *
import vcsn

# The name of the current context.
context = '[abcd]? → b'
# The current context.
ctx = vcsn.context(context)
# Whether expressions or series.
identities = "associative"

# Compute the name of the context.
contexts = {
    'b': "[a-h]* -> b",
    'br': "[a-h]* -> expressionset<[i-n]* -> b>",
    'brr': "[a-h]* -> expressionset<[i-n]* -> expressionset<[w-z]* -> b>>",
    'q': "[a-h]* -> q",
    'qr': "[a-h]* -> expressionset<[i-n]* -> q>",
    'qrr': "[a-h]* -> expressionset<[i-n]* -> expressionset<[w-z]* -> q>>",
    'z': "[a-h]* -> z",
    'zr': "[a-h]* -> expressionset<[i-n]* -> z>",
    'zrr': "[a-h]* -> expressionset<[i-n]* -> expressionset<[w-z]* -> z>>",
}


def context_update():
    global context, ctx  # pylint: disable=global-statement
    if context in contexts:
        context = contexts[context]
    ctx = vcsn.context(context)
    print("# context: {} ({})".format(context, ctx))


def expr(e):
    '''Parse.  If it fails, prepend "! " to the error
    message and return it as result.  Strip the "try -h" line.'''
    try:
        print(e, identities)
        return ctx.expression(e, identities)
    except RuntimeError:
        err = re.sub('^', '! ', str(sys.exc_info()[1]), flags=re.M)
        return r'\n'.join(err.splitlines())


def pp(re):
    '''Parse and pretty-print.  If it fails, prepend "! " to the error
    message and return it as result.  Strip the "try -h" line.'''
    return str(expr(re))


def check_rat_exp(fname):
    file = open(fname, 'r', encoding='utf-8')
    lineno = 0
    global context, identities  # pylint: disable=global-statement
    for line in file:
        lineno += 1
        loc = fname + ':' + str(lineno)

        m = re.match('#.*$|$', line)
        if m is not None:
            continue

        m = re.match('%labels?: (.*)$', line)
        if m is not None:
            labels = m.group(1)
            print('# %labels:', labels)
            prev = context
            if labels == "letters":
                context = re.sub(r'(\[.*?\])\*', r'\1', context)
            elif labels == "words":
                context = re.sub(r'(\[.*?\])([^\*])', r'\1*\2', context)
            else:
                context = labels + " -> " + context.split('->', 2)[1]
            print("# context: from {} to {}".format(prev, context))
            context_update()
            continue

        m = re.match('%weights?: (.*)$', line)
        if m is not None:
            weights = m.group(1)
            print('# %weights:', weights)
            context = context.split(',', 2)[0] + " → " + weights
            context_update()
            continue

        m = re.match('%context: (.*)$', line)
        if m is not None:
            context = m.group(1)
            print('# %context:', context)
            context_update()
            continue

        m = re.match('%identities: (.*)$', line)
        if m is not None:
            identities = m.group(1)
            print('# %identities:', identities)
            continue

        m = re.match('%include: (.*)$', line)
        if m is not None:
            check_rat_exp(os.path.dirname(fname) + '/' + m.group(1))
            continue

        # == tests that boths are equivalent.
        # => check the actual result.
        m = re.match(r'(.*\S)\s*(=>|==)\s*(.*)$', line)
        if m is not None:
            l = m.group(1)
            op = m.group(2)
            r = m.group(3)
            if op == '==':
                # If we failed to convert l, then expr(l) is a string.  Don't
                # check a string against an expression.  This is needed for
                # xfail3.
                l = expr(l)
                r = expr(r)
                if isinstance(l, str):
                    r = str(r)
                CHECK_EQ(r, l, loc)
            else:
                CHECK_EQ(r, pp(l), loc)
            continue

        # !: Look for syntax errors.
        m = re.match(r'(.*\S)\s+(!.*)$', line)
        if m is not None:
            l = m.group(1)
            err = m.group(2)
            L = pp(l)
            CHECK_EQ(err, L, loc)
            continue

        print(loc + ":", 'invalid input:', line)

check_rat_exp(sys.argv[1])
