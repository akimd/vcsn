#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/registers.hh>
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

    REGISTER_DEFINE(cominimize);
    automaton
    cominimize(const automaton& aut, const std::string& algo)
    {
      return detail::cominimize_registry().call(aut, algo);
    }
  }
}
