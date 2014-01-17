#include <vcsn/dyn/ratexp.hh>
#include <vcsn/algos/expand.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*--------------.
    | expand(exp).  |
    `--------------*/
    REGISTER_DEFINE(expand);

    ratexp
    expand(const ratexp& e)
    {
      return detail::expand_registry().call(e);
    }
  }
}
