#ifndef VCSN_ALGOS_DETERMINIZE_HH
# define VCSN_ALGOS_DETERMINIZE_HH

// http://stackoverflow.com/questions/12314763/
// efficient-hashing-of-stdbitset-or-boostdynamic-bitset-for-boosts-unor
# define BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS

# include <boost/dynamic_bitset.hpp>
# include <functional> // std::hash
# include <stack>
# include <string>
# include <type_traits>
# include <unordered_map>
# include <vector>

# include <vcsn/dyn/fwd.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/misc/hash.hh>

namespace std
{
  template <>
  struct hash<boost::dynamic_bitset<>>
  {
    size_t operator()(const boost::dynamic_bitset<>& bitset) const
    {
      size_t res = 0;
      for (auto s : bitset.m_bits)
        hash_combine(res, s);
      return res;
    }
  };
}

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
    using state_set = boost::dynamic_bitset<>;
    using stack = std::stack<state_set>;
    using map = std::unordered_map<state_set, state_t>;
    using successors_t = std::vector<std::unordered_map<label_t, state_set> >;

    const auto& letters = *a.labelset();
    automaton_t res{a.context()};
    // The stack of (input) sets of states waiting to be processed.
    stack st;
    // set of input states -> output state.
    map m;
    // Create a new output state from SS. Insert in the output
    // automaton, in the map, and push in the stack.

    successors_t successors;
    successors.resize(a.num_states() + 2);
    for (auto st : a.all_states())
      for (auto l : letters)
        {
          state_set ss;
          ss.resize(a.num_states() + 2);
          for (auto tr : a.out(st, l))
              ss.set(a.dst_of(tr));
          successors[st][l] = ss;
        }

    state_set finals;
    finals.resize(a.num_states() + 2);
    for (auto t : a.final_transitions())
      finals.set(a.src_of(t));

    auto push_new_state =
      [&res,&m,&a,&st,&finals] (const state_set& ss) -> state_t
      {
        state_t r = res.new_state();
        m[ss] = r;

        if (ss.intersects(finals))
          res.set_final(r);

        st.push(ss);
        return r;
      };

    // The input initial states.
    state_set next;
    next.resize(a.num_states() + 2);
    for (auto t : a.initial_transitions())
      next.set(a.dst_of(t));
    res.set_initial(push_new_state(next));

    while (!st.empty())
      {
        auto ss = st.top();
        st.pop();
        for (auto l: letters)
          {
            next.reset();
            for (auto s = ss.find_first(); s != boost::dynamic_bitset<>::npos;
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
    namespace details
    {

      template <typename Aut>
      dyn::automaton
      determinize(const dyn::automaton& aut)
      {
        const auto& a = dynamic_cast<const Aut&>(*aut);
        return make_automaton<Aut>(a.context(), determinize(a));
      }

      using determinize_t =
        auto (const dyn::automaton& aut) -> dyn::automaton;
      bool determinize_register(const std::string& ctx,
                                const determinize_t& fn);
    }
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_DETERMINIZE_HH
