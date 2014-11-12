#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/constant-term.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*---------------------.
    | constant_term(exp).  |
    `---------------------*/
    REGISTER_DEFINE(constant_term);

    weight
    constant_term(const expression& e)
    {
      return detail::constant_term_registry().call(e);
    }
  }
}
