#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/derivation.hh>
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
    derivation(const ratexp& e, const std::string& s, bool breaking)
    {
      return detail::derivation_registry().call(e, s, breaking);
    }

    /*--------------------.
    | derived_term(exp).  |
    `--------------------*/
    REGISTER_DEFINE(derived_term);

    automaton
    derived_term(const ratexp& e, bool breaking)
    {
      return detail::derived_term_registry().call(e, breaking);
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
