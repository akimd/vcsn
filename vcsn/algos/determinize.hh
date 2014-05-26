#ifndef VCSN_ALGOS_DETERMINIZE_HH
# define VCSN_ALGOS_DETERMINIZE_HH

# include <set>
# include <stack>
# include <string>
# include <type_traits>
# include <unordered_map>
# include <vector>

# include <vcsn/ctx/traits.hh>
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
    /// \brief Determinize an automaton.
    ///
    /// \tparam AutPtr an automaton type.
    template <typename AutPtr>
    class determinizer
    {
      static_assert(labelset_t_of<AutPtr>::is_free(),
                    "determinize: requires free labelset");
      static_assert(std::is_same<weight_t_of<AutPtr>, bool>::value,
                    "determinize: requires Boolean weights");

      using automaton_ptr = AutPtr;
      using automaton_t = typename automaton_ptr::element_type;
      using automaton_nocv_t = std::shared_ptr<typename automaton_t::self_nocv_t>;
      using label_t = label_t_of<automaton_t>;
      using state_t = state_t_of<automaton_t>;

      /// Set of (input) states.
      using state_set = dynamic_bitset;

      /// Set of input states -> output state.
      using map = std::unordered_map<state_set, state_t>;
      map map_;

      /// Input automaton.
      automaton_ptr aut_;

      /// Whether to build a complete automaton.
      bool complete_ = false;

      /// We use state numbers as indexes, so we need to know the last
      /// state number.  If states were removed, it is not the same as
      /// the number of states.
      size_t state_size_ = aut_->all_states().back() + 1;

      /// The sets of (input) states waiting to be processed.
      using stack = std::stack<state_set>;
      stack todo_;

      /// Set of final states in the input automaton.
      state_set finals_;

      /// The result.
      automaton_nocv_t res_;

    public:
      /// Build the determinizer.
      /// \param a         the automaton to determinize
      /// \param complete  whether to force the result to be complete
      determinizer(const automaton_ptr& a, bool complete = false)
        : aut_(a)
        , complete_(complete)
        , finals_(state_size_)
        , res_(std::make_shared<typename automaton_nocv_t::element_type>(aut_->context()))
      {
        for (auto t : aut_->final_transitions())
          finals_.set(aut_->src_of(t));
      }

      void clear()
      {
        map_.clear();
      }

      /// The determinized automaton.
      automaton_nocv_t operator()()
      {
        clear();

        const auto& letters = aut_->labelset()->genset();

        // successors[SOURCE-STATE][LABEL] = DEST-STATESET.
        using successors_t
          = std::vector<std::unordered_map<label_t, state_set>>;
        successors_t successors{state_size_};
        for (auto st : aut_->all_states())
          for (auto l : letters)
            {
              state_set& ss = successors[st][l];
              ss.resize(state_size_);
              for (auto tr : aut_->out(st, l))
                ss.set(aut_->dst_of(tr));
            }


        // Create a new output state from SS. Insert in the output
        // automaton, in the map, and push in the stack.
        auto push_new_state =
          [this] (const state_set& ss) -> state_t
          {
            state_t r = res_->new_state();
            map_[ss] = r;

            if (ss.intersects(finals_))
              res_->set_final(r);

            todo_.push(ss);
            return r;
          };

        // The input initial states.
        state_set next;
        next.resize(state_size_);
        for (auto t : aut_->initial_transitions())
          next.set(aut_->dst_of(t));
        res_->set_initial(push_new_state(next));

        while (!todo_.empty())
          {
            auto ss = std::move(todo_.top());
            todo_.pop();
            for (auto l: letters)
              {
                next.reset();
                for (auto s = ss.find_first(); s != ss.npos;
                     s = ss.find_next(s))
                  next |= successors[s][l];
                // Don't generate the sink.
                if (complete_ || next.any())
                  {
                    auto i = map_.find(next);
                    state_t n = ((i == map_.end())
                                 ? push_new_state(next)
                                 : i->second);
                    res_->new_transition(map_[ss], n, l);
                  }
              }
          }
        return res_;
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
        o << "/* Origins.\n"
             "    node [shape = box, style = rounded]\n";
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
            o << "\"]\n";
          }
        o << "*/\n";
        return o;
      }
    };
  }

  template <typename AutPtr>
  inline
  auto
  determinize(const AutPtr& a, bool complete = false)
    -> decltype(detail::determinizer<AutPtr>(a)())
  {
    detail::determinizer<AutPtr> determinize(a, complete);
    auto res = determinize();
    if (getenv("VCSN_ORIGINS"))
      determinize.print(std::cout, determinize.origins());
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Bool>
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
