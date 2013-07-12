#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/is-normalized.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  /*----------------.
  | is_normalized.  |
  `----------------*/

  namespace dyn
  {
    REGISTER_DEFINE(is_normalized);

    bool
    is_normalized(const automaton& aut)
    {
      return detail::is_normalized_registry().call(aut->vname(false), aut);
    }
  }
}
