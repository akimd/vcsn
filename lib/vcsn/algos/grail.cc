#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/grail.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*--------.
    | grail.  |
    `--------*/

    REGISTER_DEFINE(grail);

    std::ostream&
    grail(const automaton& aut, std::ostream& out)
    {
      detail::grail_registry().call(aut, out);
      return out;
    }
  }
}
