#ifndef VCSN_ALGOS_DETERMINIZE_HH
# define VCSN_ALGOS_DETERMINIZE_HH

# include <stack>
# include <string>
# include <type_traits>
# include <unordered_map>
# include <vector>

# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/dynamic_bitset.hh>

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
                  "requires Boolean weights");

    using automaton_t = Aut;
    using label_t = typename automaton_t::label_t;
    using state_t = typename automaton_t::state_t;

    const auto& letters = *a.labelset();
    automaton_t res{a.context()};

    // Set of states.
    using state_set = dynamic_bitset;

    // successors[SOURCE-STATE][LABEL] = DEST-STATESET.
    using successors_t = std::vector<std::unordered_map<label_t, state_set> >;
    successors_t successors{a.num_all_states()};
    for (auto st : a.all_states())
      for (auto l : letters)
        {
          state_set& ss = successors[st][l];
          ss.resize(a.num_all_states());
          for (auto tr : a.out(st, l))
            ss.set(a.dst_of(tr));
        }

    // Set of final states.
    state_set finals;
    finals.resize(a.num_all_states());
    for (auto t : a.final_transitions())
      finals.set(a.src_of(t));

    // The (input) sets of states waiting to be processed.
    using stack = std::stack<state_set>;
    stack todo;

    // set of input states -> output state.
    using map = std::unordered_map<state_set, state_t>;
    map m;

    // Create a new output state from SS. Insert in the output
    // automaton, in the map, and push in the stack.
    auto push_new_state =
      [&res,&m,&a,&todo,&finals] (const state_set& ss) -> state_t
      {
        state_t r = res.new_state();
        m[ss] = r;

        if (ss.intersects(finals))
          res.set_final(r);

        todo.push(ss);
        return r;
      };

    // The input initial states.
    state_set next;
    next.resize(a.num_all_states());
    for (auto t : a.initial_transitions())
      next.set(a.dst_of(t));
    res.set_initial(push_new_state(next));

    while (!todo.empty())
      {
        auto ss = todo.top();
        todo.pop();
        for (auto l: letters)
          {
            next.reset();
            for (auto s = ss.find_first(); s != ss.npos;
                 s = ss.find_next(s))
              next |= successors[s][l];
            auto i = m.find(next);
            state_t n = (i == m.end()) ? push_new_state(next) : i->second;

            res.add_transition(m[ss], n, l);
          }
      }
    return res;
  }

  /*-------------------.
  | dyn::determinize.  |
  `-------------------*/

  namespace dyn
  {
    namespace detail
    {

      template <typename Aut>
      automaton
      determinize(const automaton& aut)
      {
        const auto& a = dynamic_cast<const Aut&>(*aut);
        return make_automaton(a.context(), determinize(a));
      }

      REGISTER_DECLARE(determinize,
                       (const automaton& aut) -> automaton);
    }
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_DETERMINIZE_HH
