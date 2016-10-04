## ----------- ##
## automaton.  ##
## ----------- ##

import re
from subprocess import Popen, PIPE

from vcsn_cxx import automaton, label, polynomial
from vcsn.proxy import variadicProxy
from vcsn.tools import (_extend, _format, _info_to_dict,
                        _lweight, _rweight,
                        _tmp_file)
from vcsn.dot import (_dot_pretty, _dot_to_boxart, _dot_to_svg,
                      _dot_to_svg_dot2tex)


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


def _guess_format(data=None, filename=None):
    '''Try to find out what is the format used to encode this automaton.'''
    for line in open(filename) if filename else data.splitlines():
        if re.match(r'^\s*digraph', line):
            return 'dot'
        elif re.match('context *=', line) \
                or re.match(r'^\s*(\$|\w+|".*?")\s*->\s*(\$|\w+|".*?")', line):
            return 'daut'
        elif line.startswith('#! /bin/sh'):
            return 'efsm'
        elif line.startswith('(START)'):
            return 'grail'
        elif re.match('^@([DN]FA|Transducer) ', line):
            return 'fado'
    raise RuntimeError('cannot guess automaton format: ',
                       data if data else filename)

@variadicProxy('__and__')
class Conjunction:
    '''A proxy class to delay calls to the & operator in order to turn
    a & b & c into a variadic evaluation of vcsn.automaton.conjunction(a, b, c).
    '''
    def __and__(self, arg):
        if isinstance(arg, int):
            object.__getattribute__(self, "_proxy_vars")[-1] =  \
                object.__getattribute__(self, "_proxy_vars")[-1].conjunction(arg)
        else:
            object.__getattribute__(self, "_proxy_vars").append(arg)
        return self

    def __value__(self):
        aut = object.__getattribute__(self, "_proxy_vars")
        if isinstance(aut, list):
            if len(aut) == 1:
                aut = aut[0]
            else:
                aut = aut[0].conjunction(*(aut[1:]), lazy=False)
            object.__setattr__(self, "_proxy_vars", aut)
        return aut


