#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/expression.hh>
#include <vcsn/dyn/registers.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(zpc);
    automaton
    zpc(const dyn::expression& e)
    {
      return detail::zpc_registry().call(e);
    }
  }
}
