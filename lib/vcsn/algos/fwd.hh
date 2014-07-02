#ifndef LIB_VCSN_ALGOS_FWD_HH
# define LIB_VCSN_ALGOS_FWD_HH

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
#endif // !LIB_VCSN_ALGOS_FWD_HH
