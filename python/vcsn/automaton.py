## ----------- ##
## automaton.  ##
## ----------- ##

import tempfile
import os
import re
import subprocess
from subprocess import Popen, PIPE

from vcsn.conjunction import Conjunction
from vcsn_cxx import automaton, label, weight
from vcsn.tools import _info_to_dict, _left_mult, _right_mult, _tmp_file
from vcsn.dot import (_dot_pretty, _dot_to_boxart, _dot_to_svg,
                      _dot_to_svg_dot2tex, dot_to_daut, daut_to_dot)

_automaton_multiply_orig = automaton.multiply


def _automaton_multiply(self, exp, algo="auto"):
    if isinstance(exp, tuple):
        return _automaton_multiply_orig(self, *exp, algo=algo)
    else:
        return _automaton_multiply_orig(self, exp, algo)
automaton.multiply = _automaton_multiply

automaton.__add__ = automaton.sum
automaton.__and__ = lambda l, r: Conjunction(l, r)
automaton.__eq__ = lambda self, other: str(self.strip()) == str(other.strip())
automaton.__invert__ = automaton.complement
automaton.__mod__ = automaton.difference
automaton.__mul__ = _right_mult
automaton.__pow__ = automaton.multiply
automaton.__repr__ = lambda self: self.type()
automaton.__rmul__ = _left_mult
automaton.__str__ = lambda self: self.format('dot')
automaton.__sub__ = automaton.difference
automaton._repr_svg_ = lambda self: self.as_svg()

automaton.as_boxart = lambda self: _dot_to_boxart(self.dot())


def _automaton_as_svg(self, format="dot", engine="dot"):
    if format == "dot":
        return _dot_to_svg(self.dot(), engine)
    elif format == "dot2tex":
        return _dot_to_svg_dot2tex(self.format("dot,latex"), engine)
    else:
        raise(ValueError("invalid format: ", format))

automaton.as_svg = _automaton_as_svg


# conjunction.
_automaton_conjunction_orig = automaton.conjunction
def _automaton_conjunction(*args, lazy=False):
    '''Compute the conjunction of automata, possibly lazy, or the repeated
    conjunction of an automaton.'''
    if len(args) == 2 and isinstance(args[1], int):
        return _automaton_conjunction_orig(*args)
    else:
        return _automaton_conjunction_orig(list(args), lazy)
automaton.conjunction = _automaton_conjunction


def _automaton_convert(self, mode, engine="dot"):
    '''Display automaton `self` in `mode` with Graphviz `engine`.'''
    from IPython.display import SVG
    if mode in ['dot', 'pretty', 'simple', 'tooltip', 'transitions']:
        svg = _dot_to_svg(self.dot(mode), engine)
        return SVG(svg)
    elif mode == 'dot2tex':
        return SVG(self.as_svg(mode, engine))
    elif mode == 'info':
        return self.info(detailed=False)
    elif mode == 'info,detailed':
        return self.info(detailed=True)
    elif mode == 'type':
        return repr(self)
    else:
        raise(ValueError('invalid display format: ' + mode))


_automaton_determinize_orig = automaton.determinize
def _automaton_determinize(self, algo="auto",
                           lazy=False):
    if lazy:
        algo = 'lazy,' + algo
    return _automaton_determinize_orig(self, algo)
automaton.determinize = _automaton_determinize


def _automaton_display(self, mode, engine="dot"):
    '''Display automaton `self` in `mode` with Graphviz `engine`.'''
    from IPython.display import display
    display(_automaton_convert(self, mode, engine))


def _guess_format(data='', filename=''):
    '''Try to find out what is the format used to encode this automaton.'''
    for line in open(filename) if filename else data.splitlines():
        if line.startswith('digraph'):
            return 'dot'
        elif re.match('context *=', line) \
             or re.match('^\s*(\$|\w+|".*?")\s*->\s*(\$|\w+|".*?")', line):
            return 'daut'
        elif line.startswith('#! /bin/sh'):
            return 'efsm'
        elif line.startswith('(START)'):
            return 'grail'
        elif re.match('^@[DN]FA ', line):
            return 'fado'
    raise RuntimeError('cannot guess automaton format')

# automaton.__init__
# The point is to add support for the "daut" format.  So we save
# the original, C++ based, implementation of __init__ as _init,
# and then provide a new __init__.
automaton._init = automaton.__init__


def _automaton_init(self, data='', format='auto', filename='',
                    strip=True):
    if format == "auto":
        format = _guess_format(data, filename)
    if format == "daut":
        if filename:
            data = open(filename).read()
            filename = ''
        data = daut_to_dot(data)
        format = 'dot'
    self._init(data=data, format=format, filename=filename,
               strip=strip)
automaton.__init__ = _automaton_init


def _automaton_interact(self):
    '''Display automaton `self` with a local menu to the select
    the display mode.  Pay attention to not displaying large
    automata by default.
    '''
    from vcsn.ipython import interact_h
    if 20 < self.state_number():
        modes = ['info', 'simple', 'pretty']
    else:
        modes = ['simple', 'pretty', 'info']
    modes += ['info,detailed', 'tooltip', 'transitions', 'type',
              'dot', 'dot2tex']
    engines = ['dot', 'neato', 'twopi', 'circo', 'fdp', 'sfdp', 'patchwork']
    interact_h(lambda mode, engine: _automaton_display(self, mode, engine),
               mode=modes, engine=engines)
