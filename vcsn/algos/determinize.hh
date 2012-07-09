#ifndef VCSN_ALGOS_DETERMINIZE_HH
# define VCSN_ALGOS_DETERMINIZE_HH

# include <set>
# include <stack>
# include <map>

namespace vcsn
{

  template <class Aut>
  inline
  Aut
  determinize(const Aut& a)
  {
    using automaton_t = Aut;
    using state_t = typename automaton_t::state_t;
    using state_set = std::set<state_t>;
    using stack = std::stack<state_set>;
    using map = std::map<state_set, state_t>;

    state_set initial;
    for (auto t : a.initial_transitions())
      initial.insert(a.dst_of(t));

    automaton_t res{a.context()};
    if (initial.empty())
      return res;

    stack st;
    map m;
    // Create a new state. Insert in the output automaton, in the
    // map, and push in the stack.
    auto push_new_state =
      [&res,&m,&a,&st] (state_set ss) -> state_t
      {
        state_t r = res.new_state();
        m[ss] = r;

        for (auto e : ss)
          if (a.is_final(e))
            {
              res.set_final(r);
              break;
            }
        st.push(ss);
        return r;
      };

    res.set_initial(push_new_state(initial));

    while (!st.empty())
      {
        auto ss = st.top();
        st.pop();
        const auto& genset = *a.genset();
        for (auto gen : genset)
          {
            state_set next;
            for (auto s : ss)
              for (auto t : a.out(s, gen))
                next.insert(a.dst_of(t));
            auto i = m.find(next);
            state_t n = (i == m.end()) ? push_new_state(next) : i->second;

            res.add_transition(m[ss], n, gen);
          }
      }
    return res;
  }
} // namespace vcsn

#endif // !VCSN_ALGOS_DETERMINIZE_HH
