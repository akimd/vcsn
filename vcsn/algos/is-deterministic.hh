#ifndef VCSN_ALGOS_IS_DETERMINISTIC_HH
# define VCSN_ALGOS_IS_DETERMINISTIC_HH

# include <queue>
# include <unordered_set>

# include <vcsn/ctx/traits.hh>
# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/dyn/automaton.hh>

namespace vcsn
{

  /// Whether state \s is deterministic.
  template <typename Aut>
  inline bool
  is_deterministic(const Aut& aut, state_t_of<Aut> s)
  {
    using automaton_t = Aut;
    static_assert(labelset_t_of<automaton_t>::is_free(),
                  "requires labels_are_letters");

    using label_t = label_t_of<automaton_t>;
    std::unordered_set<label_t> seen;
    for (auto t : aut->all_out(s))
      if (!seen.insert(aut->label_of(t)).second)
        return false;
    return true;
  }

  /// Number of non-deterministic states.
  template <class Aut>
  inline size_t
  num_deterministic_states(const Aut& aut)
  {
    static_assert(labelset_t_of<Aut>::is_free(),
                  "requires labels_are_letters");

    size_t res = 0;
    for (auto s: aut->states())
      res += is_deterministic(aut, s);
    return res;
  }

  /// Whether has at most an initial state, and all its states
  /// are deterministic.
  template <class Aut>
  inline bool
  is_deterministic(const Aut& aut)
  {
    static_assert(labelset_t_of<Aut>::is_free(),
                  "requires labels_are_letters");

    if (1 < aut->initial_transitions().size())
      return false;

    for (auto s: aut->states())
      if (!is_deterministic(aut, s))
        return false;
    return true;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      bool
      is_deterministic(const automaton& aut)
      {
        return is_deterministic(aut->as<Aut>());
      }

      REGISTER_DECLARE(is_deterministic,
                       (const automaton& aut) -> bool);
    }
  }
} // namespace vscn

#endif // !VCSN_ALGOS_IS_DETERMINISTIC_HH