@_extend(automaton)
class automaton:
    _multiply_orig = automaton.multiply

    def multiply(self, exp, algo="auto"):
        if isinstance(exp, tuple):
            return self._multiply_orig(*exp, algo=algo)
        else:
            return self._multiply_orig(exp, algo=algo)

    __add__ = automaton.add
    __and__ = lambda l, r: Conjunction(l, r)
    __eq__ = lambda self, other: str(self.strip()) == str(other.strip())
    __invert__ = automaton.complement
    __mod__ = automaton.difference
    __matmul__ = automaton.compose
    __mul__ = _rweight
    __or__ = lambda l, r: automaton._tuple([l, r])
    __pow__ = multiply
    __repr__ = lambda self: self.type()
    __rmul__ = _lweight
    __str__ = lambda self: self.format('dot')
    __sub__ = automaton.difference
    __truediv__ = automaton.rdivide
    _repr_svg_ = lambda self: self.SVG()

    as_boxart = lambda self: _dot_to_boxart(self.dot())

    # conjunction.
    _conjunction_orig = automaton.conjunction

    def conjunction(*args, lazy=False):
        '''Compute the conjunction of automata, possibly lazy, or the repeated
        conjunction of an automaton.'''
        if len(args) == 2 and isinstance(args[1], int):
            return automaton._conjunction_orig(*args)
        else:
            return automaton._conjunction_orig(list(args), lazy)

    def _convert(self, mode, engine="dot"):
        '''Display automaton `self` in `mode` with Graphviz `engine`.'''
        from IPython import display
        if mode in ['dot', 'pretty', 'simple', 'tooltip', 'transitions']:
            svg = _dot_to_svg(self.dot(mode), engine)
            return display.SVG(svg)
        elif mode == 'dot2tex':
            return display.SVG(self.SVG(mode, engine))
        elif mode == 'info':
            return self.info(details=2)
        elif mode == 'info,detailed':
            return self.info(details=3)
        elif mode == 'info,size':
            return self.info(details=1)
        elif mode == 'type':
            return repr(self)
        else:
            raise ValueError('invalid display format: ' + mode)

    _determinize_orig = automaton.determinize

    def determinize(self, algo="auto", lazy=False):
        if lazy:
            algo = 'lazy,' + algo
        return self._determinize_orig(algo)

    def _display(self, mode, engine="dot"):
        '''Display automaton `self` in `mode` with Graphviz `engine`.'''
        from IPython.display import display
        display(self._convert(mode, engine))

    # automaton.__init__
    # The point is to add support for the format guessing.  So we save
    # the original, C++ based, implementation of __init__ as _init,
    # and then provide a new __init__.
    _init_orig = automaton.__init__

    def __init__(self, data='', format='auto', filename='',
                 strip=True):
        if format == "auto":
            format = _guess_format(data, filename)
        self._init_orig(data=data, format=format, filename=filename,
                        strip=strip)

    def display(self):
        '''Display automaton `self` with a local menu to the select
        the display mode.  Pay attention to not displaying large
        automata by default.
        '''
        from vcsn.ipython import interact_h
        modes = automaton.display.modes
        engines = automaton.display.engines
        if 20 < self.state_number():
            # Put 'info,size' first, to be the default.
            modes.remove('info,size')
            modes.insert(0, 'info,size')
        interact_h(lambda mode, engine: self._display(mode, engine),
                   mode=modes, engine=engines)

    display.modes = ['simple', 'pretty', 'info,size', 'info',
                     'info,detailed', 'tooltip',
                     'transitions', 'type', 'dot', 'dot2tex']
    display.engines = ['dot', 'neato', 'twopi', 'circo', 'fdp', 'sfdp',
                       'patchwork']

    def dot(self, mode="pretty"):
        return _dot_pretty(self.format('dot,utf8'), mode)

    # automaton.eval.
    def eval(self, w):
        '''Evaluation of word (or polynomial) `w` on `self`, with possible
        conversion from plain string to genuine label object.
        '''
        c = self.context()
        if not isinstance(w, label) and not isinstance(w, polynomial):
            w = c.word(str(w))
        return self._eval(w)
    __call__ = eval

    # automaton.format
    def format(self, fmt="daut"):
        return self._format(fmt)

    def __format__(self, spec):
        """Format the automaton according to `spec`.

        Parameters
        ----------
        spec : str, optional
            a list of letters that specify how the automaton
            should be formatted.

        Supported specifiers
        --------------------

        - 'd': use Daut syntax (default)
        - 'D': show implementation details for debugging
        - 'e': use EFSM syntax
        - 'f': use FAdo syntax
        - 'g': use Graphviz's Dot syntax
        - 'i': print info
        - 'I': print detailed info
        - 'r': use Grail syntax
        - 'x': use TikZ syntax

        - ':spec': pass the remaining specification to the
                   formating function for strings.

        """

        syntaxes = {'d': 'daut',
                    'D': 'debug',
                    'e': 'efsm',
                    'f': 'fado',
                    'g': 'dot',
                    'i': 'info',
                    'I': 'info,detailed',
                    'r': 'grail',
                    'x': 'tikz'}
        return _format(self, spec, 'daut', syntaxes)

    def as_fst(self):
        '''Return an OpenFST binary file for `self`.  When the result is
        discarded, the file might be removed, so to keep the file alive,
        keep the result alive, not just the result's name.
        '''
        fst = _tmp_file(suffix='fst')
        proc = Popen(['efstcompile'],
                     stdin=PIPE, stdout=fst, stderr=PIPE)
        proc.stdin.write(self.format('efsm').encode('utf-8'))
        _, err = proc.communicate()
        if proc.wait():
            raise RuntimeError("efstcompile failed: " + err.decode('utf-8'))
        return fst

    fstcat           = lambda self: _automaton_fst("cat", self)
    fstcompose       = lambda a, b: _automaton_fst_files("fstcompose", a, b)
    fstconjunction   = lambda a, b: _automaton_fst_files("fstintersect", a, b)
    fstdeterminize   = lambda self: _automaton_fst("fstdeterminize", self)
    fstis_equal      = lambda a, b: _automaton_fst_files("fstequal", a, b)
    fstis_equivalent = lambda a, b: _automaton_fst_files("fstequivalent", a, b)
    fstminimize      = lambda self: _automaton_fst("fstminimize", self)
    fstproper        = lambda self: _automaton_fst("fstrmepsilon", self)
    fstlightestpath  = lambda self: _automaton_fst("fstshortestpath", self)
    fstsynchronize   = lambda self: _automaton_fst("fstsynchronize", self)
    fsttranspose     = lambda self: _automaton_fst("fstreverse", self)

    def HTML(self):
        """Display `self` with SVG and MathJax together."""
        svg = _dot_to_svg(_dot_pretty(self.format('dot,mathjax'), 'pretty'))
        # Any <text> that contains at least 2 $s.
        svg = re.sub(r'<text ([^>]*)>(.*?\$.*?\$.*?)</text>',
                     r'''<foreignObject class="automaton_raw" height="24" \1>
  <body xmlns="http://www.w3.org/1999/xhtml">
    <div style="text-align: center;">
      \2
    </div>
  </body>
</foreignObject>''', svg)
        # Replace matching $s with matching \(\). Do not match \$.
        svg = re.sub(r'(?<!\\)\$(.*?)\$', r'\(\1\)', svg)
        html = r'''<!DOCTYPE html>
<html>
  <head>
    <title>MathJax in SVG diagram</title>
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.0.0/jquery.min.js"></script>
    <script src='https://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML'></script>
    <meta charset="UTF-8">
  </head>
  <body>
    {svg}
    <script>
      $('.automaton_raw').each(function(){{
        var width = $(this).parent()[0].getBoundingClientRect().width;
        var x = this.getAttribute('x');
        var y = this.getAttribute('y');

        width = parseFloat(width)
        x = parseFloat(x)
        y = parseFloat(y)

        this.setAttribute('width', width);
        this.setAttribute('x', x - width / 2);
        this.setAttribute('y', y - 14);

        $(this).removeClass('automaton_raw')
      }});
    </script>
  </body>
</html>'''.format(svg=svg)
        return html

    infiltrate = lambda *auts: automaton._infiltrate(list(auts))

    def info(self, key=None, details=2):
        formats = ['info,size', 'info', 'info,detailed']
        details = max(1, min(3, details))
        res = _info_to_dict(self.format(formats[details - 1]))
        return res[key] if key else res

    def is_synchronized_by(self, w):
        c = self.context()
        if not isinstance(w, label):
            w = c.word(str(w))
        return self._is_synchronized_by(w)

    def lift(self, *tapes, identities="default"):
        if len(tapes) == 1 and isinstance(tapes[0], list):
            tapes = tapes[0]
        else:
            tapes = list(tapes)
        return self._lift(tapes, identities)

    _proper_orig = automaton.proper

    def proper(self, direction="backward", prune=True, algo="auto", lazy=False):
        if lazy:
            if algo not in [None, 'auto']:
                raise RuntimeError('proper: algo and lazy are incompatible')
            algo = 'lazy'
        return self._proper_orig(direction=direction, prune=prune, algo=algo)

    shuffle = lambda *auts: automaton._shuffle(list(auts))

    state_number = lambda self: self.info('number of states')

    def SVG(self, format="dot", engine="dot"):
        if format == "dot":
            return _dot_to_svg(self.dot(), engine)
        elif format == "dot2tex":
            return _dot_to_svg_dot2tex(self.format("dot,latex"), engine)
        else:
            raise ValueError("invalid format: ", format)
