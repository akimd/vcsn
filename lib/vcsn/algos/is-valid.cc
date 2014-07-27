#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/is-valid.hh>
#include <vcsn/algos/is-valid-ratexp.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*----------------------.
    | is_valid(automaton).  |
    `----------------------*/

    REGISTER_DEFINE(is_valid);

    bool
    is_valid(const automaton& a)
    {
      return detail::is_valid_registry().call(a);
    }

    /*----------------.
    | is_valid(exp).  |
    `----------------*/

    REGISTER_DEFINE(is_valid_ratexp);

    bool
    is_valid(const ratexp& e)
    {
      return detail::is_valid_ratexp_registry().call(e);
    }
  }
}
