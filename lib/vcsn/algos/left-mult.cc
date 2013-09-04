#include <string>

#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/left-mult.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  namespace dyn
  {
    std::string
    vname(const automaton& aut, const weight& w)
    {
      return aut->vname(false) + " x " + w->vname(false);
    }

    REGISTER_DEFINE2(left_mult);

    automaton
    left_mult(const automaton& aut, const weight& w)
    {
      return detail::left_mult_registry().call(vname(aut, w),
                                               aut, w);
    }

    REGISTER_DEFINE2(right_mult);

    automaton
    right_mult(const automaton& aut, const weight& w)
    {
      return detail::right_mult_registry().call(vname(aut, w),
                                                aut, w);
    }
  }
}
