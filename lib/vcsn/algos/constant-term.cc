#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/constant-term.hh>
#include <vcsn/algos/derive.hh>
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
    constant_term(const ratexp& e)
    {
      return detail::constant_term_registry().call(e->vname(false),
                                                   e);
    }

    /*--------------.
    | derive(exp).  |
    `--------------*/
    REGISTER_DEFINE(derive);

    ratexp
    derive(const ratexp& e, const std::string& s)
    {
      return detail::derive_registry().call(e->vname(false),
                                            e, s);
    }
  }
}
