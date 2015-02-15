#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/expression.hh>
#include <vcsn/dyn/registers.hh>
#include <vcsn/dyn/algos.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(identities);

    rat::identities
    identities(const expression& exp)
    {
      return detail::identities_registry().call(exp);
    }

  }
}
