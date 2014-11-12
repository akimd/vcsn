#include <string>

#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/scc.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  /*-----.
  | SCC. |
  `-----*/

  namespace dyn
  {

    REGISTER_DEFINE(num_sccs);

    std::size_t
    num_sccs(const automaton& aut, const std::string& algo)
    {
      return detail::num_sccs_registry().call(aut, algo);
    }

    REGISTER_DEFINE(scc);
    automaton
    scc(const automaton& aut, const std::string& algo)
    {
      return detail::scc_registry().call(aut, algo);
    }
  }
}
