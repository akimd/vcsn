#ifndef VCSN_ALGOS_IS_DETERMINISTIC_HH
# define VCSN_ALGOS_IS_DETERMINISTIC_HH

# include <queue>
# include <unordered_set>

# include <vcsn/ctx/traits.hh>
# include <vcsn/dyn/automaton.hh>

namespace vcsn
{

  /// Whether state \a s is deterministic in \a aut.
  template <typename Aut>
  inline bool
  is_sequential(const Aut& aut, state_t_of<Aut> s)
  {
    using automaton_t = Aut;
    static_assert(labelset_t_of<automaton_t>::is_free(),
                  "is_deterministic: requires free labelset");

    using label_t = label_t_of<automaton_t>;
    std::unordered_set<label_t> seen;
    for (auto t : aut->all_out(s))
      if (!seen.insert(aut->label_of(t)).second)
        return false;
    return true;
  }

  /// Number of non-sequential states.
  template <class Aut>
  inline size_t
  num_sequential_states(const Aut& aut)
  {
    size_t res = 0;
    for (auto s: aut->states())
      res += is_sequential(aut, s);
    return res;
  }

  /// Whether has at most an initial state, and all its states
  /// are sequential.
  template <class Aut>
  inline bool
  is_sequential(const Aut& aut)
  {
    if (1 < aut->initial_transitions().size())
      return false;

    for (auto s: aut->states())
      if (!is_sequential(aut, s))
        return false;
    return true;
  }

  /// Number of non-deterministic states.
  template <class Aut>
  inline size_t
  num_deterministic_states(const Aut& aut)
  {
    static_assert(labelset_t_of<Aut>::is_free(),
                  "num_deterministic_states: requires free labelset");

    return num_sequential_states(aut);
  }

  /// Whether has at most an initial state, and all its states
  /// are deterministic.
  template <class Aut>
  inline bool
  is_deterministic(const Aut& aut)
  {
    static_assert(labelset_t_of<Aut>::is_free(),
                  "is_deterministic: requires free labelset");

    return is_sequential(aut);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      bool
      is_sequential(const automaton& aut)
      {
        return is_sequential(aut->as<Aut>());
      }

      template <typename Aut>
      bool
      is_deterministic(const automaton& aut)
      {
        return is_sequential(aut->as<Aut>());
      }

      REGISTER_DECLARE(is_sequential,
                       (const automaton& aut) -> bool);
      REGISTER_DECLARE(is_deterministic,
                       (const automaton& aut) -> bool);
    }
  }
} // namespace vscn

#endif // !VCSN_ALGOS_IS_DETERMINISTIC_HH
