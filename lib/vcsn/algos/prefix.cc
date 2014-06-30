#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/prefix.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  /*---------------.
  | prefix/suffix.  |
  `-----------------*/

  namespace dyn
  {
    REGISTER_DEFINE(prefix);

    automaton
    prefix(const automaton& aut)
    {
      return detail::prefix_registry().call(aut);
    }

    REGISTER_DEFINE(suffix);

    automaton
    suffix(const automaton& aut)
    {
      return detail::suffix_registry().call(aut);
    }

    REGISTER_DEFINE(factor);

    automaton
    factor(const automaton& aut)
    {
      return detail::factor_registry().call(aut);
    }

    REGISTER_DEFINE(subword);

    automaton
    subword(const automaton& aut)
    {
      return detail::subword_registry().call(aut);
    }

  }
}
