#include <lib/vcsn/algos/registry.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/registers.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(is_functional);
    bool is_functional(const automaton& aut)
    {
      return detail::is_functional_registry().call(aut);
    }

    REGISTER_DEFINE(is_partial_identity);
    bool is_partial_identity(const automaton& aut)
    {
      return detail::is_partial_identity_registry().call(aut);
    }
  }
}
