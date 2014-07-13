from __future__ import print_function

import os
import sys

from vcsn_cxx import *
from vcsn_version import *

def _dot_to_svg(dot, engine="dot", *args):
    "Return the conversion of a Dot source into SVG."
    import subprocess
    proc = subprocess.Popen([engine, '-Tsvg'] + list(args),
                            stdin=subprocess.PIPE,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE)
    proc.stdin.write(dot.encode('utf-8'))
    out, err = proc.communicate()
    status = proc.wait()
    if status:
        raise RuntimeError("dot failed: " + err.decode('utf-8'))
    return out.decode('utf-8')

def _info_to_dict(info):
    """Convert a "key: value" list of lines into a dictionary.
    Convert Booleans into bool, and likewise for integers.
    """
    res = dict()
    for l in info.splitlines():
        (k, v) = l.split(':')
        v = v.strip()
        # Beware that we may display "N/A" for Boolean (e.g., "is
        # ambiguous" for non LAL), and for integers (e.g., "number of
        # deterministic states" for non LAL).
        try:
            # Don't convert "0" and "1", which are used for false and
            # true, as integer: special case categories that start
            # with "is ".
            if k.startswith('is '):
                if v in ['false', '0']:
                    v = False
                elif v in ['true', '1']:
                    v = True
            # Otherwise, if it passes the conversion into a number,
            # make it a number.
            else:
                v = int(v)
        except:
            pass
        res[k] = v
    return res

# FIXME: Get rid of this.
def _is_equal(lhs, rhs):
    "A stupid string-based comparison.  Must be eliminated once we DRT."
    return isinstance(rhs, lhs.__class__) and str(lhs) == str(rhs)

def _left_mult(self, lhs):
    """Support "aut * weight".  Also serves for ratexps."""
    return self.left_mult(self.context().weight(str(lhs)))

def _right_mult(self, rhs):
    """Support both "aut * aut" and "aut * weight".  Also serves for
    ratexps."""
    if isinstance(rhs, type(self)):
        return self.concatenate(rhs)
    elif isinstance(rhs, weight):
        return self.right_mult(rhs)
    else:
        return self.right_mult(self.context().weight(str(rhs)))



from vcsn.automaton  import *
from vcsn.context    import *
from vcsn.expansion  import *
from vcsn.label      import *
from vcsn.polynomial import *
from vcsn.ratexp     import *
from vcsn.weight     import *
