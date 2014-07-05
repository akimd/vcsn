#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/minimize.hh>
#include <vcsn/algos/reduce.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(minimize);
    automaton
    minimize(const automaton& aut, const std::string& algo)
    {
      return detail::minimize_registry().call(aut, algo);
    }

    REGISTER_DEFINE(reduce);
    automaton
    reduce(const automaton& aut)
    {
      return detail::reduce_registry().call(aut);
    }
  }
}
