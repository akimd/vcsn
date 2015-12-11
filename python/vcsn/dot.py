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
import locale
import re
from subprocess import PIPE, Popen, check_call
import sys

from vcsn.tools import _tmp_file

## Style of states in dot.
# Default style for real states as issued by vcsn::dot.
state_style = 'node [shape = circle, style = rounded, width = 0.5]'
# IPython style for real states.
state_pretty = 'node [fontsize = 12, fillcolor = cadetblue1, shape = circle, style = "filled,rounded", height = 0.4, width = 0.4, fixedsize = true]'
# Style for state in `simple` mode.
state_simple = 'node [fillcolor = cadetblue1, shape = circle, style = "filled,rounded", width = 0.3]'
# Style for pre and post states, or when rendering transitions only.
state_point = 'node [shape = point, width = 0]'

# Style of transitions in dot.
edge_style = 'edge [arrowhead = vee, arrowsize = .6]'

def _label_pretty(s):
    '''Convert angle brackets and \\e to UTF-8.  We used to use
    HTML entities, but it resulted in wider arrows: dot is probably
    counting the number of characters to compute the width, instead
    of the real width of the glyphs.
    '''
    return (s.replace('<', '⟨')
            .replace('>', '⟩')
            .replace(r'\\e', 'ε')
            .replace(r'\\z', '∅')
            .replace(r'{c}', 'ᶜ'))


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
        s = re.sub(r'(label * = *)(".*?")',
                   lambda m: m.group(1) + _label_pretty(m.group(2)),
                   s)
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
        s = re.sub('^(.*)(\[.*?\])$', _dot_gray_node, s, flags=re.MULTILINE)
    return s


@lru_cache(maxsize=32)
def _dot_to_boxart(dot):
    dot = dot.replace('digraph', 'digraph a')
    p = Popen(['/opt/local/libexec/perl5.16/sitebin/graph-easy',
               '--from=graphviz', '--as=boxart'],
              stdin=PIPE, stdout=PIPE, stderr=PIPE,
              universal_newlines=True)
    p.stdin.write(dot)
    out, err = p.communicate()
    if p.wait():
        raise RuntimeError('graph-easy failed: ' + err)
    if isinstance(out, bytes):
        out = out.decode('utf-8')
    return out


@lru_cache(maxsize=32)
def _dot_to_svg(dot, engine='dot', *args):
    "The conversion of a Dot source into SVG by dot."
    # http://www.graphviz.org/content/rendering-automata
    p1 = Popen([engine] + list(args),
               stdin=PIPE, stdout=PIPE, stderr=PIPE, universal_newlines=True)
    p2 = Popen(['gvpr', '-c', 'E[head.name == "F*" && head.name != "Fpre"]{lp=pos=""}'],
               stdin=p1.stdout, stdout=PIPE, stderr=PIPE,
               universal_newlines=True)
    p3 = Popen(['neato', '-n2', '-Tsvg'],
               stdin=p2.stdout, stdout=PIPE, stderr=PIPE,
               universal_newlines=True)
    p1.stdout.close()  # Allow p1 to receive a SIGPIPE if p2 exits.
    p2.stdout.close()  # Allow p2 to receive a SIGPIPE if p3 exits.
    p1.stdin.write(dot)
    p1.stdin.close()
    out, err = p3.communicate()
    if p1.wait():
        raise RuntimeError(engine + " failed: " + p1.stderr.read())
    if p2.wait():
        raise RuntimeError("gvpr failed: " + p2.stderr.read())
    if p3.wait():
        raise RuntimeError("neato failed: " + err)
    if isinstance(out, bytes):
        out = out.decode('utf-8')
    return out


@lru_cache(maxsize=32)
def _dot_to_svg_dot2tex(dot, engine="dot", *args):
    '''The conversion of a Dot source into SVG by dot2tex.

    Requires dot2tex, texi2pdf and pdf2svg.
    '''
    with _tmp_file('tex') as tex, \
            _tmp_file('pdf') as pdf, \
            _tmp_file('svg') as svg:
        p1 = Popen(['dot2tex', '--prog', engine],
                   stdin=PIPE, stdout=tex, stderr=PIPE,
                   universal_newlines=True)
        out, err = p1.communicate(dot)
        if p1.wait():
            raise RuntimeError("dot2tex failed: " + err)
        check_call(["texi2pdf", "--batch", "--clean", "--quiet",
                    "--output", pdf.name, tex.name])
        check_call(["pdf2svg", pdf.name, svg.name])
        res = open(svg.name).read()
        if isinstance(res, bytes):
            res = res.decode('utf-8')
        return res


