#ifndef VCSN_MISC_FADO_HH
# define VCSN_MISC_FADO_HH

# include <string>
# include <sstream>

# include <vcsn/dyn/fwd.hh>

namespace vcsn
{
  namespace dyn
  {
    automaton read_efsm_file(const std::string& file);

    automaton read_fado_file(const std::string& file);
  }
}
#endif // VCSN_MISC_FADO_HH
