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

    REGISTER_DEFINE2(dot);

    std::ostream&
    dot(const automaton& aut, std::ostream& out)
    {
      detail::dot_registry().call(aut, out);
      return out;
    }
  }
}
