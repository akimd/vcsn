# -*- coding: utf-8 -*-
import re

from subprocess import check_call, PIPE, Popen

from vcsn import _tmp_file

def _latex_to_html(s):
    "Convert LaTeX angle brackets and \\e to HTML entities."
    return (s.replace('<', '⟨')
            .replace('>', '⟩')
            .replace(r'\\e', 'ε'))

def _labels_as_tooltips(s):
    return re.sub(r'label = (".*?"), shape = box, style = rounded',
                  r'tooltip = \1',
                  s)

def _dot_pretty(s, mode = "dot"):
    '''Improve pretty-printing in a dot source.

    Use some HTML entities instead of ugly ASCII sequences.
    Use nicer arrows.

    If `mode` is tooltip, convert node labels to tooltips.
    '''
    s = re.sub(r'(label * = *)(".*?")',
               lambda m: m.group(1) + _latex_to_html(m.group(2)),
               s)
    if mode == "tooltip":
        s = _labels_as_tooltips(s)
    # It looks like we could pass -E/-N option to dot to set defaults
    # at the last moment, unfortutately in that case, it takes
    # precedence on "edge" and "node" attributes defined in the file
    # itself, which would, for instance, discard the shape=circle
    # attribute.
    #
    # FIXME: This 'replace' might also be replaced by a use of gvpr.
    s = s.replace('rankdir = LR',
                  'rankdir = LR\n'
                  '  edge [arrowhead = vee, arrowsize = .6]\n'
                  '  node [fillcolor = cadetblue1, style = filled]')
    # Useless states should be filled in gray, instead of having a
    # gray contour.  Fill with a lighter gray.
    s = s.replace('[color = DimGray]',
                  '[fillcolor = lightgray, style = filled]')
    # States with values have a "style = rounded" which overrides the
    # global 'style = filled'.  Also set the size to something nicer
    # than the default (which makes box too wide).
    s = s.replace('style = rounded',
                  'style = "filled,rounded", width = .5')
    return s

def _dot_to_svg(dot, engine="dot", *args):
    "The conversion of a Dot source into SVG by dot."
    # http://www.graphviz.org/content/rendering-automata
    p1 = Popen([engine] + list(args),
               stdin=PIPE, stdout=PIPE, stderr=PIPE)
    p2 = Popen(['gvpr', '-c', 'E[head.name == "F*"]{lp=pos=""}'],
               stdin=p1.stdout, stdout=PIPE, stderr=PIPE)
    p3 = Popen(['neato', '-n2', '-Tsvg'],
               stdin=p2.stdout, stdout=PIPE, stderr=PIPE)
    p1.stdout.close()  # Allow p1 to receive a SIGPIPE if p2 exits.
    p2.stdout.close()  # Allow p2 to receive a SIGPIPE if p3 exits.
    p1.stdin.write(dot)
    p1.stdin.close()
    out, err = p3.communicate()
    if p1.wait():
        raise RuntimeError(engine + " failed: " + p1.stderr.redd().decode('utf-8'))
    if p2.wait():
        raise RuntimeError("gprv failed: " + p2.stderr.read().decode('utf-8'))
    if p3.wait():
        raise RuntimeError("neato failed: " + err.decode('utf-8'))
    return out.decode('utf-8')

def _dot_to_svg_dot2tex(dot, engine="dot", *args):
    '''The conversion of a Dot source into SVG by dot2tex.

    Requires dot2tex, texi2pdf and pdf2svg.
    '''
    with _tmp_file('tex') as tex, \
         _tmp_file('pdf') as pdf, \
         _tmp_file('svg') as svg:
        p1 = Popen(['dot2tex', '--prog', engine],
                   stdin=PIPE, stdout=tex, stderr=PIPE)
        p1.stdin.write(dot)
        out, err = p1.communicate()
        if p1.wait():
            raise RuntimeError("dot2tex failed: " + err.decode('utf-8'))
        check_call(["texi2pdf", "--batch", "--clean", "--quiet",
                    "--output", pdf.name, tex.name])
        check_call(["pdf2svg", pdf.name, svg.name])
        return open(svg.name).read()