class Daut:

    def __init__(self):
        self.transitions = []
        self.id = r'(?:\w+|"(?:[^\\"]|\\.)*")'
        # An RE than matches transitions in Daut syntax.
        self.re_daut_tr = '^ *({id}|\$)? *-> *({id}|\$)? *(.*?)$'.format(id = self.id)

    def quote(self, s):
        '''Turn a string (label) into a string in double-quotes.'''
        if len(s) < 2 or s[0] != '"' or s[-1] != '"':
            s = '"' + re.sub(r'([\\"])', r'\\\1', s) + '"'
        return s

    def unquote(self, s):
        '''Strip double-quotes and escapes from a string.'''
        if 2 <= len(s) and s[0] == '"' and s[-1] == '"':
            s = re.sub(r'\\(.)', r'\1', s[1:-1])
        return s

    # Using split(',') is tempting, but will break strings
    # that contain commas --- e.g., [label = "a, b"].
    def attr_dot_split(self, s):
        attr = r'{id}(?:\s*=\s*{id})?'.format(id=self.id)

        scanner = re.Scanner([
            (",;", None),
            (attr, lambda scanner, tok: tok),
            (r"\s+", None),
        ])
        return scanner.scan(s)[0]

    def parse_attr_dot(self, s):
        if s:
            s.strip()
            if s.startswith('[') and s.endswith(']'):
                s = s[1:-1]
            res = [a.strip() for a in self.attr_dot_split(s)]
            return res
        else:
            res = []
        return res

    def attr_dot(self, attrs):
        '''Receive a Dot list of attributes, and if the first
        is not a proper assignment, consider it's the label, so
        prepend 'label=' to it.
        '''
        if attrs and not attrs[0].startswith("label"):
            attrs[0] = 'label = {}'.format(self.quote(attrs[0]))
        for i, a in enumerate(attrs):
            if a in ['blue', 'red', 'green']:
                attrs[i] = "color={a}, fontcolor={a}".format(a=a)
        # Join on ";" rather that ",".
        if attrs:
            return "[" + "; ".join(attrs) + "]"
        else:
            return ""

    def parse_attr_daut(self, s):
        '''Return the list of attributes in Daut syntax.'''
        if s:
            s.strip()
            res = [a.strip() for a in s.split(';')]
        else:
            res = []
        return res

    def attr_daut(self, attrs):
        '''Keep a single attribute: the label.'''
        if attrs:
            for a in attrs:
                if a.startswith('label'):
                    return self.unquote(re.sub('label *= *', '', a))
        return ''

    def transition_daut(self, s, d, a):
        '''Format a transition to Daut syntax.'''
        label = self.attr_daut(a)
        return "{} -> {}{}{}".format(s or '$', d or '$',
                                     ' ' if label else '', label)

    def prepend(self, prefix, s):
        if s.startswith('"'):
            return '"' + prefix + s[1:]
        else:
            return prefix + s

    def transition_dot(self, s, d, a):
        '''Format a transition to Dot syntax.'''
        if s == '' or s == '$':
            s = self.prepend('I', d)
            self.hidden.append(s)
        if d == '' or d == '$':
            d = self.prepend('F', s)
            self.hidden.append(d)
        a = self.attr_dot(a)
        return '  {} -> {}{}{}'.format(s, d, ' ' if a else '', a)

    def parse_context(self, match):
        '''Record the context.'''
        self.context = match.group(1)

    def parse_transition(self, match, format):
        '''Return (source, destination, attributes) with Daut syntax.'''
        s = match.group(1)
        if s is None or s.startswith('I') or s.startswith('"I'):
            s = '$'
        d = match.group(2)
        if d is None or d.startswith('F') or s.startswith('"F'):
            d = '$'
        if format == "dot":
            attr = self.parse_attr_dot(match.group(3))
        else:
            attr = self.parse_attr_daut(match.group(3))
        return (s, d, attr)

    def daut_to_dot(self, s):
        '''Convert from Daut syntax to Dot.'''
        self.context = "lal_char, b"
        # The list of pre/post states.
        self.hidden = []
        s = re.sub('^ *(?:vcsn_)?(?:context|ctx) *= *"?(.*?)"?$',
                   self.parse_context, s, flags=re.MULTILINE)
        s = re.sub(self.re_daut_tr,
                   lambda m: self.transition_dot(
                       *self.parse_transition(m, "daut")),
                   s, flags=re.MULTILINE)
        return '''digraph
{{
  vcsn_context = "{context}"
  rankdir = LR
  {edge_style}
  {{
    {state_point}
    {hidden}
  }}
  {state_style}
  {transitions}
}}'''.format(context=self.context,
             transitions=s,
             state_point=state_point,
             state_style=state_style,
             edge_style=edge_style,
             hidden=" ".join(self.hidden))

    def daut_to_transitions(self, s):
        '''Extract the list of transitions (as triples) from Daut.'''
        return re.findall(self.re_daut_tr, s, flags=re.MULTILINE)

    def dot_to_daut(self, s):
        '''Convert from Dot syntax to Daut.'''
        res = []
        s = re.sub('^ *vcsn_context *= *"(.*?)"$',
                   lambda m: res.append('context = "{}"'.format(m.group(1))),
                   s, flags=re.MULTILINE)
        re.sub('^ *({id}?) *-> *({id}?) *(\[.*?\])?$'.format(id=self.id),
               lambda m: res.append(
                   self.transition_daut(*self.parse_transition(m, "dot"))),
               s, flags=re.MULTILINE)
        return "\n".join(res)


def daut_to_dot(s):
    '''Read a Daut input, translate to regular Dot.'''
    d = Daut()
    return d.daut_to_dot(s)


def dot_to_daut(s):
    '''Read a Dot input, simplify it into Daut.'''
    d = Daut()
    return d.dot_to_daut(s)


def daut_to_transitions(s):
    '''From a Daut, return the list of transitions as triples
    `(src, dst, entry)`, using `$` to denote pre/post states.'''
    d = Daut()
    return d.daut_to_transitions(s)
