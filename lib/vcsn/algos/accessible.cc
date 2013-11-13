#include <vcsn/algos/accessible.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(accessible);

    automaton
    accessible(const automaton& aut)
    {
      return detail::accessible_registry().call(aut);
    }

    REGISTER_DEFINE(coaccessible);

    automaton
    coaccessible(const automaton& aut)
    {
      return detail::coaccessible_registry().call(aut);
    }

    REGISTER_DEFINE(trim);

    automaton
    trim(const automaton& aut)
    {
      return detail::trim_registry().call(aut);
    }

    REGISTER_DEFINE(is_trim);

    bool
    is_trim(const automaton& aut)
    {
      return detail::is_trim_registry().call(aut);
    }

    REGISTER_DEFINE(is_useless);

    bool
    is_useless(const automaton& aut)
    {
      return detail::is_useless_registry().call(aut);
    }

    REGISTER_DEFINE(is_empty);

    bool
    is_empty(const automaton& aut)
    {
      return detail::is_empty_registry().call(aut);
    }

  }
}
