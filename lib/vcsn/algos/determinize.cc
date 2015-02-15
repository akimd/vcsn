#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/registers.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(determinize);

    automaton
    determinize(const automaton& aut, const std::string& algo)
    {
      return detail::determinize_registry().call(aut, algo);
    }

    REGISTER_DEFINE(codeterminize);

    automaton
    codeterminize(const automaton& aut, const std::string& algo)
    {
      return detail::codeterminize_registry().call(aut, algo);
    }

  }
}
