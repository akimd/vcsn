#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/star.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(star);

    automaton
    star(const automaton& aut)
    {
      return detail::star_registry().call(aut);
    }
  }
}
