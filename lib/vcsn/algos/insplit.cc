#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/registers.hh>
#include <lib/vcsn/algos/registry.hh>


namespace vcsn
{
  namespace dyn
  {

    /*----------------.
    | insplit(aut).   |
    `----------------*/
    REGISTER_DEFINE(insplit);

    automaton
    insplit(const automaton& a)
    {
      return detail::insplit_registry().call(a);
    }
  }
}
