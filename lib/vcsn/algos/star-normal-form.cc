#include <vcsn/dyn/expression.hh>
#include <vcsn/dyn/registers.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*------------------------.
    | star_normal_form(exp).  |
    `------------------------*/
    REGISTER_DEFINE(star_normal_form);

    expression
    star_normal_form(const expression& e)
    {
      return detail::star_normal_form_registry().call(e);
    }
  }
}
