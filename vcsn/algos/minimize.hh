#ifndef VCSN_ALGOS_MINIMIZE_HH
# define VCSN_ALGOS_MINIMIZE_HH

# include <unordered_map>
# include <unordered_set> // FIXME: remove unless neeeded

# include <vcsn/dyn/automaton.hh>
# include <vcsn/algos/accessible.hh>
# include <vcsn/misc/pair.hh> // FIXME: remove unless neeeded
# include <vcsn/misc/hash.hh> // FIXME: remove unless neeeded

# ifndef NDEBUG
#  include <vcsn/algos/is-deterministic.hh>
# endif

namespace vcsn
{

  /*--------------------------------------.
  | minimization with Moore's algorithm.  |
  `--------------------------------------*/
  namespace detail
  {
    // std::ostream& operator<<(std::ostream& o, const signature_multimap &q){
    //   return o;
    // }

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

      /// The computed result.
      automaton_t res_;

      using labelset_t = typename automaton_t::labelset_t;
      using weightset_t = typename automaton_t::weightset_t;

      const labelset_t& ls_;
      const weightset_t& ws_;

      using label_t = typename automaton_t::label_t;
      using state_t = typename automaton_t::state_t;
      using class_t = unsigned;
      using classes_t = std::vector<class_t>;
      using set_t = std::vector<state_t>;
      using sets_t = std::vector<set_t>;
      using state_to_class_t = std::map<state_t, class_t>;
      using target_class_to_states_t = std::unordered_map<class_t, set_t>;
      using class_to_set_t = std::unordered_map<class_t, set_t>;
      using class_to_state_t = std::unordered_map<class_t, state_t>;
      using state_to_state_t = std::unordered_map<state_t, state_t>;

      // These are to be used as class_t values.
      enum : class_t {
        invalid_class =         0, // A non-existing class.
        empty_class =           1, // A class containing no states.
        // final_class =           3, // FIXME: this is tentative
        // nonfinals_class =       4, // FIXME: this is tentative 
        first_available_index = 2  //5  // For ordinary classes.
      };
      class_t next_class_index_ = first_available_index;

      class_to_set_t class_to_set_;
      /* mutable */ state_to_class_t state_to_class_;
      class_to_state_t class_to_res_state_;
      state_to_state_t state_to_res_state_;

      using weight_t = typename Aut::weight_t; // FIXME: for the future
      using transition_output_t = std::pair<label_t, std::pair<weight_t, state_t>>; // FIXME: make this not suck.
      //struct transition_output_t{ label_t label; weight_t weight; state_t state; };
      using state_output_t = std::set<transition_output_t>;

      // // We commit a slight abuse of language and call "signature" the
      // // state output, without explicitly referring the
      // // state_to_class_t map needed to map the output states into
      // // classes.

      std::unordered_map<state_t, state_output_t> state_to_state_output_;

      //friend class signature_hasher; // FIXME: decrappify
      class signature_hasher : public std::hash<state_output_t>
      {
        const state_to_class_t& state_to_class_;
      public:
        signature_hasher(const state_to_class_t& state_to_class)
          : state_to_class_(state_to_class)
        {}

        size_t operator()(const state_output_t& state_output) const noexcept
        {
          size_t res = 0;
          for (auto t : state_output)
            {
              label_t label = t.first;//item.label;
              weight_t weight = t.second.first;//item.weight;
              state_t state = t.second.second;//item.state;
              std::hash_combine(res, label);
              std::hash_combine(res, weight);
              std::hash_combine(res, /*NEW_*/state_to_class_.at(state));
            }
          return res;
        }
      }; // class signature_hasher

      class signature_equal_to : public std::equal_to<state_output_t>
      {
        const labelset_t& ls_;
        const weightset_t& ws_;
        const state_to_class_t& state_to_class_;
      public:
        signature_equal_to(const labelset_t& ls,
                           const weightset_t& ws,
                           const state_to_class_t& state_to_class)
          : ls_(ls)
          , ws_(ws)
          , state_to_class_(state_to_class)
        {}

        bool operator()(const state_output_t& as, const state_output_t& bs) const noexcept
        {
          if (as.size() != bs.size())
            return false;

          auto b_i = bs.cbegin();
          for (const auto& a : as)
            {
              label_t a_label = a.first;
              weight_t a_weight = a.second.first;
              state_t a_state = a.second.second;

              label_t b_label = b_i->first;
              weight_t b_weight = b_i->second.first;
              state_t b_state = b_i->second.second;

              if ((  ! ls_.equals(a_label, b_label))
                  || ! ws_.equals(a_weight, b_weight)
                  || state_to_class_.at(a_state) != state_to_class_.at(b_state))
                return false;

              ++ b_i;
            }

          return true;
        }
      }; // class signature_equal_to

