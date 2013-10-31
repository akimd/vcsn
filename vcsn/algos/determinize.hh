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

  /*----------------------.
  | subset construction.  |
  `----------------------*/
  namespace detail
  {
    template <typename Aut>
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

      /// The determinized automaton.
      /// \param a         the automaton to determinize
      /// \param complete  whether to force the result to be complete
      automaton_nocv_t operator()(const automaton_t& a, bool complete = false)
      {
        clear();

        // We use state numbers as indexes, so we need to know the
        // last state number.  If states were removed, it is not the
        // same as the number of states.
        size_t state_size = a.all_states().back() + 1;

        const auto& letters = *a.labelset();
        automaton_nocv_t res{a.context()};

        // successors[SOURCE-STATE][LABEL] = DEST-STATESET.
        using successors_t
          = std::vector<std::unordered_map<label_t, state_set>>;
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
                // Don't generate the sink.
                if (complete || next.any())
                  {
                    auto i = map_.find(next);
                    state_t n = ((i == map_.end())
                                 ? push_new_state(next)
                                 : i->second);
                    res.add_transition(map_[ss], n, l);
                  }
              }
          }
        return std::move(res);
      }

      /// A map from determinized states to sets of original states.
      using origins_t = std::map<state_t, std::set<state_t>>;
      origins_t
      origins() const
      {
        origins_t res;
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

      /// Print the origins.
      static
      std::ostream&
      print(std::ostream& o, const origins_t& orig)
      {
        o << "/* Origins." << std::endl
          << "    node [shape = box, style = rounded]" << std::endl;
        for (auto p : orig)
          {
            o << "    " << p.first - 2
              << " [label = \"";
            const char* sep = "";
            for (auto s: p.second)
              {
                o << sep << s - 2;
                sep = ",";
              }
            o << "\"]" << std::endl;
          }
        o << "*/" << std::endl;
        return o;
      }
    };
  }

  template <typename Aut>
  inline
  Aut
  determinize(const Aut& a, bool complete = false)
  {
    detail::determinizer<Aut> determinize;
    auto res = determinize(a, complete);
    // FIXME: Not absolutely elegant.  But currently no means to
    // associate meta-data to states.
    if (getenv("VCSN_ORIGINS"))
      determinize.print(std::cout, determinize.origins());
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
      determinize(const automaton& aut, bool complete)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(determinize(a, complete));
      }

      REGISTER_DECLARE(determinize,
                       (const automaton& aut, bool complete) -> automaton);
    }
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_DETERMINIZE_HH
