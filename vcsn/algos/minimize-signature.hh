#ifndef VCSN_ALGOS_MINIMIZE_SIGNATURE_HH
# define VCSN_ALGOS_MINIMIZE_SIGNATURE_HH

# include <algorithm> // min_element.
# include <unordered_map>
# include <unordered_set> // FIXME: remove unless neeeded

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
  namespace signature
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

      std::ostream& print_(std::ostream& o, const set_t& ss)
      {
        const char* sep = "{";
        for (auto s : ss)
          {
            o << sep << s;
            sep = ", ";
          }
        return o << "}";
      }
      std::ostream& print_(std::ostream& o, const class_to_set_t& c2ss)
      {
        const char* sep = "";
        for (unsigned i = 0; i < c2ss.size(); ++i)
          {
            o << sep << '[' << i << "] = ";
            print_(o, c2ss[i]);
            sep = "\n";
          }
        return o;
      }

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
        unsigned num_classes_;
      public:
        signature_hasher(minimizer& the_minimizer,
                         size_t num_classes)
          : state_to_class_(the_minimizer.state_to_class_)
          , num_classes_(num_classes)
        {}

        size_t operator()(const state_output_t* state_output_) const noexcept
        {
          const state_output_t& state_output = *state_output_;
          size_t res = 0;
          dynamic_bitset bits(num_classes_);
          for (auto& t : state_output)
            {
              const label_t& label = t.label;
              std::hash_combine(res, label);
              // Hash the set of classes reached with label.  Of
              // course the hash must not depend on class ordering.
              bits.reset();
              for (auto s : t.to_states)
                bits.set(state_to_class_.at(s));
              std::hash_combine(res, bits);
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

        bool operator()(const state_output_t *as_,
                        const state_output_t *bs_) const noexcept
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
                       signature_hasher(the_minimizer, class_bound),
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
      }

      /// Make a new class with the given set of states.
      class_t make_class(set_t&& set, class_t number = -1)
      {
        if (number == class_t(-1))
          number = num_classes_ ++;

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

    public:
      minimizer(const Aut& a)
        : a_(a)
        , is_deterministic_(is_deterministic(a_))
        , ls_(*a_.labelset())
        , ws_(*a.weightset())
      {
        if (!(is_trim(a_) || is_complete(a_)))
          abort();

        // Fill state_to_state_output.
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
      }

      /// Build the initial classes, and split until fix point.
      void build_classes_()
      {
        // Don't even bother to split between final and non-final
        // states, this initialization is useless.
        std::unordered_set<class_t> classes;
        {
          const auto& all = a_.all_states();
          classes.insert(make_class(set_t{std::begin(all), std::end(all)}));
        }

        bool go_on;
        do
          {
            go_on = false;
            for (auto i = std::begin(classes), end = std::end(classes);
                 i != end;
                 /* nothing. */)
              {
                auto c = *i;
                const set_t& c_states = class_to_set_.at(c);

                if (c_states.size() < 2)
                  {
                    i = classes.erase(i);
                    continue;
                  }

                // Try to find distinguishable states in c_states:
                signature_multimap
                  signature_to_state(*this,
                                     ls_, ws_, state_to_class_,
                                     num_classes_);
                for (auto s : c_states)
                  signature_to_state[& state_to_state_output_[s]].emplace_back(s);
                if (2 <= signature_to_state.size())
                  {
                    go_on = true;
                    i = classes.erase(i);
                    for (auto p: signature_to_state)
                      {
                        class_t c2 = make_class(std::move(p.second), c);
                        classes.insert(c2);
                        c = -1;
                      }
                  }
                else
                  ++i;
              } // for on classes
          }
        while (go_on);
      }

      /// Sort the classes.
      ///
      /// This step, which is "useless" in that the result would be
      /// correct anyway, just ensures that the classes are numbered
      /// after their states: classes are sorted by the smallest of
      /// their state ids.
      void sort_classes_()
      {
        /* For each class, put its smallest numbered state first.  We
           don't need to fully sort.  */
        for (unsigned c = 0; c < num_classes_; ++c)
            std::swap(class_to_set_[c][0],
                      *std::min_element(begin(class_to_set_[c]),
                                        end(class_to_set_[c])));

        /* Sort class numbers by smallest state number.  */
        std::sort(begin(class_to_set_), end(class_to_set_),
                  [](const set_t& lhs, const set_t& rhs) -> bool
                  {
                    return lhs[0] < rhs[0];
                  });

        /* Update state_to_class_.  */
        for (unsigned c = 0; c < num_classes_; ++c)
          for (auto s: class_to_set_[c])
            state_to_class_[s] = c;
      }

      /// Build the resulting automaton.
      automaton_t build_result_()
      {
        automaton_t res{a_.context()};
        class_to_res_state_.resize(num_classes_);
        for (unsigned c = 0; c < num_classes_; ++c)
          {
            state_t s = class_to_set_[c][0];
            class_to_res_state_[c]
              = s == a_.pre()  ? res.pre()
              : s == a_.post() ? res.post()
              : res.new_state();
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
                res.add_transition(src, dst, a_.label_of(t));
              }
          }
        return std::move(res);
      }

      /// The minimized automaton.
      automaton_t operator()()
      {
        build_classes_();
        sort_classes_();
        return build_result_();
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

    template <typename Aut>
    inline
    Aut
    minimize(const Aut& a)
    {
      minimizer<Aut> minimize(a);
      auto res = minimize();
      // FIXME: Not absolutely elegant.  But currently no means to
      // associate meta-data to states.
      if (getenv("VCSN_ORIGINS"))
        minimize.print(std::cout, minimize.origins());
      return res;
    }

  } // signature::
} // namespace vcsn

#endif // !VCSN_ALGOS_MINIMIZE_SIGNATURE_HH
