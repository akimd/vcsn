#ifndef VCSN_MISC_FADO_HH
# define VCSN_MISC_FADO_HH

# include <string>
# include <sstream>

# include <vcsn/dyn/fwd.hh>

namespace vcsn
{
  namespace detail
  {
    namespace efsm
    {
      dyn::automaton parse_file(const std::string& file);
    }

    namespace fado
    {
      dyn::automaton parse_file(const std::string& file);
    }
  }
}
#endif // VCSN_MISC_FADO_HH
