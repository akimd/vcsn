#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/standard.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(standard);

    automaton
    standard(const dyn::ratexp& e)
    {
      return details::standard_registry().call(e->ctx().vname(), e);
    }
  }
}
