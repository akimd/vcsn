#ifndef VCSN_ALGOS_DETERMINIZE_HH
# define VCSN_ALGOS_DETERMINIZE_HH

# include <set>
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
  namespace detail
  {
    template <class Aut>
    class determinizer
    {
      static_assert(Aut::context_t::is_lal,
                    "requires labels_are_letters");
      static_assert(std::is_same<typename Aut::weight_t, bool>::value,
                    "requires Boolean weights");

      using automaton_t = Aut;
      using automaton_nocv_t = typename automaton_t::self_nocv_t;
      using label_t = typename automaton_t::label_t;
      using state_t = typename automaton_t::state_t;

      // Set of (input) states.
      using state_set = dynamic_bitset;

      // Set of input states -> output state.
      using map = std::unordered_map<state_set, state_t>;
      map map_;

    public:
      void clear()
      {
        map_.clear();
      }

      /// Determinize \a a, and return the result.
      automaton_nocv_t operator()(const automaton_t& a)
      {
        clear();

        // We use state numbers as indexes, so we need to know the
        // last state number.  If states were removed, it is not the
        // same as the number of states.
        size_t state_size = a.all_states().back() + 1;

        const auto& letters = *a.labelset();
        automaton_nocv_t res{a.context()};

        // successors[SOURCE-STATE][LABEL] = DEST-STATESET.
        using successors_t = std::vector<std::unordered_map<label_t, state_set> >;
        successors_t successors{state_size};
        for (auto st : a.all_states())
          for (auto l : letters)
            {
              state_set& ss = successors[st][l];
              ss.resize(state_size);
              for (auto tr : a.out(st, l))
                ss.set(a.dst_of(tr));
            }

        // Set of final states.
        state_set finals;
        finals.resize(state_size);
        for (auto t : a.final_transitions())
          finals.set(a.src_of(t));

        // The (input) sets of states waiting to be processed.
        using stack = std::stack<state_set>;
        stack todo;

        // Create a new output state from SS. Insert in the output
        // automaton, in the map, and push in the stack.
        auto push_new_state =
          [this,&res,&todo,&finals] (const state_set& ss) -> state_t
          {
            state_t r = res.new_state();
            map_[ss] = r;

            if (ss.intersects(finals))
              res.set_final(r);

            todo.push(ss);
            return r;
          };

        // The input initial states.
        state_set next;
        next.resize(state_size);
        for (auto t : a.initial_transitions())
          next.set(a.dst_of(t));
        res.set_initial(push_new_state(next));

        while (!todo.empty())
          {
            auto ss = std::move(todo.top());
            todo.pop();
            for (auto l: letters)
              {
                next.reset();
                for (auto s = ss.find_first(); s != ss.npos;
                     s = ss.find_next(s))
                  next |= successors[s][l];
                auto i = map_.find(next);
                state_t n = (i == map_.end()) ? push_new_state(next) : i->second;

                res.add_transition(map_[ss], n, l);
              }
          }
        return std::move(res);
      }

      /// A map from determinized states to sets of original states.
      std::map<state_t, std::set<state_t>>
      origins() const
      {
        std::map<state_t, std::set<state_t>> res;
        for (const auto& p: map_)
          {
            std::set<state_t> from;
            const auto& ss = p.first;
            for (auto s = ss.find_first(); s != ss.npos;
                 s = ss.find_next(s))
              from.emplace(s);
            res.emplace(p.second, std::move(from));
          }
        return res;
      }
    };
  }

  template <class Aut>
  inline
  Aut
  determinize(const Aut& a)
  {
    detail::determinizer<Aut> determinize;
    return determinize(a);
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
