#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/ratexp.hh>
#include <vcsn/algos/identities.hh>
#include <vcsn/dyn/algos.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(identities);

    rat::identities
    identities(const ratexp& exp)
    {
      return detail::identities_registry().call(exp);
    }

  }
}
