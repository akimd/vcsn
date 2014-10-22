## ----------- ##
## automaton.  ##
## ----------- ##

from __future__ import print_function

import tempfile
import os
import re
import subprocess

from vcsn.conjunction import Conjunction
from vcsn_cxx import automaton, label, weight
from vcsn import _info_to_dict, _left_mult, _right_mult, _tmp_file
from vcsn.dot import _dot_pretty, _dot_to_svg, _dot_to_svg_dot2tex, from_dot, to_dot

automaton.__add__ = automaton.sum
automaton.__and__ = lambda l, r: Conjunction(l, r)
automaton.__eq__ = lambda self, other: str(self.strip()) == str(other.strip())
automaton.__invert__ = automaton.complement
automaton.__mod__ = automaton.difference
automaton.__mul__ = _right_mult
automaton.__or__ = automaton.union
automaton.__pow__ = automaton.power
automaton.__repr__ = lambda self: self.info()['type']
automaton.__rmul__ = _left_mult
automaton.__str__ = lambda self: self.format('dot')
automaton.__sub__ = automaton.difference
automaton._repr_svg_ = lambda self: self.as_svg()

def _automaton_as_svg(self, format = "dot", engine = "dot"):
    if format == "dot":
        return _dot_to_svg(self.dot(), engine)
    elif format == "dot2tex":
        return _dot_to_svg_dot2tex(self.format("dot2tex"), engine)
    else:
        raise(ValueError("invalid format: ", format))

automaton.as_svg = _automaton_as_svg

def _automaton_convert(self, mode, engine = "dot"):
    '''Display automaton `self` in `mode` with Graphviz `engine`.'''
    from IPython.display import SVG
    if mode in ["dot", "tooltip", "transitions"]:
        svg = _dot_to_svg(self.dot(mode), engine)
        return SVG(svg)
    elif mode == "dot2tex":
        return SVG(self.as_svg(mode, engine))
    elif mode == "info":
        return self.info(False)
    elif mode == "info,detailed":
        return self.info(True)
    elif mode == "type":
        return repr(self)
    else:
        raise(ValueError("invalid display format: " + mode))

def _automaton_display(self, mode, engine = "dot"):
    '''Display automaton `self` in `mode` with Graphviz `engine`.'''
    from IPython.display import display
    display(_automaton_convert(self, mode, engine))

# automaton.__init__
# The point is to add support for the "daut" format.  So we save
# the original, C++ based, implementation of __init__ as _init,
# and then provide a new __init__.
automaton._init = automaton.__init__
def _automaton_init(self, data = '', format = '', filename = ''):
    if format == "daut":
        if filename:
            data = open(filename).read()
        data = to_dot(data)
        self._init(data = data, format = 'dot')
    else:
        self._init(data = data, format = format, filename = filename)
automaton.__init__ = _automaton_init

def _automaton_interact(self):
    '''Display automaton `self` with a local menu to the select
    the display mode.  Pay attention to not displaying large
    automata by default.
    '''
    from ipython import interact_h
    if 20 < self.state_number():
        modes = ['info', 'dot']
    else:
        modes = ['dot', 'info']
    modes += ['info,detailed', 'tooltip', 'transitions', 'type', 'dot2tex']
    engines = ['dot', 'neato', 'twopi', 'circo', 'fdp', 'sfdp', 'patchwork']
    interact_h(lambda mode, engine: _automaton_display(self, mode, engine),
               mode = modes, engine = engines)
automaton.display = _automaton_interact

automaton.dot = lambda self, mode = "dot": _dot_pretty(self.format('dot'), mode)

# automaton.eval.
def _automaton_eval(self, w):
    '''Evaluation of word `w` on `self`, with possible conversion from
    plain string to genuine label object.
    '''
    c = self.context()
    if not isinstance(w, label):
        w = c.word(str(w))
    return self._eval(w)
automaton.__call__ = _automaton_eval
automaton.eval = _automaton_eval

# automaton.format
def _automaton_format(self, fmt = "daut"):
    if fmt == "daut":
        return from_dot(self._format('dot'))
    else:
        return self._format(fmt)
automaton.format = _automaton_format

def _automaton_fst(cmd, aut):
    '''Run the command `cmd` on the automaton `aut` coded in OpenFST
    format via pipes.
    '''
    from subprocess import Popen, PIPE
    p1 = Popen(['efstcompile'],   stdin=PIPE,      stdout=PIPE, stderr=PIPE)
    p2 = Popen(cmd,               stdin=p1.stdout, stdout=PIPE, stderr=PIPE)
    p3 = Popen(['efstdecompile'], stdin=p2.stdout, stdout=PIPE, stderr=PIPE)
    p1.stdout.close()  # Allow p1 to receive a SIGPIPE if p2 exits.
    p2.stdout.close()  # Allow p2 to receive a SIGPIPE if p3 exits.
    p1.stdin.write(aut.format('efsm'))
    p1.stdin.close()
    res, err = p3.communicate()
    if p1.wait():
        raise RuntimeError("efstcompile failed: " + p1.stderr.read().decode('utf-8'))
    if p2.wait():
        raise RuntimeError(" ".join(cmd) + " failed: " + p2.stderr.read().decode('utf-8'))
    if p3.wait():
        raise RuntimeError("efstdecompile failed: " + err.decode('utf-8'))
    return automaton(res, "efsm")

def _automaton_fst_files(cmd, *aut):
    '''Run the command `cmd` on the automata `aut` coded in OpenFST
    format, via files.
    '''
    from subprocess import Popen, PIPE
    files = []
    for a in aut:
        fst = _tmp_file(suffix='fst')
        proc = Popen(['efstcompile'],
                     stdin=PIPE, stdout=fst, stderr=PIPE)
        proc.stdin.write(a.format('efsm'))
        out, err = proc.communicate()
        if proc.wait():
            raise RuntimeError("efstcompile failed: " + err.decode('utf-8'))
        files.append(fst)

    proc = Popen([cmd] + [f.name for f in files],
                 stdin=PIPE, stdout=PIPE, stderr=PIPE)
    decode = Popen(['efstdecompile'],
                   stdin=proc.stdout, stdout=PIPE, stderr=PIPE)
    res, err = decode.communicate()
    if proc.wait():
        raise RuntimeError(" ".join(cmd) + " failed: " + proc.stderr.read().decode('utf-8'))
    if decode.wait():
        raise RuntimeError("efstdecompile failed: " + err.decode('utf-8'))
    return automaton(res, "efsm")

automaton.fstdeterminize = lambda self: _automaton_fst("fstdeterminize", self)
automaton.fstintersect = lambda a, b: _automaton_fst_files("fstintersect", a, b)
automaton.fstminimize = lambda self: _automaton_fst("fstminimize", self)

automaton.infiltration = lambda *auts: automaton._infiltration(list(auts))

automaton.info = lambda self, detailed = False: \
  _info_to_dict(self.format('info,detailed' if detailed else 'info'))

def _automaton_is_synchronized_by(self, w):
    c = self.context()
    if not isinstance(w, label):
        w = c.word(str(w))
    return self._is_synchronized_by(w)
automaton.is_synchronized_by = _automaton_is_synchronized_by

automaton.shuffle = lambda *auts: automaton._shuffle(list(auts))

automaton.state_number = lambda self: self.info(False)['number of states']
