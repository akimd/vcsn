#include <string>

#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/registers.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  /*-----.
  | SCC. |
  `-----*/

  namespace dyn
  {

    REGISTER_DEFINE(num_components);
    std::size_t
    num_components(const automaton& aut)
    {
      return detail::num_components_registry().call(aut);
    }

    REGISTER_DEFINE(scc);
    automaton
    scc(const automaton& aut, const std::string& algo)
    {
      return detail::scc_registry().call(aut, algo);
    }

    REGISTER_DEFINE(component);
    automaton
    component(const automaton& aut, unsigned com_num)
    {
      return detail::component_registry().call(aut, com_num);
    }

    REGISTER_DEFINE(condense);
    automaton
    condense(const automaton& aut)
    {
      return detail::condense_registry().call(aut);
    }

  }
}
