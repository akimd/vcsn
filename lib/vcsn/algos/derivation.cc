#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/derivation.hh>
#include <vcsn/algos/first-order.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*--------------------------.
    | derivation(exp, string).  |
    `--------------------------*/
    REGISTER_DEFINE(derivation);

    polynomial
    derivation(const ratexp& e, const label& l, bool breaking)
    {
      return detail::derivation_registry().call(e, l, breaking);
    }

    /*--------------------.
    | derived_term(exp).  |
    `--------------------*/
    REGISTER_DEFINE(derived_term);

    automaton
    derived_term(const ratexp& e, bool breaking)
    {
      return getenv("VCSN_LINEAR")
        ? linear(e)
        : detail::derived_term_registry().call(e, breaking);
    }

    /*--------------.
    | linear(exp).  |
    `--------------*/

    REGISTER_DEFINE(linear);
    automaton
    linear(const ratexp& e, bool use_spontaneous)
    {
      return detail::linear_registry().call(e, use_spontaneous);
    }

    /*---------------------------.
    | first_order(exp, string).  |
    `---------------------------*/
    REGISTER_DEFINE(first_order);

    expansion
    first_order(const ratexp& e, bool use_spontaneous)
    {
      return detail::first_order_registry().call(e, use_spontaneous);
    }

    /*-------------.
    | split(exp).  |
    `-------------*/
    REGISTER_DEFINE(split);

    polynomial
    split(const ratexp& e)
    {
      return detail::split_registry().call(e);
    }
  }
}
