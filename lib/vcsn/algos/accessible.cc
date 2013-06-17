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
      return detail::accessible_registry().call(aut->vname(),
                                                  aut);
    }

    REGISTER_DEFINE(coaccessible);

    automaton
    coaccessible(const automaton& aut)
    {
      return detail::coaccessible_registry().call(aut->vname(),
                                                  aut);
    }

    REGISTER_DEFINE(trim);

    automaton
    trim(const automaton& aut)
    {
      return detail::trim_registry().call(aut->vname(),
                                          aut);
    }
  }
}
