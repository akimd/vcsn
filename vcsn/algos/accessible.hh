#ifndef VCSN_ALGOS_ACCESSIBLE_HH
# define VCSN_ALGOS_ACCESSIBLE_HH

# include <deque>
# include <queue>
# include <map>

# include <vcsn/algos/copy.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/attributes.hh>
# include <vcsn/misc/set.hh>

namespace vcsn
{

  /*--------------------------------------------------.
  | Sets of accessible, coaccessible, useful states.  |
  `--------------------------------------------------*/

  // The set of accessible states, including pre(), and possibly post().
  template <typename Aut>
  std::set<typename Aut::state_t>
  accessible_states(const Aut& a)
  {
    using automaton_t = Aut;
    using state_t = typename automaton_t::state_t;

    // Reachable states.
    std::set<state_t> res;
    res.emplace(a.pre());

    // States work list.
    using worklist_t = std::queue<state_t>;
    worklist_t todo;
    todo.emplace(a.pre());

    while (!todo.empty())
    {
      const state_t src = todo.front();
      todo.pop();

      for (auto tr : a.all_out(src))
      {
        state_t dst = a.dst_of(tr);
        // If we have not seen it already, explore its successors.
        if (res.emplace(dst).second)
          todo.emplace(dst);
      }
    }

    return res;
  }

  // The set of coaccessible states, including post(), and possibly pre().
  template <typename Aut>
  std::set<typename Aut::state_t>
  coaccessible_states(const Aut& a)
  {
    return accessible_states(transpose(a));
  }

  // The set of coaccessible states, including post(), and possibly pre().
  template <typename Aut>
  std::set<typename Aut::state_t>
  useful_states(const Aut& a)
  {
    auto accessible = accessible_states(a);
    auto coaccessible = coaccessible_states(a);
    return intersection(accessible, coaccessible);
  }


  /*----------------------------------------------------.
  | Number of accessible, coaccessible, useful states.  |
  `----------------------------------------------------*/

  /// Number of accessible states, not counting pre() and post().
  template <typename Aut>
  size_t
  num_accessible_states(const Aut& a)
  {
    auto set = accessible_states(a);
    size_t res = set.size();
    // Don't count pre().
    res -= 1;
    // Don't count post().
    if (has(set, a.post()))
      res -= 1;
    return res;
  }

  /// Number of accessible states, not counting pre() and post().
  template <typename Aut>
  size_t
  num_coaccessible_states(const Aut& a)
  {
    return num_accessible_states(transpose(a));
  }

  /// Number of accessible states, not counting pre() and post().
  template <typename Aut>
  size_t
  num_useful_states(const Aut& a)
  {
    auto set = useful_states(a);
    size_t res = set.size();
    // Don't count pre().
    if (has(set, a.pre()))
      res -= 1;
    // Don't count post().
    if (has(set, a.post()))
      res -= 1;
    return res;
  }


  /*-----------------------------------------------.
  | accessible, coaccessible, useful subautomata.  |
  `-----------------------------------------------*/

  template <typename Aut>
  Aut accessible(const Aut& a)
  {
    return vcsn::copy(a, accessible_states(a));
  }

  template <typename Aut>
  Aut coaccessible(const Aut& a)
  {
    return vcsn::copy(a, coaccessible_states(a));
  }

  template <typename Aut>
  Aut trim(const Aut& a)
  {
    return vcsn::copy(a, useful_states(a));
  }

  /*--------------------------------.
  | is_trim, is_empty, is_useless.  |
  `--------------------------------*/

  template <typename Aut>
  bool is_trim(const Aut& a)
  {
    return num_useful_states(a) == a.num_states();
  }

  template <typename Aut>
  bool is_useless(const Aut& a)
  {
    return num_useful_states(a) == 0;
  }

  template <typename Aut>
  bool is_empty(const Aut& a) ATTRIBUTE_PURE;

  template <typename Aut>
  bool is_empty(const Aut& a)
  {
    // FIXME: Beware of the case where there is a transition from
    // pre() to post().
    return a.num_states() == 0;
  }

  namespace dyn
  {
    namespace detail
    {

      /*------------------.
      | dyn::accessible.  |
      `------------------*/

      template <typename Aut>
      automaton
      accessible(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(accessible(a));
      }

      REGISTER_DECLARE(accessible,
                       (const automaton&) -> automaton);

      /*--------------------.
      | dyn::coaccessible.  |
      `--------------------*/

      template <typename Aut>
      automaton
      coaccessible(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(coaccessible(a));
      }

      REGISTER_DECLARE(coaccessible,
                       (const automaton&) -> automaton);

      /*------------.
      | dyn::trim.  |
      `------------*/

      template <typename Aut>
      automaton
      trim(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(trim(a));
      }

      REGISTER_DECLARE(trim,
                       (const automaton&) -> automaton);

      /*---------------.
      | dyn::is_trim.  |
      `---------------*/

      template <typename Aut>
      bool
      is_trim(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return is_trim(a);
      }

      REGISTER_DECLARE(is_trim,
                       (const automaton&) -> bool);

      /*------------------.
      | dyn::is_useless.  |
      `------------------*/

      template <typename Aut>
      bool
      is_useless(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return is_useless(a);
      }

      REGISTER_DECLARE(is_useless,
                       (const automaton&) -> bool);

      /*----------------.
      | dyn::is_empty.  |
      `----------------*/

      template <typename Aut>
      bool
      is_empty(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return is_empty(a);
      }

      REGISTER_DECLARE(is_empty,
                       (const automaton&) -> bool);
    }
  }
}

#endif // !VCSN_ALGOS_ACCESSIBLE_HH