class Daut:

    def __init__(self):
        self.transitions = []
        self.id = r'(?:\w+|"(?:[^\\"]|\\.)*")'

    # Using split(',') is tempting, but will break strings
    # that contain commas --- e.g., [label = "a, b"].
    def attr_dot_split(self, s):
        attr = r'{id}(?:\s*=\s*{id})?'.format(id = self.id)

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
            attrs[0] = 'label = "{}"'.format(attrs[0].strip('"'))
        for i, a in enumerate(attrs):
            if a in ['blue', 'red', 'green']:
                attrs[i] = "color={a}, fontcolor={a}".format(a = a)
        # Join on ";" rather that ",".
        if attrs:
            return "[" + "; ".join(attrs) + "]"
        else:
            return "";

    def parse_attr_daut(self, s):
        if s:
            s.strip()
            if s.startswith('[') and s.endswith(']'):
                s = s[1:-1]
            res = [a.strip() for a in s.split(';')]
        else:
            res = []
        return res

    def attr_daut(self, attrs):
        '''Return a list of attributes with the Daut syntax.'''
        if attrs:
            attrs[0] = re.sub("label *= *", '', attrs[0])
            return "; ".join(attrs)
        else:
            return ""

    def transition_daut(self, s, d, a):
        a = self.attr_daut(a)
        return "{} -> {}{}{}".format(s or '$', d or '$',
                                     " " if a else "", a)

    def transition_dot(self, s, d, a):
        if s == "" or s == "$":
            s = "I" + d
            self.hidden.append(s)
        if d == "" or d == "$":
            d = "F" + s
            self.hidden.append(d)
        a = self.attr_dot(a)
        return "  {} -> {}{}{}".format(s, d, " " if a else "", a)

    def parse_context(self, match):
        self.context = match.group(1)

    def parse_transition(self, match, format):
        '''Return (source, destination, attributes) with Daut syntax.'''
        s = match.group(1)
        if s is None or s.startswith('I'):
            s = '$'
        d = match.group(2)
        if d is None or d.startswith('F'):
            d = '$'
        if format == "dot":
            attr = self.parse_attr_dot(match.group(3))
        else:
            attr = self.parse_attr_daut(match.group(3))
        return (s, d, attr)

    def daut_to_dot(self, s):
        '''Convert from Daut syntax to Dot.'''
        self.context = "lal_char(abc)_b"
        self.hidden = []
        s = re.sub('^ *(?:vcsn_)?(?:context|ctx) *= *"?(.*?)"?$',
                   self.parse_context, s, flags = re.MULTILINE)
        s = re.sub('^ *({id}|\$)? *-> *({id}|\$)? *(.*?)$'.format(id = self.id),
                   lambda m: self.transition_dot(*self.parse_transition(m, "daut")),
                   s, flags = re.MULTILINE)
        return '''digraph
{{
  vcsn_context = "{context}"
  rankdir = LR
  {{
    node [shape = point, width = 0]
    {hidden}
  }}
  node [shape = circle]
  {transitions}
}}'''.format(context = self.context,
             transitions = s,
             hidden=" ".join(self.hidden))

    def dot_to_daut(self, s):
        '''Convert from Dot syntax to Daut.'''
        res = []
        s = re.sub('^ *vcsn_context *= *"(.*?)"$',
                   lambda m: res.append('context = "{}"'.format(m.group(1))),
                   s, flags = re.MULTILINE)
        re.sub('^ *({id}?) *-> *({id}?) *(\[.*?\])?$'.format(id = self.id),
               lambda m: res.append(self.transition_daut(*self.parse_transition(m, "dot"))),
               s, flags = re.MULTILINE)
        return "\n".join(res)

def to_dot(s):
    '''Read a Daut input, translate to regular Dot.'''
    d = Daut()
    return d.daut_to_dot(s)

def from_dot(s):
    '''Read a Dot input, simplify it into Daut.'''
    d = Daut()
    return d.dot_to_daut(s)
