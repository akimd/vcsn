#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/synchronizing-word.hh>
#include <vcsn/dyn/algos.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(pair);
    automaton
    pair(const automaton& aut)
    {
      return detail::pair_registry().call(aut);
    }

    REGISTER_DEFINE(synchronizing_word);
    std::string
    synchronizing_word(const automaton& aut)
    {
      return detail::synchronizing_word_registry().call(aut);
    }

    REGISTER_DEFINE(is_synchronized_by);
    bool
    is_synchronized_by(const automaton& aut, const std::string& word)
    {
      return detail::is_synchronized_by_registry().call(aut, word);
    }
  }
}
