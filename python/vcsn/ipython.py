from __future__ import print_function
from IPython.core.magic import (Magics, magics_class, line_magic)
from IPython.display import display, SVG
from IPython.html import widgets

import vcsn
from vcsn.dot import _dot_to_svg, _dot_pretty
from IPython.utils.warn import info, error

# The class MUST call this class decorator at creation time
class EditAutomatonWidget:
    def __init__(self, ipython, name):
        self.ipython = ipython
        self.name = name
        if self.name in self.ipython.shell.user_ns:
            # Strip the automaton, as we cannot preserve the state names
            # anyway.
            aut = self.ipython.shell.user_ns[self.name].strip()
        else:
            aut = vcsn.context('lal_char(abc)_b').ratexp(r'\e').standard()
            self.ipython.shell.user_ns[self.name] = aut
        daut = from_dot(str(aut))

        self.text = widgets.TextareaWidget(value = daut)
        height = self.text.value.count('\n')
        self.text.set_css({'lines': '500'})
        self.text.on_trait_change(lambda: self.update())

        self.error = widgets.HTMLWidget(value = '')
        self.svg = widgets.HTMLWidget(value = aut._repr_svg_())

        cw = widgets.ContainerWidget()
        cw.remove_class('vbox')
        cw.add_class('hbox')
        cw.children = [self.svg, self.error, self.text]
        display(cw)

    def update(self):
       try:
            self.error.value = ''
            dot = self.text.value.encode('utf-8')
            dot = to_dot(dot)
            self.ipython.shell.user_ns[self.name] = vcsn.automaton(dot)
            self.svg.value = _dot_to_svg(_dot_pretty(dot))
       except RuntimeError, e:
            self.error.value = str(e)

@magics_class
class EditAutomaton(Magics):

    @line_magic
    def automaton(ipython, line):
        name = line.split()[0]
        EditAutomatonWidget(ipython, name)

ip = get_ipython()
ip.register_magics(EditAutomaton)



class Daut:

    def __init__(self):
        self.transitions = []
        self.id = r'(?:\w+|"(?:[^\\"]|\\.)*")'

    # Using split(',') is tempting, but will break strings
    # that contain commas --- e.g., [label = "a, b"].
    def attr_split(self, s):
        import re
        attr = r'{id}(?:\s*=\s*{id})?'.format(id = self.id)

        scanner = re.Scanner([
            # Don't split on ",", as we like to use it in transition
            # labels.
            (r";", None),
            (attr, lambda scanner, tok: tok),
            (r"\s+", None),
            ])
        return scanner.scan(s)[0]

    def attr_dot(self, s):
        '''Receive a Dot list of attributes, and if the first
        is not a proper assignment, consider it's the label, so
        prepend 'label=' to it.
        '''
        if s:
            s.strip()
            if s.startswith('[') and s.endswith(']'):
                s = s[1:-1]
            attrs = [a.strip() for a in self.attr_split(s)]
            if attrs and not attrs[0].startswith("label"):
                attrs[0] = 'label = "{}"'.format(attrs[0].strip('"'))
            for i, a in enumerate(attrs):
                if a in ['blue', 'red', 'green']:
                    attrs[i] = "color={a}, fontcolor={a}".format(a = a)
            # Join on ";" rather that ",".
            s = "[" + "; ".join(attrs) + "]"
        else:
            s = ""
        return s

    def attr_daut(self, s):
        '''Return a list of attributes with the Daut syntax.'''
        import re
        if s:
            s.strip()
            if s.startswith('[') and s.endswith(']'):
                s = s[1:-1]
            attrs = [a.strip() for a in self.attr_split(s)]
            if attrs:
                attrs[0] = re.sub("label *= *", '', attrs[0])
            # Join on ";" rather that "," to avoid confusion with
            # multiple labels.
            s = "; ".join(attrs)
        else:
            s = ""
        return s

    def transition_daut(self, s, d, a):
        return "{} -> {} {}".format(s or '$', d or '$', a)

    def transition_dot(self, s, d, a):
        if s == "" or s == "$":
            s = "I" + d
            self.hidden.append(s)
        if d == "" or d == "$":
            d = "F" + s
            self.hidden.append(d)
        return "  {} -> {} {}".format(s, d, self.attr_dot(a))

    def daut(self):
        return "\n".join([self.transition_daut(t[0], t[1], t[2])
                          for t in self.transitions])

    def parse_transition(self, match):
        s = match.group(1)
        if s.startswith('I'):
            s = '$'
        d = match.group(2)
        if d.startswith('F'):
            d = '$'
        attr = self.attr_daut(match.group(3))
        return (s, d, attr)

    def daut_to_dot(self, s):
        import re
        self.hidden = []
        s = re.sub('^ *({id}|\$)? *-> *({id}|\$)? *(.*?)?$'.format(id = self.id),
                   lambda m: self.transition_dot(*self.parse_transition(m)),
                   s, flags = re.MULTILINE)
        return '''digraph
{{
  vcsn_context = "lal_char(abc)_b"
  rankdir = LR
  {{
    node [shape = point, width = 0]
    {hidden}
  }}
  node [shape = circle]
  {transitions}
}}'''.format(transitions = s, hidden=" ".join(self.hidden))

    def dot_to_daut(self, s):
        import re
        res = []
        re.sub('^ *({id}?) *-> *({id}?) *(\[.*?\])?$'.format(id = self.id),
                   lambda m: res.append(self.transition_daut(*self.parse_transition(m))),
                   s, flags = re.MULTILINE)
        return "\n".join(res)

def to_dot(s):
    d = Daut()
    return d.daut_to_dot(s)

def from_dot(s):
    d = Daut()
    return d.dot_to_daut(s)
