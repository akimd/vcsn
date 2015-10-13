import re

try:
    import regex as re
    has_regex = True
    param1 = r'''
(?<rec1>    # capturing group rec1
 (?:        # non-capturing group
  [^<>]++   # anything but angle brackets one or more times without backtracking
  |         # or
  <(?&rec1)>  # recursive substitute of group rec1
 )*
)'''
    param2 = r'''
(?<rec2>    # capturing group rec2
 (?:        # non-capturing group
  [^<>]++   # anything but angle brackets one or more times without backtracking
  |         # or
  <(?&rec2)>  # recursive substitute of group rec2
 )*
)'''
except ImportError:
    has_regex = False
import sys

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
    color_cycle = [ red, blue, yellow, green, magenta, cyan ]
    colors_enabled = True
except:
    colors_enabled = False

parameters = r'''
(?<rec>    # capturing group rec
 (?:       # non-capturing group
  [^<>]++  # anything but angle brackets one or more times without backtracking
  |        # or
  <(?&rec)>  # recursive substitute of group rec
 )*
)'''


def sugar(s):
    '''Perform some transformations that aim at displaying a cuter version
    of a Vcsn type string.'''
    # Long specs are split into subdirs.  Remove the subdirs.
    s = s.replace('/', '')
    s = re.sub(r'(char|string)_letter', r'\1', s)
    s = re.sub(r'(\w+)_automaton', r'\1', s)
    if has_regex:
        s = re.sub(r'nullableset<{param1}>'.format(param1=param1),
                                   r'(\1)?',
                                   s,
                                   flags=re.VERBOSE)
        s = re.sub(r'letterset<{param1}>'.format(param1=param1),
                                   r'\1',
                                   s,
                                   flags=re.VERBOSE)
        s = re.sub(r'wordset<{param1}>'.format(param1=param1),
                                   r'(\1)*',
                                   s,
                                   flags=re.VERBOSE)
        s = re.sub(r'context<{param1},\ +{param2}>'.format(param1=param1,
                                                           param2=param2),
                   r'\1 → \2',
                   s,
                   flags=re.VERBOSE)
    else:
        s = re.sub(r'^context<(.*)>$', r'\1', s)
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
            "always" : True,
            "never" : False,
            "auto" : sys.stdout.isatty()
            }

    return color_dict[color]

delimiters_open = [ '<', '[', '(' ]
delimiters_close = {
        '>' : '<',
        ']' : '[',
        ')' : '('
        }

if colors_enabled:
    color_patterns = [
        [ "In file included from", green ],
        [ "In instantiation of", magenta ],
        [ "required from here", magenta ],
        [ " required from", magenta ], #space is voluntary
        [ "recursively required by substitution of", magenta ],
        [ "required by substitution of ", magenta ], #space is voluntary
        [ "error:", red ],
        [ "could not convert", red ],
        [ "' from '", red ],
        [ "' to '", red ],
        [ "no type named ", red ],
        [ "was not declared in this scope", red ],
        [ "' in '", red ],
        [ "in expansion of macro", white ],
        [ "with ", white ],
        ]

def colorize_pattern(line):
    for p in color_patterns:
        line = line.replace(p[0], p[1] + p[0] + std)
    return line


def colorize_line(line):
    res = ""
    delim_stack = []

    for c in line:
        if c in delimiters_open:
            res += color_cycle[len(delim_stack) % len(color_cycle)]
            res += c
            res += std
            delim_stack.append(c)
        elif c in delimiters_close.keys():
            if delim_stack == [] or delimiters_close[c] != delim_stack[-1]:
                return line #error, don't color anything
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
    return res + '\n'

def colorize(line):
    if not colors_enabled:
        return line
    lines = line.split('\n')

    res = ""
    for line in lines:
        res += colorize_line(line)
    return colorize_pattern(res)



def sub(pattern, repl, string, *args, **kwargs):
    '''Beware that in pattern the spaces are ignored: use \s.'''
    pattern = pattern.format(params = parameters)
    num = 1
    while num:
        (string, num) = re.subn(pattern, repl, string, *args,
                                flags=re.VERBOSE, **kwargs)
#        print(string, num)
    return string

def demangle_regex(s, color="auto"):
    color = has_color(color)
    # C++.
    s = sub(r'std::(?:__1|__cxx11)::(allocator|basic_string|basic_ostream|char_traits|forward|less|make_shared|map|pair|set|shared_ptr|string|tuple)',
            r'std::\1',
            s)
    s = sub(r'std::basic_string<char(?:,\s*(?:std::)?char_traits<char>,\s*(?:std::)?allocator<char>\s*)?>',
            r'std::string',
            s)

    # Misc.
    s = sub(r'boost::flyweights::flyweight<std::string,\s*boost::flyweights::no_tracking,\s*boost::flyweights::intermodule_holder(?:,\s*boost::parameter::void_)*>',
            r'vcsn::symbol',
            s)

    # Labesets.
    s = sub(r'(?:vcsn::)?letterset<(?:vcsn::)?set_alphabet<(?:vcsn::)?(\w+)_letters>\s*>',
            r'lal_\1',
            s)
    s = sub(r'(?:vcsn::)?wordset<(?:vcsn::)?set_alphabet<(?:vcsn::)?(\w+)_letters>\s*>',
            r'law_\1',
            s)
    s = sub(r'vcsn::(nullableset|tupleset)<({params})>',
            r'\1<\2>',
            s)

    # Weightsets.
    s = sub(r'(?:vcsn::)?weightset_mixin<(?:vcsn::)?(?:detail::)?([bq]|[zr](?:min)?)_impl>',
            r'\1',
            s)

    s = sub(r'(?:vcsn::)?weightset_mixin<(?:vcsn::rat::)?(expressionset)_impl<({params})>\s*>',
            r'\1<\2>',
            s)

    s = sub(r'(?:vcsn::)?weightset_mixin<(?:vcsn::detail::)?(tupleset)_impl<({params})>\s*>',
            r'\1<\2>',
            s)

    # Contexts.
    s = sub(r'vcsn::(context)<({params})>',
            r'\1<\2>',
            s)

    # Polynomials.
    s = sub(r'(vcsn::detail::wet_map<std::shared_ptr<const\s*vcsn::rat::node<({params})\s*>\s*>,\s*bool),\s*vcsn::less<expressionset<\2\s*>,\s*std::shared_ptr<const\s*vcsn::rat::node<\2\s*>\s*>\s*>\s*>',
            r'\1>',
            s)

    # Typedef.
    s = sub(r'(vcsn::detail::partition_automaton_impl<Aut>)::(transition|state)_t',
            r'\2_t_of<\1>',
            s)

    # Automata.
    #
    # The optional 'std::' is surprising, granted, but I do have seen
    # it (with clang 3.6).
    #
    # The optional parameter of shared_ptr is the memory lock policy (g++)
    s = sub(r'(?:std::)?(?:__)?shared_ptr<(?:vcsn::)?(?:detail::)?(\w+_automaton)_impl<({params})>\s*(?:, [^>]+)?>',
            r'\1<\2>',
            s)

    # Dyn::.
    s = sub(r'vcsn::dyn::detail::(automaton|expression(?:set)?)_wrapper<\1<({params})>\s*>',
            r'dyn::\1<\2>',
            s)

    if color:
        s = colorize(s)

    return s

def demangle_re(s, color="auto"):
    return s

demangle = demangle_regex if has_regex else demangle_re
