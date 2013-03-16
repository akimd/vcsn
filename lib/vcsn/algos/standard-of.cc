#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/standard_of.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(standard_of);

    automaton
    standard_of(const dyn::ratexp& e)
    {
      return details::standard_of_registry().call(e->ctx().vname(), e);
    }
  }
}
