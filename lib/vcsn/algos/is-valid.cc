#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/is-valid.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*----------------.
    | is_valid_r(exp).  |
    `----------------*/
    REGISTER_DEFINE(is_valid_r);

    bool
    is_valid_r(const ratexp& e)
    {
      return detail::is_valid_r_registry().call(e->vname(false),
                                                   e);
    }
  }
}
