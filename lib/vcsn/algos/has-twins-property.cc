#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/has-twins-property.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  namespace dyn
  {

    REGISTER_DEFINE(is_cycle_unambiguous);

    bool
    is_cycle_unambiguous(const automaton& aut)
    {
      return detail::is_cycle_unambiguous_registry().call(aut);
    }


    REGISTER_DEFINE(has_twins_property);

    bool
    has_twins_property(const automaton& aut)
    {
      return detail::has_twins_property_registry().call(aut);
    }

    REGISTER_DEFINE(inverse);

    automaton
    inverse(const automaton& aut)
    {
      return detail::inverse_registry().call(aut);
    }
  }
}
