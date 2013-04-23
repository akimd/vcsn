#ifndef VCSN_ALGOS_ACCESSIBLE_HH
# define VCSN_ALGOS_ACCESSIBLE_HH

# include <deque>
# include <map>
# include <set>

# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/fwd.hh>

namespace vcsn
{
  template <class Aut>
  Aut accessible(const Aut& a)
  {
    // FIXME Static assert for label's type
    using automaton_t = Aut;
    using state_t = typename Aut::state_t;

    automaton_t res{a.context()};

    // a.state -> res.state.
    using map = std::map<state_t, state_t>;
    map res_state;
    res_state[a.pre()] = res.pre();
    res_state[a.post()] = res.post();

    // Stack of a.states.
    using stack = std::deque<state_t>;
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
    namespace details
    {
      template <class Aut>
      dyn::automaton
      accessible(const dyn::automaton& aut)
      {
        const auto& a = dynamic_cast<const Aut&>(*aut);
        return make_automaton<Aut>(a.context(), accessible(a));
      }

      REGISTER_DECLARE(accessible,
                       (const dyn::automaton&) -> dyn::automaton);
    }
  }
}

#endif // !VCSN_ALGOS_ACCESSIBLE_HH
