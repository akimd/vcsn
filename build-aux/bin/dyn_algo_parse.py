#! /usr/bin/env python

import re
import textwrap

class Indent(str):
    '''
    An indenting string, to be used with str.format.
    See http://stackoverflow.com/questions/37501228.
    '''

    indent_format_spec = re.compile(r'i([0-9]+)')

    def __format__(self, format_spec):
        matches = self.indent_format_spec.search(format_spec)
        if matches:
            level = int(matches.group(1))
            first, sep, text = textwrap.dedent(self).strip().partition('\n')
            return first + sep + textwrap.indent(text, ' ' * level)
        else:
            return super(Indent, self).__format__(format_spec)


def vcsn_prefix(s):
    '''Convert prefix types that belong to `vcsn::`.'''
    return re.sub('(letter_class_t)',
                  r'vcsn::\1', s)

# The types defined in vcsn::dyn that we wrap.
dyn_types = ['automaton', 'context', 'expansion', 'expression', 'label',
             'polynomial', 'weight']


# A regexp to "parse" a function declaration in `vcsn/dyn/algos.hh`.
function_re = re.compile(r'''(?P<doc>(?:^\s*///[^\n]*\n)*)?
^\s*(?P<result>[:\w]+)
\s+(?P<dynfun>\w+)\s*\((?P<formals>.*?)\);''',
                    flags=re.DOTALL | re.MULTILINE | re.VERBOSE)

# A regex to parse the components of a C++ formal argument such
# as `const std::string& algo = "default"`.
formal_re = re.compile(r'^(?P<type>(?P<const>const\s+)?(?P<class>[:\w<>]+)&?)'
                       r'\s+'
                       r'(?P<arg>\w+)'
                       r'(?P<default>\s+=.*)?$',
                       flags=re.MULTILINE)
def formals_list(formals):
    return re.split(r',\s*', formals)


def formal_to_class(formal):
    '''`const std::string& algo = "default"` => `std::string`.'''
    return formal_re.sub(r'\g<class>', formal)

def formal_to_arg(formal):
    '''`const std::string& algo = "default"` => `algo`, but
    `const automaton& aut = "default"` => `aut.val_`'''
    res = formal_re.sub(r'\g<arg>', formal)
    cls = formal_to_class(formal)
    if cls in dyn_types:
        res += '.val_'
    return res

def formals_to_args(formals):
    return [formal_to_arg(f) for f in formals]
