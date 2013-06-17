#ifndef VCSN_ALGOS_ACCESSIBLE_HH
# define VCSN_ALGOS_ACCESSIBLE_HH

# include <deque>
# include <queue>
# include <map>
# include <set>

# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/fwd.hh>

namespace vcsn
{

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
    if (set.find(a.post()) != end(set))
      res -= 1;
    return res;
  }

  template <typename Aut>
  Aut accessible(const Aut& a)
  {
    using automaton_t = Aut;
    using state_t = typename automaton_t::state_t;

    automaton_t res{a.context()};

    // a.state -> res.state.
    using map = std::map<state_t, state_t>;
    map res_state;
    res_state[a.pre()] = res.pre();
    res_state[a.post()] = res.post();

    // Stack of a.states.
    using stack = std::deque<state_t>; // FIXME: Appropriate data type?
    stack todo;
    todo.push_back(a.pre());

    while (!todo.empty())
    {
      const state_t asrc = todo.front();
      todo.pop_front();

      for (auto tr : a.all_out(asrc))
      {
        state_t adst = a.dst_of(tr);
        // If the dst state does not exist in res, create it.
        auto p = res_state.emplace(adst, a.null_state());
        if (p.second)
        {
          todo.push_back(adst);
          p.first->second = res.new_state();
        }

        res.add_transition(res_state[asrc], p.first->second,
                           a.label_of(tr), a.weight_of(tr));
      }
    }

    return res;
  }

  /*-----------------.
  | dyn::accessible. |
  `-----------------*/

  namespace dyn
  {
    namespace detail
    {
      template <class Aut>
      automaton
      accessible(const automaton& aut)
      {
        const auto& a = dynamic_cast<const Aut&>(*aut);
        return make_automaton(a.context(), accessible(a));
      }

      REGISTER_DECLARE(accessible,
                       (const automaton&) -> automaton);
    }
  }
}

#endif // !VCSN_ALGOS_ACCESSIBLE_HH
