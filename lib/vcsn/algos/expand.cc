#include <vcsn/dyn/expression.hh>
#include <vcsn/dyn/registers.hh>
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

    expression
    expand(const expression& e)
    {
      return detail::expand_registry().call(e);
    }
  }
}
