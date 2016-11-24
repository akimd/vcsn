#! /usr/bin/env python

import re
import subprocess
from test import *
from vcsn_tools.config import config

def canonical(s):
     '''Clean up paths (keep only the last two directories to be
     independent of where vcsn was installed), and the line numbers (so
     that we may edit files in vcsn/ without having to tweak the
     lines here). Also remove the escape sequence trying to set the window
     title.
     '''
     s = re.sub('\033]0;[^\a]*\a', '', s)
     def place(m):
         line = "??" if 'vcsn' in m.group(1) else m.group(2)
         return '"{}", line {}'.format(m.group(1), line)
     return re.sub('".*?([^/\n]+/[^/\n]+)", line (\d+)', place, s)

tests = ['non-verbose', 'verbose']

## -------- ##
## Python.  ##
## -------- ##

for t in tests:
    try:
        # IPython needs `.ipy` files, but Python doesn't care,
        # so the tests scripts have the `ipy` extension.
        subprocess.check_output([config['python'], mefile(t, 'ipy')],
                                stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
        output = e.output.decode('utf-8')
        ## Remove warnings about missing packages (e.g., regex) from output
        output = re.sub(r'(?m)^.*[Ww]arning.*\n?', '', output)
        ## Clean up paths and line numbers
        output = canonical(output)
        with open(mefile(t, 'py.out')) as f:
            CHECK_EQ(f.read(), output)
    else:
        FAIL(mefile(t, 'ipy'), 'did not fail!')

## --------- ##
## IPython.  ##
## --------- ##


# Check for IPython
ipython = None
if 'missing' in config['ipython']:
    SKIP('missing IPython')
else:
    try: # Check that ipython is running Python 3
        subprocess.check_output([config['ipython'], '-c',
            'import sys; assert sys.version_info.major >= 3'])
    except Exception as e:
        SKIP(config['ipython'], 'runs Python < 3', e)
    else:
        # Check that IPython is at least IPython 3
        try:
            assert subprocess.check_output([config['ipython'], '-c',
                'import IPython; assert IPython.version_info[0] >= 3']) == ""
        except Exception as e:
            SKIP("runs IPython <= 3, tracebacks won't be filtered correctly", e)
        else:
            ipython = config['ipython']

if ipython:
    for t in tests:
        # IPython doesn't fail and prints on stdout,
        # so not try..except, redirection to stdout nor warning removal here.
        # However the warnings will still appear on stderr, so redirect to devnull.
        output = subprocess.check_output([ipython, mefile(t, 'ipy')],
                                         stderr=subprocess.DEVNULL)
        output = output.decode('utf-8')
        ## Clean up absolute paths
        output = re.sub('/.*?([^/\n]+/[^/\n]+) in', r'\1 in', output)
        ## Clean up line numbers
        output = re.sub(r' \d+', ' ??', output)
        ## Remove window title escape sequence
        output = re.sub('\033]0;[^\a]*\a', '', output)
        with open(mefile(t, 'ipy.out')) as f:
            CHECK_EQ(f.read(), output)