automaton.display = _automaton_interact

automaton.dot = lambda self, mode = "pretty": _dot_pretty(
    self.format('dot,utf8'), mode)


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
def _automaton_format(self, fmt="daut"):
    if fmt == "daut":
        return dot_to_daut(self._format('dot'))
    else:
        return self._format(fmt)
automaton.format = _automaton_format


def _automaton_fst(cmd, aut):
    '''Run the command `cmd` on the automaton `aut` coded in OpenFST
    format via pipes.
    '''
    p1 = Popen(['efstcompile'],   stdin=PIPE,      stdout=PIPE, stderr=PIPE)
    p2 = Popen(cmd,               stdin=p1.stdout, stdout=PIPE, stderr=PIPE)
    p3 = Popen(['efstdecompile'], stdin=p2.stdout, stdout=PIPE, stderr=PIPE)
    p1.stdout.close()  # Allow p1 to receive a SIGPIPE if p2 exits.
    p2.stdout.close()  # Allow p2 to receive a SIGPIPE if p3 exits.
    p1.stdin.write(aut.format('efsm').encode('utf-8'))
    p1.stdin.close()
    res, err = p3.communicate()
    if p1.wait():
        raise RuntimeError(
            "efstcompile failed: " + p1.stderr.read().decode('utf-8'))
    if p2.wait():
        raise RuntimeError(
            cmd + " failed: " + p2.stderr.read().decode('utf-8'))
    if p3.wait():
        raise RuntimeError("efstdecompile failed: " + err.decode('utf-8'))
    return automaton(res.decode('utf-8'), 'efsm')


def _automaton_as_fst(self):
    '''Return an OpenFST binary file for `self`.  When the result is
    discarded, the file might be removed, so to keep the file alive,
    keep the result alive, not just the result's name.
    '''
    fst = _tmp_file(suffix='fst')
    proc = Popen(['efstcompile'],
                 stdin=PIPE, stdout=fst, stderr=PIPE)
    proc.stdin.write(self.format('efsm').encode('utf-8'))
    out, err = proc.communicate()
    if proc.wait():
        raise RuntimeError("efstcompile failed: " + err.decode('utf-8'))
    return fst
automaton.as_fst = _automaton_as_fst


def _automaton_fst_files(cmd, *aut):
    '''Run the command `cmd` on the automata `aut` coded in OpenFST
    format, via files.
    '''
    files = [a.as_fst() for a in aut]
    proc = Popen([cmd] + [f.name for f in files],
                 stdin=PIPE, stdout=PIPE, stderr=PIPE)
    decode = Popen(['efstdecompile'],
                   stdin=proc.stdout, stdout=PIPE, stderr=PIPE)
    res, err = decode.communicate()
    if proc.wait():
        raise RuntimeError(
            cmd + " failed: " + proc.stderr.read().decode('utf-8'))
    if decode.wait():
        raise RuntimeError("efstdecompile failed: " + err.decode('utf-8'))
    return automaton(res.decode('utf-8'), 'efsm')

automaton.fstcat           = lambda self: _automaton_fst("cat", self)
automaton.fstcompose       = lambda a, b: _automaton_fst_files("fstcompose", a, b)
automaton.fstconjunction   = lambda a, b: _automaton_fst_files("fstintersect", a, b)
automaton.fstdeterminize   = lambda self: _automaton_fst("fstdeterminize", self)
automaton.fstis_equal      = lambda a, b: _automaton_fst("fstequal", a, b)
automaton.fstis_equivalent = lambda a, b: _automaton_fst("fstequivalent", a, b)
automaton.fstminimize      = lambda self: _automaton_fst("fstminimize", self)
automaton.fstproper        = lambda self: _automaton_fst("fstrmepsilon", self)
automaton.fstsynchronize   = lambda self: _automaton_fst("fstsynchronize", self)
automaton.fsttranspose     = lambda self: _automaton_fst("fstreverse", self)

automaton.infiltration = lambda *auts: automaton._infiltration(list(auts))


def _automaton_info(self, key=None, detailed=False):
    res = _info_to_dict(self.format('info,detailed' if detailed else 'info'))
    return res[key] if key else res
automaton.info = _automaton_info


def _automaton_is_synchronized_by(self, w):
    c = self.context()
    if not isinstance(w, label):
        w = c.word(str(w))
    return self._is_synchronized_by(w)
automaton.is_synchronized_by = _automaton_is_synchronized_by


def _automaton_lift(self, *tapes, identities="default"):
    if len(tapes) == 1 and isinstance(tapes[0], list):
        tapes = tapes[0]
    else:
        tapes = list(tapes)
    return self._lift(tapes, identities)
automaton.lift = _automaton_lift

_automaton_proper_orig = automaton.proper
def _automaton_proper(self, prune=True, backward=True, algo="auto", lazy=False):
    if lazy:
        if algo not in [None, 'auto']:
            raise RuntimeError('proper: algo and lazy are incompatible')
        algo = 'lazy'
    return _automaton_proper_orig(self, prune, backward, algo)
automaton.proper = _automaton_proper

automaton.shuffle = lambda *auts: automaton._shuffle(list(auts))

automaton.state_number = lambda self: self.info('number of states')