      class signature_multimap
        : public std::unordered_multimap<state_output_t,
                                         state_t,
                                         signature_hasher,
                                         signature_equal_to>
      {
        const state_to_class_t& state_to_class_;
      public:
        signature_multimap(const labelset_t& ls,
                           const weightset_t& ws,
                           state_to_class_t& state_to_class)
          : std::unordered_multimap<state_output_t,
                                    state_t,
                                    signature_hasher,
                                    signature_equal_to>
            (1,
             signature_hasher(state_to_class),
             signature_equal_to(ls, ws, state_to_class))
          , state_to_class_(state_to_class)
        {}

        friend std::ostream& operator<<(std::ostream& o,
                                        const state_output_t& so)
        {
          o << "[";
          // for (auto to : so)
          //   {
          //     label_t l = to.first;
          //     state_t s = to.second.second;
          //     o << "<" << l << "," << s << ">";
          //   }
          // o << "]";
          return o;
        }
        friend std::ostream& operator<<(std::ostream& o,
                                        const signature_multimap& mm)
        {
          for (auto o_s : mm)
            {
              o << "[";
              for (auto to : o_s.first)
                {
                  label_t l = to.first;
                  state_t s = to.second.second;
                  o << "<" << l << " s" << s << "(c" << mm.state_to_class_.at(s) << ")>";
                }

              o << "]: s" << o_s.second << "  ";
            }
          //for (auto o_s : mm)
          //  o << (o_s.first) << ": " << o_s.second << ",  ";
          o << "\n";
          return o;
        }
      }; // class signature_multimap

      // inline bool are_state_outputs_distinguishable(const state_output_t& state_output1, size_t hash1,
      //                                               const state_output_t& state_output2, size_t hash2)
      // {
      //   return hash1 != hash2 || state_output1 != state_output2;
      // }

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
        next_class_index_ = 2;
        class_to_set_[empty_class].clear();
        class_to_res_state_.clear();
        state_to_res_state_.clear();
        for (auto s : res_.states())
          res_.del_state(s);
        out_.clear();
      }

      void make_class_named(const set_t& set, class_t class_identifier)
      {

        class_to_set_[class_identifier] = set;
        for (auto s : set)
          state_to_class_[s] = class_identifier;
      }

      /// Make a new class with the given set of states.
      class_t make_class(const set_t& set)
      {
        class_t res = next_class_index_ ++;
        //std::cerr << "Making class " << res << "\n";
        make_class_named(set, res);
        return res;
      }

      /// The destination class of \a s with \a l in \a a.
      /// Return \a empty_class if \a s has no successor with \a l.
      class_t out_class(state_t s, label_t l)
      {
        auto i = out_.find(s);
        if (i == out_.end())
          return empty_class;
        auto j = i->second.find(l);
        if (j == i->second.end())
          return empty_class;
        return state_to_class_[j->second];
      }

      /// Whether there are at least two classes.
      bool more_than_one_class(const target_class_to_states_t& c) const
      {
        auto i = std::begin(c);
        auto end = std::end(c);
        return i != end && ++i != end;
      }

    public:
      minimizer(const Aut& a)
        : a_(a)
        , letters_(*a_.labelset())
        , res_{a.context()}
        , ls_(letters_) // FIXME: redundant
        , ws_(*a.weightset())
      {}

