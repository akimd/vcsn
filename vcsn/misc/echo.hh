#ifndef VCSN_MISC_ECHO_HH
# define VCSN_MISC_ECHO_HH

# define V(S) #S ": " << S << " "
# define ECHO(S) std::cerr << S << std::endl
# define ECHOV(S) ECHO(V(S))

#endif // !VCSN_MISC_ECHO_HH
