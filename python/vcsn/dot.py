# -*- coding: utf-8 -*-

# FIXME: Many re.sub/string.replace should probably be replaced by
# uses of gvpr.
#
# It looks like we could pass -E/-N option to dot to set defaults
# at the last moment, unfortutately in that case, it takes
# precedence on "edge" and "node" attributes defined in the file
# itself, which would, for instance, discard the shape=circle
# attribute.

from functools import lru_cache
import re
from subprocess import PIPE, Popen, check_call

from vcsn.tools import _tmp_file

## Style of states in dot.
# Default style for real states as issued by vcsn::dot.
state_style = 'node [shape = circle, style = rounded, width = 0.5]'
# IPython style for real states.
state_pretty = ('node [fontsize = 12, fillcolor = cadetblue1, '
                'shape = circle, style = "filled,rounded", height = 0.4, '
                'width = 0.4, fixedsize = true]')
# Style for state in `simple` mode.
state_simple = ('node [fillcolor = cadetblue1, shape = circle, '
                'style = "filled,rounded", width = 0.3]')
# Style for pre and post states, or when rendering transitions only.
state_point = 'node [shape = point, width = 0]'

# Style of transitions in dot.
edge_style = 'edge [arrowhead = vee, arrowsize = .6]'

# Process timeout (in seconds).
timeout = 60

def _states_as_tooltips(s):
    s = re.sub(r'label = (".*?"), shape = box', r'tooltip = \1', s)
    return _states_as_pretty(s)


def _states_as_simple(s):
    'Make all the states simple circles, put its label in its tooltip.'
    s = s.replace(state_style, state_simple)
    # A non-decorated state.
    s = re.sub(r'^( *)([0-9]+)$',
               r'\1\2 [label = "", tooltip = "\2"]',
               s, flags=re.MULTILINE)
    # A decorated state.
    s = re.sub(r'^(\ *([0-9]+)\ *\[.*?label\ *=\ *)"(.*?)", shape = box',
               r'\1"", tooltip = "\2: \3"',
               s, flags=re.MULTILINE)
    return s


def _states_as_points(s):
    '''Transform all the nodes into simple points, as to reveal only
    the transitions.'''
    return (s.replace(state_style, state_point)
            .replace(', shape = box', ''))


def _states_as_pretty(s):
    '''Transform all the nodes into colored points.'''
    return (s.replace(state_style, state_pretty)
            .replace('shape = box', 'shape = box, fixedsize = false'))


def _dot_gray_node(m):
    '''Replace gray node contours by gray nodes, and apply style to
    nodes with their own style.'''
    node = m.group(1)
    attr = m.group(2)
    if ' -> ' not in node:
        attr = attr.replace('color = DimGray', 'fillcolor = lightgray')
        attr = re.sub(r'style = (\w+)', r'style = "\1"', attr)
        attr = re.sub(r'style = "(.+?)"',
                      r'style = "\1,filled,rounded"', attr)
        # This is really ugly...  We should definitely use gvpr.
        attr = re.sub(r'(?:filled,)?rounded,filled,rounded',
                      r'filled,rounded', attr)
    return node + attr


def _dot_pretty(s, mode="pretty"):
    '''
    Improve pretty-printing in a dot source.

    Use some HTML entities instead of ugly ASCII sequences.
    Use nicer arrows.

    If `mode` is `tooltip`, convert node labels to tooltips.
    If it is `transitions`, then hide the states.
    '''
    if mode != 'dot':
        if mode == 'simple':
            s = _states_as_simple(s)
        elif mode == 'tooltip':
            s = _states_as_tooltips(s)
        elif mode == 'transitions':
            s = _states_as_points(s)
        else: # mode == 'pretty'
            s = _states_as_pretty(s)
        # Useless states should be filled in gray, instead of having a
        # gray contour.  Fill with a lighter gray.  But don't change the
        # color of the arrows.
        s = re.sub(r'^(.*)(\[.*?\])$', _dot_gray_node, s, flags=re.MULTILINE)
    return s


@lru_cache(maxsize=32)
def _dot_to_boxart(dot):
    dot = dot.replace('digraph', 'digraph a')
    p = Popen(['/opt/local/libexec/perl5.16/sitebin/graph-easy',
               '--from=graphviz', '--as=boxart'],
              stdin=PIPE, stdout=PIPE, stderr=PIPE,
              universal_newlines=True)
    p.stdin.write(dot)
    out, err = p.communicate(timeout=timeout)
    if p.wait():
        raise RuntimeError('graph-easy failed: ' + err)
    return out


@lru_cache(maxsize=32)
def _dot_to_svg(dot, engine='dot', *args):
    "The conversion of a Dot source into SVG by dot."
    # http://www.graphviz.org/content/rendering-automata.
    #
    # This routine used to be composed of a pipe between these three
    # processes.  But it failed to end on large input.  On a typical
    # use case (ladybird 10), we get no significant difference with
    # %timeit: 1.37s before, 1.39s after.
    p = Popen([engine] + list(args),
               stdin=PIPE, stdout=PIPE, stderr=PIPE,
               universal_newlines=True)
    res, err = p.communicate(dot, timeout=timeout)
    if p.wait():
        raise RuntimeError("{} failed: {}", format(engine, err))

    p = Popen(['gvpr', '-c', 'E[head.name == "F*" && head.name != "Fpre"]{lp=pos=""}'],
              stdin=PIPE, stdout=PIPE, stderr=PIPE,
              universal_newlines=True)
    res, err = p.communicate(res, timeout=timeout)
    if p.wait():
        raise RuntimeError("{} failed: {}", format('gvpr', err))

    p = Popen(['neato', '-n2', '-Tsvg'],
              stdin=PIPE, stdout=PIPE, stderr=PIPE,
              universal_newlines=True)
    res, err = p.communicate(res, timeout=timeout)
    if p.wait():
        raise RuntimeError("{} failed: {}", format('neato', err))

    return res


@lru_cache(maxsize=32)
def _dot_to_svg_dot2tex(dot, engine="dot"):
    '''The conversion of a Dot source into SVG by dot2tex.

    Requires dot2tex, texi2pdf and pdf2svg.
    '''
    with _tmp_file('tex') as tex, \
            _tmp_file('pdf') as pdf, \
            _tmp_file('svg') as svg:
        p1 = Popen(['dot2tex', '--prog', engine],
                   stdin=PIPE, stdout=tex, stderr=PIPE,
                   universal_newlines=True)
        _, err = p1.communicate(dot, timeout=timeout)
        if p1.wait():
            raise RuntimeError("dot2tex failed: " + err)
        check_call(["texi2pdf", "--batch", "--clean", "--quiet",
                    "--output", pdf.name, tex.name], timeout=timeout)
        check_call(["pdf2svg", pdf.name, svg.name], timeout=timeout)
        res = open(svg.name).read()
        return res

def daut_to_transitions(s):
    '''From a Daut, return the list of transitions as triples
    `(src, dst, entry)`, using `$` to denote pre/post states.'''
    re_id = r'(?:\w+|"(?:[^\\"]|\\.)*")'
    # An RE than matches transitions in Daut syntax.
    re_tr = r'^ *({id}|\$)? *-> *({id}|\$)? *(.*?)$'.format(id=re_id)
    return re.findall(re_tr, s, flags=re.MULTILINE)
