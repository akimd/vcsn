#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/is-ambiguous.hh>
#include <vcsn/dyn/algos.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(ambiguous_word);
    label
    ambiguous_word(const automaton& aut)
    {
      return detail::ambiguous_word_registry().call(aut);
    }

    REGISTER_DEFINE(is_ambiguous);
    bool
    is_ambiguous(const automaton& aut)
    {
      return detail::is_ambiguous_registry().call(aut);
    }
  }
}
