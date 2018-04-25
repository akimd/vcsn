#! /usr/bin/env python

import re
import subprocess
from test import *
from vcsn_cxx import configuration as config

def canonical(s):
    '''Clean up paths (keep only the last two directories to be
    independent of where vcsn was installed), and the line numbers (so
    that we may edit files in vcsn/ without having to tweak the
    lines here). Also remove the escape sequence trying to set the window
    title.
    '''
    res = re.sub('\033]0;[^\a]*\a', '', s)
    def place(m):
        line = "??" if 'vcsn' in m.group(1) else m.group(2)
        return '"{}", line {}'.format(m.group(1), line)
    res = re.sub('".*?([^/\n]+/[^/\n]+)", line (\d+)', place, res)
    return res

tests = ['non-verbose', 'verbose']

## -------- ##
## Python.  ##
## -------- ##

print("Checking Python")
for t in tests:
    try:
        # IPython needs `.ipy` files, but Python doesn't care,
        # so the tests scripts have the `ipy` extension.
        print("testing: {}".format(mefile(t, 'ipy')))
        subprocess.check_output([config('configuration.python'), mefile(t, 'ipy')],
                                stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
        output = e.output.decode('utf-8')
        ## Remove warnings about missing packages (e.g., regex) from output
        output = re.sub(r'(?m)^.*[Ww]arning.*\n?', '', output)
        ## Clean up paths and line numbers
        output = canonical(output)
        print("canonical output: {}".format(output))
        with open(mefile(t, 'py.out')) as f:
            CHECK_EQ(f.read(), output)
    else:
        FAIL(mefile(t, 'ipy'), 'did not fail!')


## --------- ##
## IPython.  ##
## --------- ##

print("Checking for IPython")
ipython = None
ipy = config('configuration.ipython')
if 'missing' in ipy:
    SKIP('missing IPython')
else:
    try: # Check that ipython is running Python 3.
        subprocess.check_output([ipy, '-c',
            'import sys; assert sys.version_info.major >= 3'])
    except Exception as e:
        SKIP(ipy, 'runs Python < 3', e)
    else:
        print("{} run Python >= 3".format(ipy))
        # Check that IPython is at least IPython 3.
        try:
            res = subprocess.check_output([ipy, '-c',
                'import IPython; assert IPython.version_info[0] >= 3'])
            # Eliminate possible escapes to change the terminal title.
            res = canonical(res.decode("utf-8"))
            assert res == ""
        except Exception as e:
            SKIP("runs IPython <= 3, tracebacks won't be filtered correctly", e)
        else:
            ipython = ipy


if ipython:
    print("Checking IPython")
    for t in tests:
        print("running {} {}".format(ipython, mefile(t, 'ipy')))
        # so not try..except, redirection to stdout nor warning removal here.
        # However the warnings will still appear on stderr, so redirect to devnull.
        output = subprocess.check_output([ipython, mefile(t, 'ipy')],
                                         stderr=subprocess.DEVNULL)
        output = output.decode('utf-8')
        ## Clean up absolute paths (possibly ~/src/...).
        output = re.sub('~?/.*?([^/\n]+/[^/\n]+) in', r'\1 in', output)
        ## Clean up line numbers.
        output = re.sub(r' \d+', ' ??', output)
        ## Remove window title escape sequence.
        output = re.sub('\033]0;[^\a]*\a', '', output)
        with open(mefile(t, 'ipy.out')) as f:
            CHECK_EQ(f.read(), output)
