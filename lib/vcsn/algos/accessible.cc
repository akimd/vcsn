#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/registers.hh>

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

    REGISTER_DEFINE(is_accessible);

    bool
    is_accessible(const automaton& aut)
    {
      return detail::is_accessible_registry().call(aut);
    }

    REGISTER_DEFINE(is_coaccessible);

    bool
    is_coaccessible(const automaton& aut)
    {
      return detail::is_coaccessible_registry().call(aut);
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
