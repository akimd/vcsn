import os

# pylint: disable=line-too-long
import sys

try:
    import regex as re
    has_regex = True
except ImportError:
    import warnings
    warnings.warn('you should install regex for Python')
    has_regex = False
    import re


try:
    from colorama import Fore, Style
    green   = Fore.GREEN   + Style.BRIGHT
    red     = Fore.RED     + Style.BRIGHT
    yellow  = Fore.YELLOW  + Style.BRIGHT
    blue    = Fore.BLUE    + Style.BRIGHT
    magenta = Fore.MAGENTA + Style.BRIGHT
    cyan    = Fore.CYAN    + Style.BRIGHT
    white   = Fore.WHITE   + Style.BRIGHT
    std     = Style.RESET_ALL
    color_cycle = [red, blue, yellow, green, magenta, cyan]
    colors_enabled = True
except ImportError:
    colors_enabled = False


param_num = 0
def param(name=None):
    if name is None:
        global param_num # pylint: disable=global-statement
        name = param_num
        param_num += 1
    # Here, we use a backtracking group, which is *much* slower, but
    # it allows to write patterns such as `context<{param}, {param}>`,
    # where, were the [^<>]+ be possessive, the first one would eat
    # everything (both labelset and weightset).
    #
    # FIXME: maybe we need something in between.
    return r'''
(?<g{name}>     # capturing group
 (?:            # non-capturing group
  [^<>]+        # anything but angle brackets one or more times with backtracking
  |             # or
  <(?&g{name})> # recursive substitute of group
 )*
)'''.format(name=name)


def sub(pattern, repl, string, *args, **kwargs):
    r'''Apply `s/pattern/repl/g` as many times as possible in `string`.

    Spaces in the pattern are mapped to `\s*`.'''
    pattern = pattern.replace(' ', r'\s*')
    if '{param}' in pattern:
        if has_regex:
            pattern = re.sub(r'\{param\}', lambda _: param(), pattern)
        else:
            return string
    s = None
    while s != string:
        s = string
        string = re.sub(pattern, repl, s, *args,
                        flags=re.VERBOSE, **kwargs)
    return string


def sugar(s):
    '''Perform some transformations that aim at displaying a cuter version
    of a Vcsn type string.'''
    # Long specs are split into subdirs.  Remove the subdirs.
    s = s.replace('/', '')
    s = sub(r'(char|string)_letter', r'\1', s)
    s = sub(r'(\w+)_automaton', r'\1', s)
    s = sub(r'nullableset<{param}>', r'(\1)?', s)
    s = sub(r'letterset<{param}>', r'\1', s)
    s = sub(r'wordset<{param}>', r'(\1)*', s)
    s = sub(r'context<{param},\s*{param}>', r'\1 → \2', s)
    s = sub(r'^context<(.*)>$', r'\1', s)
    return s


def pretty_plugin(filename):
    '''Split compilation type with its arguments and add sugar to the message.'''
    # what = algos|contexts, specs = argument specifications.
    what, specs = re.match(r'.*/plugins/([^/]+)/(.*)', filename).group(1, 2)
    if what == 'algos':
        title, message = specs.split('/', 1)
        message = sugar(message)
    else:
        title = 'context'
        message = sugar(specs)
    return title, message


def has_color(color):
    color_dict = {
        "always": True,
        # Emacs is having too hard a time with the colors here.
        "auto":   sys.stdout.isatty() and not 'EMACS' in os.environ,
        "never":  False,
    }
    return color_dict[color]

delimiters_open = ['<', '[', '(']
delimiters_close = {
    '>': '<',
    ']': '[',
    ')': '('
}

if colors_enabled:
    color_patterns = [
        ["In file included from", green],
        ["In instantiation of", magenta],
        ["required from here", magenta],
        [" required from", magenta],  # space is voluntary
        ["recursively required by substitution of", magenta],
        ["required by substitution of ", magenta],  # space is voluntary
        ["error:", red],
        ["could not convert", red],
        ["' from '", red],
        ["' to '", red],
        ["no type named ", red],
        ["was not declared in this scope", red],
        ["' in '", red],
        ["in expansion of macro", white],
        ["with ", white],
        ["aka ", green],
    ]


def colorize_pattern(line):
    '''Highlight some interesting part of the error messages.'''
    for p in color_patterns:
        line = line.replace(p[0], p[1] + p[0] + std)
    return line


