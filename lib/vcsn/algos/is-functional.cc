#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/algos/is-functional.hh>
#include <vcsn/algos/is-partial-identity.hh>
#include <lib/vcsn/algos/registry.hh>

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
