#ifndef VCSN_ALGOS_MINIMIZE_HH
# define VCSN_ALGOS_MINIMIZE_HH

# include <unordered_map>
# include <vector>

# include <vcsn/algos/accessible.hh>
# include <vcsn/algos/is-deterministic.hh>
# include <vcsn/algos/minimize-signature.hh>
# include <vcsn/dyn/automaton.hh>

namespace vcsn
{

  /*--------------------------------------.
  | minimization with Moore's algorithm.  |
  `--------------------------------------*/
  namespace detail
  {
    template <typename Aut>
    class minimizer
    {
      static_assert(Aut::context_t::is_lal,
                    "requires labels_are_letters");
      static_assert(std::is_same<typename Aut::weight_t, bool>::value,
                    "requires Boolean weights");

      using automaton_t = Aut;

      /// Input automaton, supplied at construction time.
      const automaton_t &a_;

      /// Non-special letters.
      const typename Aut::context_t::labelset_t& letters_;

      using label_t = typename automaton_t::label_t;
      using state_t = typename automaton_t::state_t;
      using class_t = unsigned;
      using classes_t = std::vector<class_t>;
      using set_t = std::vector<state_t>;
      using state_to_class_t = std::map<state_t, class_t>;
      using target_class_to_states_t = std::unordered_map<class_t, set_t>;
      using class_to_set_t = std::vector<set_t>;
      using class_to_state_t = std::unordered_map<class_t, state_t>;
      using state_to_state_t = std::unordered_map<state_t, state_t>;

      // These are to be used as class_t values.
      constexpr static class_t class_invalid = -1;
      class_t num_classes_ = 0;

      // First two classes are reserved, and are empty.
      class_to_set_t class_to_set_;
      state_to_class_t state_to_class_;
      class_to_state_t class_to_res_state_;
      state_to_state_t state_to_res_state_;

      /// An auxiliary data structure enabling fast access to
      /// transitions from a given state and label, in random order.
      /// This is a clear win for automata with many transitions
      /// between a couple of states.
      /// FIXME: this uglyish hash-of-hashes is slightly faster than
      /// an unordered_map of pairs, as of GCC 4.8.2 on Luca's amd64
      /// laptop, compiled with -O3.
      std::unordered_map<state_t, std::unordered_map<label_t, state_t>>
        out_;

      void clear()
      {
        class_to_set_.clear();
        state_to_class_.clear();
        num_classes_ = 0;
        class_to_res_state_.clear();
        state_to_res_state_.clear();
        out_.clear();
      }

      /// Make a new class with the given set of states.
      class_t make_class(set_t&& set, class_t number = class_invalid)
      {
        if (number == class_invalid)
          number = num_classes_++;

        for (auto s : set)
          state_to_class_[s] = number;

        if (number < class_to_set_.size())
          class_to_set_[number] = std::move(set);
        else
          {
            assert(number == class_to_set_.size());
            class_to_set_.emplace_back(std::move(set));
          }

        return number;
      }

      /// The destination class of \a s with \a l in \a a.
      /// Return \a class_invalid if \a s has no successor with \a l.
      class_t out_class(state_t s, label_t l)
      {
        auto i = out_.find(s);
        if (i == out_.end())
          return class_invalid;
        auto j = i->second.find(l);
        if (j == i->second.end())
          return class_invalid;
        return state_to_class_[j->second];
      }

    public:
      minimizer(const Aut& a)
        : a_(a)
        , letters_(*a_.labelset())
      {
        // We _really_ need determinism here.  See for instance
        // minimization of standard(aa+a) (not a+aa).
        if (!is_deterministic(a_))
          abort();
        for (auto t : a_.all_transitions())
          out_[a_.src_of(t)][a_.label_of(t)] = a_.dst_of(t);
      }

