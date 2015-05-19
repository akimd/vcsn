#pragma once

#include <vcsn/config.hh>

#if VCSN_STD_REGEX_WORKS

# include <regex>

#else

# include <boost/tr1/regex.hpp>
namespace std
{
  using namespace std::tr1;
}

#endif // !VCSN_STD_REGEX_WORKS
