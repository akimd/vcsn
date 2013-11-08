#ifndef VCSN_MISC_FADO_HH
# define VCSN_MISC_FADO_HH

# include <string>
# include <sstream>

# include <vcsn/dyn/fwd.hh>

namespace vcsn
{
  namespace dyn
  {
    // efsm.cc.
    automaton read_efsm(std::istream& is);
    // fado.cc.
    automaton read_fado(std::istream& is);
  }
}
#endif // VCSN_MISC_FADO_HH
