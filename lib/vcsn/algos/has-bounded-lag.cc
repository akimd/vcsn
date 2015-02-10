#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/has-bounded-lag.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  namespace dyn
  {

    REGISTER_DEFINE(has_bounded_lag);

    bool
    has_bounded_lag(const automaton& aut)
    {
      return detail::has_bounded_lag_registry().call(aut);
    }
  }
}
