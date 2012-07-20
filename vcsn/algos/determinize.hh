#ifndef VCSN_ALGOS_DETERMINIZE_HH
# define VCSN_ALGOS_DETERMINIZE_HH

# include <map>
# include <set>
# include <stack>
# include <type_traits>

# include <vcsn/core/kind.hh>

namespace vcsn
{

  template <class Aut>
  inline
  Aut
  determinize(const Aut& a)
  {
    static_assert(Aut::context_t::is_lal,
                  "requires labels_are_letters");
    static_assert(std::is_same<typename Aut::weight_t, bool>::value,
                  "requires Boolean labels");

    using automaton_t = Aut;
    using state_t = typename automaton_t::state_t;
    using state_set = std::set<state_t>;
    using stack = std::stack<state_set>;
    using map = std::map<state_set, state_t>;

    const auto& letters = *a.genset();
    automaton_t res{a.context()};
    // The stack of (input) sets of states waiting to be processed.
    stack st;
    // set of input states -> output state.
    map m;
    // Create a new output state from SS. Insert in the output
    // automaton, in the map, and push in the stack.
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

    // The input initial states.
    state_set initial;
    for (auto t : a.initial_transitions())
      initial.insert(a.dst_of(t));
    res.set_initial(push_new_state(initial));

    while (!st.empty())
      {
        auto ss = st.top();
        st.pop();
        for (auto l: letters)
          {
            state_set next;
            for (auto s : ss)
              for (auto t : a.out(s, l))
                next.insert(a.dst_of(t));
            auto i = m.find(next);
            state_t n = (i == m.end()) ? push_new_state(next) : i->second;

            res.add_transition(m[ss], n, l);
          }
      }
    return res;
  }
} // namespace vcsn

#endif // !VCSN_ALGOS_DETERMINIZE_HH
