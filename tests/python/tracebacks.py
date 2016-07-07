#! /usr/bin/env python

import re
import subprocess
from test import *
from vcsn_tools.config import config

def canonical(s):
     '''Clean up paths (keep only the last two directories to be
     independent of where vcsn was installed), and the line numbers (so
     that we may edit files in vcsn/ without having to tweak the
     lines here).
     '''
     def place(m):
         line = "??" if 'vcsn' in m.group(1) else m.group(2)
         return '"{}", line {}'.format(m.group(1), line)
     return re.sub('".*?([^/\n]+/[^/\n]+)", line (\d+)', place, s)

for t in ['non-verbose', 'verbose']:
    try:
        subprocess.check_output([config['python'], mefile(t, 'py')],
                                stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
        output = e.output.decode('utf-8')
        ## Remove warnings about missing packages (e.g., regex) from output
        output = re.sub(r'(?m)^.*[Ww]arning.*\n?', '', output)
        ## Clean up paths and line numbers
        output = canonical(output)
        with open(mefile(t, 'out')) as f:
            CHECK_EQ(f.read(), output)
    else:
        FAIL(mefile(t, 'py'), 'did not fail!')
