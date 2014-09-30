#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/derivation.hh>
#include <vcsn/algos/derived-term.hh>
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
    derived_term(const ratexp& e, const std::string& algo)
    {
      return detail::derived_term_registry().call(e, algo);
    }

    /*-------------------.
    | first_order(exp).  |
    `-------------------*/
    REGISTER_DEFINE(first_order);

    expansion
    first_order(const ratexp& e)
    {
      return detail::first_order_registry().call(e);
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

    /*---------------------.
    | split(polynomial).   |
    `---------------------*/
    REGISTER_DEFINE(split_polynomial);

    polynomial
    split(const polynomial& p)
    {
      return detail::split_polynomial_registry().call(p);
    }
  }
}
