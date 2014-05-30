#include <vcsn/algos/dot.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*------.
    | dot.  |
    `------*/

    REGISTER_DEFINE(dot);

    std::ostream&
    dot(const automaton& aut, std::ostream& out, bool dot2tex)
    {
      detail::dot_registry().call(aut, out, dot2tex);
      return out;
    }
  }
}
