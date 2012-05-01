#ifndef VCSN_ALGOS_DETERMINIZE_HH
# define VCSN_ALGOS_DETERMINIZE_HH

# include <set>
# include <utility>
# include <stack>
# include <map>

namespace vcsn {

  template <class Aut>
  struct determinize_functor
  {
    using automaton_t = Aut;
    using state_t = typename automaton_t::state_t;
    using state_set = std::set<state_t>;
    using stack = std::stack<state_set>;
    using map = std::map<state_set, state_t>;

    automaton_t operator()(const automaton_t& a)
    {
      state_set initial;
      for (auto t : a.initial_transitions())
        initial.insert(a.dst_of(t));

      automaton_t out(a.genset(), a.weightset());
      if (initial.empty()) // No initial state
        return out;
      // Lambda functions
      // Set the state final in the automaton `out' if necessary.
      auto set_final = [&] (state_set s, state_t st) {
        for (auto e : s)
          {
            if (a.is_final(e))
              {
                out.set_final(st);
                return;
              }
          }
      };

      // Create the initial state
      state_t init = out.new_state();
      out.set_initial(init);
      set_final(initial, init);

      // Create the stack and the map
      stack st;
      map m;
      // Push the initial state on the stack and the map
      m[initial] = init;
      st.push(initial);

      while (!st.empty())
        {
          auto elt = st.top();
          st.pop();
          const auto& genset = a.genset();
          // For each label
          for (auto label : genset)
            {
              state_set states;
              // For each element
              for (auto st : elt)
                // For each output transition
                for (auto tr : a.out(st, label))
                  states.insert(a.dst_of(tr));
              // If the state doesn't exist in the map.
              // Create it.
              if (m.end() == m.find(states))
                {
                  m[states] = out.new_state();
                  set_final(states, m[states]);
                  st.push(states);
                }
              out.add_transition(m[elt], m[states], label);
            }
        }
      return out;
    }
  };

  template <class Aut>
  inline
  Aut
  determinize(const Aut& aut)
  {
    determinize_functor<Aut> df;
    return df(aut);
  }
} // namespace vcsn

#endif // !VCSN_ALGOS_DETERMINIZE_HH
