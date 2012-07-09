#ifndef VCSN_MISC_ECHO_HH
# define VCSN_MISC_ECHO_HH

/// Display S and its value.
# define V(S)                                   \
  #S ": " << S << " "

/// Display S and its value in pseudo XML tags, in case of nesting.
# define VV(S)                                                          \
  "<" #S ":" __HERE__ << ">" << S << "</" << #S << ">"

# define ECHO(S) std::cerr << S << std::endl

# define ECHOV(S) ECHO(V(S))

# define __HERE__ __FILE__ ": " << __LINE__

# define ECHOH(S) ECHO(__HERE__ << ": " << S)

#endif // !VCSN_MISC_ECHO_HH