def colorize_line(line):
    '''Give matching colors to left/right pairs of parens/angles/brackets.'''

    res = ""
    delim_stack = []

    for c in line:
        if c in delimiters_open:
            res += color_cycle[len(delim_stack) % len(color_cycle)]
            res += c
            res += std
            delim_stack.append(c)
        elif c in delimiters_close.keys():
            # Beware that we may display a message which is a portion
            # of the source, so we might have more closing delimiters
            # than opening ones.  For instance:
            #
            # foo.cc:9:19: warning: expression result unused [-Wunused-value]
            #       ([](auto) { "foo"; },
            #                   ^~~~~
            # foo.cc:10:19: warning: expression result unused [-Wunused-value]
            #        [](auto) { "bar"; })
            #                   ^~~~~
            #
            # in line 10, there is one closing brace too many.
            if delim_stack == [] or delimiters_close[c] != delim_stack[-1]:
                return line  # error, don't color anything
            delim_stack.pop()
            res += color_cycle[len(delim_stack) % len(color_cycle)]
            res += c
            res += std
        else:
            res += c

    if res != "" and res[0] == '/':
        colon = res.find(':')
        if colon != -1:
            res = white + res[:colon] + std + res[colon:]
    return res


def colorize(line):
    if colors_enabled:
        line = "".join([colorize_line(l) for l in line.splitlines(True)])
        line = colorize_pattern(line)
    return line

def demangle(s, color="auto"):
    '''Improve the legibility of `s` (which can be several lines long) by
    using more friendly type names.'''

    color = has_color(color)
    # C++.
    s = sub(r'std::(?:__1|__cxx11)::(allocator|basic_string|basic_ostream|char_traits|equal_to|forward|function|hash|less|make_shared|(unordered_)?map|pair|(unordered_)?set|shared_ptr|string|tuple|vector)',
            r'std::\1',
            s)
    s = sub(r'std::basic_string<char(?:, (?:std::)?char_traits<char>, (?:std::)?allocator<char> )?>',
            r'std::string',
            s)
    s = sub(r'std::basic_([io]stream)<char(?:, (?:std::)?char_traits<char> )?>',
            r'std::\1',
            s)
    s = sub(r'std::vector<{param}, std::allocator<\1 > >',
            r'std::vector<\1>',
            s)
    s = sub(r'std::unordered_set<{param},  std::hash<\1 >, std::equal_to<\1 >,  std::allocator<\1 >',
            r'std::unordered_set<\1>',
            s)

    # Misc.
    s = sub(r'boost::flyweights::flyweight<std::string, boost::flyweights::no_tracking, boost::flyweights::intermodule_holder(?:, boost::parameter::void_)*>',
            r'vcsn::symbol',
            s)

    # Labesets.
    s = sub(r'(?:vcsn::)?letterset<(?:vcsn::)?set_alphabet<(?:vcsn::)?(\w+)_letters> >',
            r'lal_\1',
            s)
    s = sub(r'(?:vcsn::)?wordset<(?:vcsn::)?set_alphabet<(?:vcsn::)?(\w+)_letters> >',
            r'law_\1',
            s)
    s = sub(r'(?:vcsn::)?(nullableset|tupleset)<({param})>',
            r'\1<\2>',
            s)

    # Weightsets.
    s = sub(r'(?:vcsn::)?weightset_mixin<(?:vcsn::)?(?:detail::)?([bq]|[zr](?:min)?)_impl>',
            r'\1',
            s)

    s = sub(r'(?:vcsn::)?weightset_mixin<(?:vcsn::rat::)?(expressionset)_impl<({param})> >',
            r'\1<\2>',
            s)

    s = sub(r'(?:vcsn::)?weightset_mixin<(?:vcsn::detail::)?(tupleset)_impl<({param})> >',
            r'\1<\2>',
            s)

    # Contexts.
    s = sub(r'(?:vcsn::)?(context)<({param})>',
            r'\1<\2>',
            s)

    # Polynomials.
    s = sub(r'(vcsn::detail::wet_map<std::shared_ptr<const vcsn::rat::node<({param}) > >, bool), vcsn::less<expressionset<\2 >, std::shared_ptr<const vcsn::rat::node<\2 > > > >',
            r'\1>',
            s)

    # Automata.
    #
    # The optional 'std::' is surprising, granted, but I do have seen
    # it (with clang 3.6).
    #
    # The optional parameter of shared_ptr is the memory lock policy (g++)
    s = sub(r'(?:std::)?(?:__)?shared_ptr<(?:(?:vcsn::)?detail::)?(\w+_automaton)_impl<({param})> (?:, [^>]+)?>',
            r'\1<\2>',
            s)

    s = sub(r'(?:(?:vcsn::)?detail::)?index_t_impl<{param}::(state|transition)_tag>',
            r'\1::\2_t',
            s)

    # Typedef.  Beware that {param} introduces a group.
    s = sub(r'(?:(?:vcsn::)?detail::)?(partition_automaton_impl<{param}>)::(transition|state)_t',
            r'\3_t_of<\1>',
            s)

    # Dyn::.
    s = sub(r'vcsn::dyn::detail::value<vcsn::dyn::detail::(\w+)_tag>',
            r'dyn::\1',
            s)

    if color:
        s = colorize(s)

    return s
