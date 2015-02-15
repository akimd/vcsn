#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/registers.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  /*--------.
  | Filter. |
  `--------*/

  namespace dyn
  {

    REGISTER_DEFINE(filter);

    automaton
    filter(const automaton& aut, const std::vector<unsigned>& ss)
    {
      return detail::filter_registry().call(aut, ss);
    }
  }
}
