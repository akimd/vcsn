#include <vcsn/dyn/algos.hh>
#include <vcsn/algos/insplitting.hh>
#include <lib/vcsn/algos/registry.hh>


namespace vcsn
{
  namespace dyn
  {

    /*----------------.
    | insplit(aut).  |
    `----------------*/
    REGISTER_DEFINE(insplit);

    automaton
    insplit(const automaton& a)
    {
      return detail::insplit_registry().call(a);
    }
  }
}

