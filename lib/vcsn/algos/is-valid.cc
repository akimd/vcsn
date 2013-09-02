#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/is-valid.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*----------------.
    | is_valid(exp).  |
    `----------------*/

    REGISTER_DEFINE(is_valid_exp);

    bool
    is_valid(const ratexp& e)
    {
      return detail::is_valid_exp_registry().call(e->vname(false),
                                                  e);
    }
  }
}
