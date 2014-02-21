#include <vcsn/dyn/algos.hh>
#include <vcsn/algos/outsplitting.hh>
#include <lib/vcsn/algos/registry.hh>


namespace vcsn
{
  namespace dyn
  {

    /*----------------.
    | outsplit(aut).  |
    `----------------*/
    REGISTER_DEFINE(outsplit);

    automaton
    outsplit(const automaton& a)
    {
      return detail::outsplit_registry().call(a);
    }
  }
}
