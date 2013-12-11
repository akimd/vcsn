#ifndef VCSN_ALGOS_MINIMIZE_HH
# define VCSN_ALGOS_MINIMIZE_HH

# include <unordered_map>
# include <unordered_set> // FIXME: remove unless neeeded
# include <algorithm> // FIXME: remove unless neeeded

# include <vcsn/dyn/automaton.hh>
# include <vcsn/algos/accessible.hh>
# include <vcsn/misc/pair.hh> // FIXME: remove unless neeeded
# include <vcsn/misc/hash.hh> // FIXME: remove unless neeeded
# include <vcsn/misc/dynamic_bitset.hh>

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
    template <typename Aut>
    class minimizer
    {
      static_assert(Aut::context_t::is_lal,
                    "requires labels_are_letters");
      static_assert(std::is_same<typename Aut::weight_t, bool>::value,
                    "requires Boolean weights"); // FIXME: relax this

      using automaton_t = Aut;

      /// Input automaton, supplied at construction time.
      const automaton_t &a_;
      const bool is_deterministic_;

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
      using state_to_class_t = std::unordered_map<state_t, class_t>;
      using class_to_set_t = std::vector<set_t>;
      using class_to_state_t = std::vector<state_t>;
      using state_to_state_t = std::unordered_map<state_t, state_t>;

      unsigned num_classes_ = 0;

      class_to_set_t class_to_set_;
      state_to_class_t state_to_class_;
      class_to_state_t class_to_res_state_;

      using weight_t = typename Aut::weight_t; // FIXME: for the future
      struct state_output_for_label_t
      {
        // For some unstored state.
        label_t label;
        std::vector<state_t> to_states; // Ordered.
      };

      // This is sorted by label.
      using state_output_t = std::vector<state_output_for_label_t>;

      friend class label_less;
      class label_less
      {
        minimizer& minimizer_;
        const labelset_t& ls_;
      public:
        label_less(minimizer& the_minimizer)
          : minimizer_(the_minimizer)
          , ls_(the_minimizer.ls_)
        {}
        bool operator()(const label_t& a,
                        const label_t& b) const noexcept
        {
          return ls_.less_than(a, b);
        }
      }; // class label_less

      // This structure is only useful at initialization time, when
      // sorting transitions from a given state in a canonical order.
      class label_to_states_t : public std::map<label_t,
                                                std::vector<state_t>,
                                                label_less>
      {
      public:
        label_to_states_t(minimizer& the_minimizer)
          : std::map<label_t, std::vector<state_t>, label_less>(the_minimizer)
        {}
      }; // class label_to_states_t


      std::unordered_map<state_t, state_output_t> state_to_state_output_;

      friend class signature_hasher;
      class signature_hasher : public std::hash<state_output_t*>
      {
        const state_to_class_t& state_to_class_;
      public:
        signature_hasher(minimizer& the_minimizer)
          : state_to_class_(the_minimizer.state_to_class_)
        {}

        size_t operator()(const state_output_t* state_output_) const noexcept
        {
          const state_output_t& state_output = *state_output_;
          size_t res = 0;
          for (auto& t : state_output)
            {
              const label_t& label = t.label;
              std::hash_combine(res, label);

              const std::vector<state_t>& to_states = t.to_states;

              // Hash the set of classes reached with label.  Of
              // course the hash must not depend on class ordering.

              // FIXME: what follows is overkill in the deterministic
              // case.  Since we made sure at initialization time that
              // the states in to_states are in a fixed order, by
              // hashing the class associated to each state in order
              // yields a canonical order over classes as well, so
              // that we don't need to sort here.

              // Here I use a set to fix a canonical order among
              // classes (FIXME: use a bitset)
              std::set<class_t> classes;
              for (auto s : to_states)
                classes.emplace(state_to_class_.at(s));

              for (const auto c : classes)
                std::hash_combine(res, c);
            }
          return res;
        }
      }; // class signature_hasher

      friend class signature_equal_to;
      class signature_equal_to : public std::equal_to<state_output_t*>
      {
        minimizer& minimizer_;
        const labelset_t& ls_;
        const weightset_t& ws_;
        const state_to_class_t& state_to_class_;
        const size_t class_bound_;
      public:
        signature_equal_to(minimizer& the_minimizer,
                           // FIXME: remove these unless really needed
                           const labelset_t& ls,
                           const weightset_t& ws,
                           const state_to_class_t& state_to_class,
                           size_t class_bound)
          : minimizer_(the_minimizer)
          , ls_(ls)
          , ws_(ws)
          , state_to_class_(state_to_class)
          , class_bound_(class_bound)
        {}

        bool operator()(const state_output_t *as_, const state_output_t *bs_) const noexcept
        {
          const state_output_t& as = *as_;
          const state_output_t& bs = *bs_;

          // In the deterministic case the number of *labels* leading to
          // output states must be the same, for two signatures to match.
          if (minimizer_.is_deterministic_
              && as.size() != bs.size())
            return false;

          auto b_i = bs.cbegin();
          dynamic_bitset a_bits(class_bound_), b_bits(class_bound_);
          for (const auto& a : as)
            {
              const label_t& a_label = a.label;
              const label_t& b_label = b_i->label;
              const std::vector<state_t>& a_states = a.to_states;
              const std::vector<state_t>& b_states = b_i->to_states;

              if (! ls_.equals(a_label, b_label))
                return false;

              // a_states and b_states may have different sizes, but
              // still be considered "equal", up to the state-to-class
              // assignment.

              // FIXME: this can and should be optimized a lot.
              //dynamic_bitset a_bits(class_bound_), b_bits(class_bound_);
              a_bits.reset(); b_bits.reset();
              for (auto s : a_states)
                a_bits.set(state_to_class_.at(s));
              for (auto s : b_states)
                b_bits.set(state_to_class_.at(s));
              if (a_bits != b_bits)
                return false;

              ++ b_i;
            }

          return true;
        }
      }; // class signature_equal_to

      friend class signature_multimap;
      class signature_multimap
        : public std::unordered_map<state_output_t*, set_t,
                                    signature_hasher, signature_equal_to>
      {
        minimizer& minimizer_;
        const state_to_class_t& state_to_class_;
        using super_type
          = std::unordered_map<state_output_t*, set_t,
                               signature_hasher, signature_equal_to>;
      public:
        signature_multimap(minimizer& the_minimizer,
                           // FIXME: remove these unless really needed.
                           const labelset_t& ls,
                           const weightset_t& ws,
                           state_to_class_t& state_to_class,
                           const size_t class_bound)
          : super_type(1,
                       signature_hasher(the_minimizer),
                       signature_equal_to(the_minimizer,
                                          ls, ws, state_to_class, class_bound))
          , minimizer_(the_minimizer)
          , state_to_class_(state_to_class)
        {}

        friend std::ostream& operator<<(std::ostream& o,
                                        const signature_multimap& mm)
        {
          for (auto o_s : mm)
            {
              const char* sep = "{";
              for (auto to : o_s.first)
                {
                  label_t l = to.first;
                  state_t s = to.second.second;
                  o << sep << s << '@' << mm.state_to_class_.at(s) << ' ' << l;
                  sep = ", ";
                }
              o << "} -> ";
              sep = "{";
              for (auto s: o_s.second)
                {
                  o << sep << s << '@' << mm.state_to_class_.at(s);
                  sep = ", ";
                }
              o << "} ";
            }
          o << "\n";
          return o;
        }
      }; // class signature_multimap

      void clear()
      {
        class_to_set_.clear();
        state_to_class_.clear();
        num_classes_ = 0;
        class_to_res_state_.clear();
        // FIXME: mutable_automaton.clear().
        for (auto s : res_.states())
          res_.del_state(s);
      }

      void make_class_named(const set_t& set, class_t class_identifier)
      {
        if (class_identifier < class_to_set_.size())
          class_to_set_[class_identifier] = set;
        else
          {
            assert(class_identifier == class_to_set_.size());
            class_to_set_.push_back(set);
          }
        for (auto s : set)
          state_to_class_[s] = class_identifier;
      }

      /// Make a new class with the given set of states.
      class_t make_class(const set_t& set, class_t number = -1)
      {
        if (number == class_t(-1))
          number = num_classes_ ++;
        //std::cerr << "Making class " << res << "\n";
        make_class_named(set, number);
        return number;
      }

    public:
      minimizer(const Aut& a)
        : a_(a)
        , is_deterministic_(is_deterministic(a_))
        , letters_(*a_.labelset())
        , res_{a.context()}
        , ls_(letters_) // FIXME: redundant
        , ws_(*a.weightset())
      {}

      /// The minimized automaton.
      automaton_t operator()()
      {
        std::cerr << "Starting...\n";
        if (!(is_trim(a_) || is_complete(a_)))
          abort();
        clear();

        std::cerr << "Filling state_to_state_output...\n";
        // FIll state_to_state_output.
        for (auto s : a_.all_states())
          {
            // Get the out-states from s, by label:
            label_to_states_t label_to_states(*this);
            for (auto t : a_.all_out(s))
              label_to_states[a_.label_of(t)].emplace_back(a_.dst_of(t));

            // Associate this information to s, as a vector sorted by label:
            state_output_t& state_output = state_to_state_output_[s];
            for (auto& l_ss : label_to_states)
              {
                std::sort(l_ss.second.begin(), l_ss.second.end());
                state_output.emplace_back(state_output_for_label_t{l_ss.first,
                                                                   std::move(l_ss.second)});
              }
          }
        std::cerr << "...Done\n";

        // Alexandre-style initialization: one class only.
        std::unordered_set<class_t> classes;
        {
          set_t all_states;
          for (auto s : a_.all_states())
            all_states.emplace_back(s);
          classes.insert(make_class(all_states));
        }

        std::cerr << "Entering the main loop...\n";
        int iteration_no = 0;
        bool go_on;
        do
          {
            go_on = false;

            std::cerr << "Iteration " << ++ iteration_no
                      << ": there are " << num_classes_ << " classes.\n";
            //std::cerr << "Classes are: "; for (auto c : classes) std::cerr << c << " "; std::cerr << "\n";

            for (auto i = std::begin(classes), end = std::end(classes);
                 i != end;
                 /* nothing. */)
              {
                auto c = *i;
                //std::cerr << "splitting " << c << "\n";
                //std::cerr << "ok 1a "<< c << "\n";
                const set_t& c_states = class_to_set_.at(c);
                //std::cerr << "ok 1b\n";

                if (c_states.size() < 2)
                  {
                    //std::cerr << "Ignoring trivial class " << c << " from now on\n";
                    ++i;
                    continue;
                  }

                // Try to find distinguishable states in c_states:
                signature_multimap signature_to_state(*this,
                                                      ls_, ws_, state_to_class_,
                                                      num_classes_ * a_.num_all_states()); // FIXME: make this not suck
                for (auto s : c_states)
                  signature_to_state[& state_to_state_output_[s]].emplace_back(s);
                //std::cerr << "The multimap has size " << signature_to_state.size() << "\n";

                //signature_to_state.print(); // ???
                //std::cerr << signature_to_state << "\n"; // ???
                //std::cerr << "splitting this class: ";
                //for (auto s : c_states) std::cerr << s << " "; std::cerr << "\n";
                //std::cerr << "\n";
                if (1 < signature_to_state.size())
                  {
                    go_on = true;

                    //std::cerr << "Breaking class " << c << "\n";
                    i = classes.erase(i);

                    for (auto p: signature_to_state)
                      {
                        class_t c2 = make_class(p.second, c);
                        //std::cerr << "* making class " << c << "\n";
                        classes.insert(c2);
                        c = -1;
                      }
                  }
                else
                  ++i;
              } // for on classes
          }
        while (go_on);

        /* For each input state compute the corresponding class and
           its corresponding output state.  Starting by making result
           states in a separate loop on c_s would be slightly simpler,
           but would yield an unspecified state numbering. */
        class_to_res_state_.resize(num_classes_);
        for (unsigned c = 0; c < num_classes_; ++c)
          {
            state_t s = class_to_set_[c][0];
            class_to_res_state_[c]
              = s == a_.pre()  ? res_.pre()
              : s == a_.post() ? res_.post()
              : res_.new_state();
#if DEBUG
            std::cerr << c << " -> " << class_to_res_state_[c] << " (";
            for (auto s: class_to_set_[c])
              std::cerr << ' ' << s;
            std::cerr << ")\n";
#endif
          }
        for (auto c = 0; c < num_classes_; ++c)
          {
            // Copy the transitions of the first state of the class in
            // the result.
            state_t s = class_to_set_[c][0];
            state_t src = class_to_res_state_[c];
            for (auto t : a_.all_out(s))
              {
                state_t d = a_.dst_of(t);
                state_t dst = class_to_res_state_[state_to_class_[d]];
#if DEBUG
                std::cerr
                  << s << ' ' << a_.label_of(t) << ' ' << d
                  << " => "
                  << src << ' ' << a_.label_of(t) << ' ' << dst<< '\n';
#endif
                res_.add_transition(src, dst, a_.label_of(t));
              }
          }

        return std::move(res_);
        // /* Moore's construction maps each set of indistinguishable
        //    states into a classe; however the fact of being
        //    distinguishable from one another doesn't make all classes
        //    useful. */
        // return trim(res_);
      }

      /// A map from minimized states to sets of original states.
      using origins_t = std::map<state_t, std::set<state_t>>;
      origins_t
      origins()
      {
        origins_t res;
        for (unsigned c = 0; c < num_classes_; ++c)
          res[class_to_res_state_[c]]
              .insert(begin(class_to_set_[c]), end(class_to_set_[c]));
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
          if (2 <= p.first)
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
