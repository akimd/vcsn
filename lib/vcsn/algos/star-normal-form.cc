#include <vcsn/dyn/ratexp.hh>
#include <vcsn/algos/star-normal-form.hh>
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

    ratexp
    star_normal_form(const ratexp& e)
    {
      return detail::star_normal_form_registry().call(e);
    }
  }
}