      /// The minimized automaton.
      void build_classes_()
      {
        // Initialization: two classes, partitioning final and non-final states.
        {
          set_t nonfinals, finals;
          for (auto s : a_.states())
            if (a_.is_final(s))
              finals.emplace_back(s);
            else
              nonfinals.emplace_back(s);
          make_class(std::move(nonfinals));
          make_class(std::move(finals));
        }

        bool go_on;
        do
          {
            go_on = false;
            for (class_t c = 0; c < num_classes_; ++c)
              {
                const set_t& c_states = class_to_set_[c];
                for (auto l : letters_)
                  {
                    target_class_to_states_t target_class_to_c_states;
                    bool with_class_invalid = false;
                    for (auto s : c_states)
                      {
                        auto c2 = out_class(s, l);
                        if (c2 == class_invalid)
                          with_class_invalid = true;
                        else
                          target_class_to_c_states[c2].emplace_back(s);
                      }

                    // Are there more than two keys?
                    //
                    // std::unordered_map::size is said to be O(1).
                    if (2 <= (target_class_to_c_states.size()
                              + with_class_invalid))
                      {
                        go_on = true;
                        class_t num = c;
                        for (auto& p : target_class_to_c_states)
                          {
                            make_class(std::move(p.second), num);
                            num = class_invalid;
                          }
                        // Ignore other labels for this partition
                        break;
                      }
                  } // for on labels
              } // for on classes
          }
        while (go_on);
      }

      automaton_t build_result_()
      {
        automaton_t res{a_.context()};
        /* For each input state compute the corresponding class and
           its corresponding output state.  Starting by making result
           states in a separate loop on c_s would be slightly simpler,
           but would yield an unspecified state numbering. */
        state_to_res_state_[a_.pre()] = res.pre();
        state_to_res_state_[a_.post()] = res.post();
        for (auto s : a_.states())
          {
            class_t s_class = state_to_class_[s];
            auto iterator = class_to_res_state_.find(s_class);
            state_t res_state;
            if (iterator == class_to_res_state_.end())
              class_to_res_state_[s_class] = res_state = res.new_state();
            else
              res_state = iterator->second;

            state_to_res_state_[s] = res_state;
          }

        /* Add input transitions to the result automaton, including
           the special ones defining which states are initial or
           final.  Here we rely on weights being Boolean.  */
        for (auto t : a_.all_transitions())
          res.add_transition(state_to_res_state_[a_.src_of(t)],
                             state_to_res_state_[a_.dst_of(t)],
                             a_.label_of(t));

        /* Moore's construction maps each set of indistinguishable
           states into a class; however the fact of being
           distinguishable from one another doesn't make all classes
           useful.  */
        return trim(res);
      }

      /// Return the quotient.
      automaton_t operator()()
      {
        build_classes_();
        return build_result_();
      }

      /// A map from minimized states to sets of original states.
      using origins_t = std::map<state_t, std::set<state_t>>;
      origins_t
      origins()
      {
        origins_t res;

        for (auto s : a_.states())
          res[state_to_res_state_[s]].emplace(s);

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
  minimize(const Aut& a)
  {
    detail::minimizer<Aut> minimize(a);
    auto res = minimize();
    // FIXME: Not absolutely elegant.  But currently no means to
    // associate meta-data to states.
    if (getenv("VCSN_ORIGINS"))
      minimize.print(std::cout, minimize.origins());
    return res;
  }

  /*----------------.
  | dyn::minimize.  |
  `----------------*/

  namespace dyn
  {
    namespace detail
    {

      template <typename Aut, typename String>
      automaton
      minimize(const automaton& aut, const std::string& algo = "signature")
      {
        const auto& a = aut->as<Aut>();
        if (algo == "moore")
          return make_automaton(minimize(a));
        else if(algo == "signature")
          return make_automaton(signature::minimize(a));
        else
          throw std::runtime_error("minimize: invalid algorithm: "
                                   + str_escape(algo));
      }

      REGISTER_DECLARE
      (minimize,
       (const automaton& aut, const std::string& algo) -> automaton);
    }
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_MINIMIZE_HH
