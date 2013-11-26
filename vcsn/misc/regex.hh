#ifndef VCSN_MISC_REGEX_HH
# define VCSN_MISC_REGEX_HH

# include <vcsn/config.hh>

# if VCSN_STD_REGEX_WORKS

#  include <regex>

# else

#  include <boost/tr1/regex.hpp>
namespace std
{
  using namespace std::tr1;
}

# endif // !VCSN_STD_REGEX_WORKS

#endif // !VCSN_MISC_REGEX_HH
