#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/synchronizing-word.hh>
#include <vcsn/dyn/algos.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(pair);
    automaton
    pair(const automaton& aut, bool keep_initials)
    {
      return detail::pair_registry().call(aut, keep_initials);
    }

    REGISTER_DEFINE(synchronizing_word);
    label
    synchronizing_word(const automaton& aut, const std::string& algo)
    {
      return detail::synchronizing_word_registry().call(aut, algo);
    }

    REGISTER_DEFINE(is_synchronized_by);
    bool
    is_synchronized_by(const automaton& aut, const label& word)
    {
      return detail::is_synchronized_by_registry().call(aut, word);
    }

    REGISTER_DEFINE(is_synchronizing);
    bool
    is_synchronizing(const automaton& aut)
    {
      return detail::is_synchronizing_registry().call(aut);
    }

    REGISTER_DEFINE(cerny);
    automaton
    cerny(const context& aut, unsigned num_states)
    {
      return detail::cerny_registry().call(aut, num_states);
    }
  }
}
