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
    # Nodes with values have a "style = rounded" which overrides the
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
