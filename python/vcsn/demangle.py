try:
    import regex as re
    has_regex = True
except ImportError:
    has_regex = False
import sys

parameters = r'''
(?<rec>    # capturing group rec
 (?:       # non-capturing group
  [^<>]++  # anything but angle brackets one or more times without backtracking
  |        # or
  <(?&rec)>  # recursive substitute of group rec
 )*
)'''

def sub(pattern, repl, string, *args, **kwargs):
    '''Beware that in pattern the spaces are ignored: use \s.'''
    pattern = pattern.format(params = parameters)
    num = 1
    while num:
        (string, num) = re.subn(pattern, repl, string, *args,
                                flags=re.VERBOSE, **kwargs)
#        print(string, num)
    return string

def demangle_regex(s):
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
    s = sub(r'(?:std::)?shared_ptr<(?:vcsn::)?(?:detail::)?(\w+_automaton)_impl<({params})>\s*>',
            r'\1<\2>',
            s)

    # Dyn::.
    s = sub(r'vcsn::dyn::detail::(automaton|expression(?:set)?)_wrapper<\1<({params})>\s*>',
            r'dyn::\1<\2>',
            s)

    return s

def demangle_re(s):
    return s

demangle = demangle_regex if has_regex else demangle_re