      /// The minimized automaton.
      automaton_t operator()()
      {
        assert(is_deterministic(a_));
        clear();
        // FIXME: is this still needed?
        for (auto t : a_.all_transitions())
          out_[a_.src_of(t)][a_.label_of(t)] = a_.dst_of(t);

        // FIll state_to_state_output:
        for (auto t : a_.all_transitions())
          state_to_state_output_[a_.src_of(t)].
             emplace(transition_output_t{a_.label_of(t),
                                         {a_.weight_of(t), a_.dst_of(t)}});

        // // Initialization: two classes, partitioning final and non-final states.
        // std::unordered_set<class_t> classes;
        // set_t nonfinals, finals;
        // for (auto s : a_.states())
        //   nonfinals.emplace_back(s);
        // nonfinals.emplace_back(a_.pre());
        // finals.emplace_back(a_.post());
        // classes.insert({make_class(nonfinals),
        //                 make_class(finals)});

        // Alexandre-style initialization: one class only:
        std::unordered_set<class_t> classes;
        set_t all_states;
        for (auto s : a_.all_states())
          all_states.emplace_back(s);
        classes.insert(make_class(all_states));

        classes_t classes_to_remove;
        sets_t sets_to_add;
        int iteration_no = 0;
        do
          {
            sets_to_add.clear();
            classes_to_remove.clear();

            //std::cerr << "Iteration "<< ++iteration_no<<": there are " << classes.size() << " classes.\n";
            for (auto c : classes)
              {
                //std::cerr << "splitting " << c << "\n";
                const set_t c_states = class_to_set_.at(c);

                if (c_states.size() < 2)
                  {
                    //std::cerr << "Skipping trivial class " << c << "\n";
                    continue;
                  }

                // Try to find distinguishable states in c_states:
                signature_multimap signature_to_state(ls_, ws_, state_to_class_);
                for (auto s : c_states)
                  signature_to_state.emplace(state_to_state_output_[s], s);

                //signature_to_state.print(); // ???
                //std::cerr << signature_to_state << "\n"; // ???
                //std::cerr << "splitting this class: ";
                //for (auto s : c_states) std::cerr << s << " "; std::cerr << "\n";
                std::vector<set_t> new_sets;
                while (! signature_to_state.empty())
                  {
                    auto first_signature = signature_to_state.cbegin()->first;
                    auto range = signature_to_state.equal_range(first_signature);
                    set_t new_set;
                    for (auto i = range.first; i != range.second; ++ i)
                      new_set.emplace_back(i->second);
                    new_sets.emplace_back(new_set);

                    //std::cerr << "* new_set: "; for (auto s : new_set) std::cerr << s << " "; std::cerr << "\n";

                    signature_to_state.erase(first_signature);
                  } // while
                //std::cerr << "\n";
                if (new_sets.size() > 1)
                  {
                    classes_to_remove.emplace_back(c);
                    for (auto s : new_sets)
                      sets_to_add.emplace_back(s);

                  }
              } // for on classes

            // Destroy the classes we've split and make the new ones.
            for (auto c : classes_to_remove)
              {
                class_to_set_.erase(c);
                classes.erase(c);
                //std::cerr << "Removed class " << c << "\n";
              }
            for (auto set : sets_to_add)
              classes.insert(make_class(set));
            //std::cerr << "Continuing? " << (! sets_to_add.empty()) << "\n";
          }
        while (! sets_to_add.empty());

        // //// BEGIN
        // signature_t t1 = {{'a', {2, 3}}};
        // signature_t t2 = {{'a', {2, 3}}};
        // signature_t t3 = {{'b', {2, 4}}};
        // size_t h1 = hash_signature(t1);
        // size_t h2 = hash_signature(t2);
        // size_t h3 = hash_signature(t3);

        // std::cerr << "Are t1 and t2 distinguishable? " << are_signatures_distinguishable(t1, h1, t2, h2) << "\n";
        // std::cerr << "Are t1 and t3 distinguishable? " << are_signatures_distinguishable(t1, h1, t3, h3) << "\n";
        // //// END

        // classes_t classes_to_remove;
        // sets_t sets_to_add;
        // do
        //   {
        //     sets_to_add.clear();
        //     // We empty classes_to_remove at the end of each iteration.

        //     for (auto c_s : class_to_set_)
        //       {
        //         class_t c = c_s.first;
        //         const set_t c_states = c_s.second;

        //         for (auto l : letters_)
        //           {
        //             target_class_to_states_t target_class_to_c_states;
        //             for (auto s : c_states)
        //               target_class_to_c_states[out_class(s, l)].emplace_back(s);

        //             // Are there more than two keys?
        //             if (more_than_one_class(target_class_to_c_states))
        //               {
        //                 classes_to_remove.emplace_back(c);
        //                 for (const auto& p : target_class_to_c_states)
        //                   sets_to_add.emplace_back(std::move(p.second));
        //                 // Ignore other labels for this partition.
        //                 break;
        //               }
        //           } // for on labels
        //       } // for on classes

        //     for (auto c : classes_to_remove)
        //       class_to_set_.erase(c);
        //     classes_to_remove.clear();
        //     for (auto set : sets_to_add)
        //       make_class(set);
        //   }
        // while (! sets_to_add.empty());

        /* For each input state compute the corresponding class and
           its corresponding output state.  Starting by making result
           states in a separate loop on c_s would be slightly simpler,
           but would yield an unspecified state numbering. */
        state_to_res_state_[a_.pre()] = res_.pre();
        state_to_res_state_[a_.post()] = res_.post();
        for (auto s : a_.states())
          {
            class_t s_class = state_to_class_[s];
            auto iterator = class_to_res_state_.find(s_class);
            state_t res_state;
            if (iterator == class_to_res_state_.end())
              class_to_res_state_[s_class] = res_state = res_.new_state();
            else
              res_state = iterator->second;

            state_to_res_state_[s] = res_state;
          }

        /* Add input transitions to the result automaton, including
           the special ones defining which states are initial or
           final.  Here we rely on weights being Boolean. */
        for (auto t : a_.all_transitions())
          res_.add_transition(state_to_res_state_[a_.src_of(t)],
                              state_to_res_state_[a_.dst_of(t)],
                              a_.label_of(t));

        /* Moore's construction maps each set of indistinguishable
           states into a classe; however the fact of being
           distinguishable from one another doesn't make all classes
           useful. */
        return trim(res_);
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

      template <typename Aut>
      automaton
      minimize(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(minimize(a));
      }

      REGISTER_DECLARE(minimize,
                       (const automaton& aut) -> automaton);
    }
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_MINIMIZE_HH
