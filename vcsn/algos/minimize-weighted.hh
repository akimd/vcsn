#ifndef VCSN_ALGOS_MINIMIZE_WEIGHTED_HH
# define VCSN_ALGOS_MINIMIZE_WEIGHTED_HH

# include <unordered_map>
# include <unordered_set>

# include <vcsn/algos/accessible.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/misc/indent.hh>
# include <vcsn/misc/raise.hh>

namespace vcsn
{

  /*-------------------------------------------------------------------------.
  | minimization with signatures; general version working on any weightset.  |
  `-------------------------------------------------------------------------*/
  namespace detail_weighted
  {
    template <typename Aut>
    class minimizer
    {
      using automaton_t = Aut;

      /// Input automaton, supplied at construction time.
      const automaton_t& a_;

      using labelset_t = labelset_t_of<automaton_t>;
      const labelset_t& ls_;

      using weightset_t = weightset_t_of<automaton_t>;
      const weightset_t& ws_;

      using label_t = label_t_of<automaton_t>;
      using weight_t = weight_t_of<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using class_t = unsigned;
      using set_t = std::vector<state_t>;
      using state_to_class_t = std::unordered_map<state_t, class_t>;
      using class_to_set_t = std::vector<set_t>;

      constexpr static const char* me() { return "minimize-weighted"; }

      /// An invalid class.
      constexpr static class_t class_invalid = -1;
      unsigned num_classes_ = 0;

      class_to_set_t class_to_set_;
      state_to_class_t state_to_class_;

      using weight_and_state_t = std::pair<weight_t, state_t>;

      struct state_output_for_label_t
      {
        // For some unstored state.
        label_t label;
        std::vector<weight_and_state_t> weights_and_destinations;
      };

      // This is sorted by label.
      using state_output_t = std::vector<state_output_for_label_t>;

      friend class label_less;
      class label_less
      {
        minimizer& minimizer_;
      public:
        label_less(minimizer& the_minimizer)
          : minimizer_(the_minimizer)
        {}
        bool operator()(const label_t& a,
                        const label_t& b) const noexcept
        {
          return minimizer_.ls_.less_than(a, b);
        }
      }; // class label_less

      // This structure is only useful at initialization time, when
      // sorting transitions from a given state in a canonical order.
      class label_to_weights_and_states_t: public std::map<label_t,
                                                           std::vector<weight_and_state_t>,
                                                           label_less>
      {
      public:
        label_to_weights_and_states_t(minimizer& the_minimizer)
          : std::map<label_t,
                     std::vector<weight_and_state_t>,
                     label_less>(the_minimizer)
        {}
      }; // class label_to_weights_and_states_t

      std::unordered_map<state_t, state_output_t> state_to_state_output_;

      /// The output of a given letter from a given state, keeping
      /// into account classes and weights, in a format suitable to
      /// comparison or hashing.  If a class is reachable with weight
      /// zero, it's guaranteed to be omitted from the table.
      using state_label_output_map_t = std::map<class_t, weight_t>;

      const state_label_output_map_t
      state_label_output_map(const std::vector<weight_and_state_t>& wss) const
      {
        state_label_output_map_t res;

        for (const auto& wd : wss)
          {
            class_t c = state_to_class_.at(wd.second);
            const auto& i = res.find(c);
            if (i == res.end())
              res[c] = wd.first;
            else
              {
                i->second = ws_.add(i->second, wd.first);
                if (ws_.is_zero(i->second))
                  res.erase(c);
              }
          }
        return res;
      }

      friend class signature_hasher;
      class signature_hasher : public std::hash<state_output_t*>
      {
        const minimizer& minimizer_;
        unsigned num_classes_;
      public:
        signature_hasher(minimizer& the_minimizer,
                         size_t num_classes)
          : minimizer_(the_minimizer)
          , num_classes_(num_classes)
        {}

        size_t operator()(const state_output_t* state_output_) const noexcept
        {
          const state_output_t& state_output = *state_output_;
          size_t res = 0;
          for (auto& t : state_output)
            {
              state_label_output_map_t map =
                minimizer_.state_label_output_map(t.weights_and_destinations);
              // I've chosen *not* to hash the label when all
              // transitions with a given label cancel one another.
              if (! map.empty())
                std::hash_combine(res, labelset_t::hash(t.label));
              for (const auto& cw : map)
                {
                  std::hash_combine(res, cw.first);
                  std::hash_combine(res, weightset_t::hash(cw.second));
                }
            }
          return res;
        }
      }; // class signature_hasher

      friend class signature_equal_to;
      class signature_equal_to : public std::equal_to<state_output_t*>
      {
        minimizer& minimizer_;
        const size_t class_bound_;
      public:
        signature_equal_to(minimizer& the_minimizer,
                           size_t class_bound)
          : minimizer_(the_minimizer)
          , class_bound_(class_bound)
        {}

        bool match(const state_label_output_map_t& a,
                    const state_label_output_map_t& b) const noexcept
        {
          if (a.size() != b.size())
            return false;
          for (const auto& cw : a)
            {
              const auto& cwb = b.find(cw.first);
              if (cwb == b.end())
                return false;
              if (! weightset_t::equals(cw.second, cwb->second))
                return false;
            }
          return true;
        }

        /// Update the iterator to point to the first non-empty
        /// state_output_for_label_t or v.cend(), and update the map to be the
        /// one associated to *i, or empty.
        void first(const state_output_t& v,
                   typename state_output_t::const_iterator& i,
                   state_label_output_map_t& map) const
        {
          i = v.cbegin();
          map.clear();

          if (i != v.cend())
            map = std::move(minimizer_
                            .state_label_output_map(i->weights_and_destinations));
        }

        /// Update the iterator to point to the next non-empty
        /// state_output_for_label_t or v.cend(), and update the map to be the
        /// one associated to *i, or empty.
        void next(const state_output_t& v,
                  typename state_output_t::const_iterator& i,
                  state_label_output_map_t& map) const
        {
          while (true)
            {
              i ++;
              map.clear();
              if (i == v.cend())
                return;
              map = std::move(minimizer_
                  .state_label_output_map(i->weights_and_destinations));
              if (! map.empty())
                return;
            }
        }

        bool operator()(const state_output_t *as_,
                        const state_output_t *bs_) const noexcept
        {
          const state_output_t& as = *as_;
          const state_output_t& bs = *bs_;

          // Scan as and bs in lockstep, verifying that they match for
          // each letter.  Letters occur in the same order by
          // construction.
          typename state_output_t::const_iterator ia, ib;
          state_label_output_map_t mapa, mapb;
          first(as, ia, mapa);
          first(bs, ib, mapb);
          // if ((ia == as.cend()) != (ib == bs.cend()))
          //   return false;
          for (/* Nothing. */;
               ia != as.cend() && ib != bs.cend();
               next(as, ia, mapa), next(bs, ib, mapb))
            if (! minimizer_.ls_.equals(ia->label, ib->label)
                || ! match(mapa, mapb))
              return false;

          return (ia == as.cend()) == (ib == bs.cend());
        }
      }; // class signature_equal_to

      friend class signature_multimap;
      class signature_multimap
        : public std::unordered_map<state_output_t*, set_t,
                                    signature_hasher, signature_equal_to>
      {
        minimizer& minimizer_;
        using super_type
          = std::unordered_map<state_output_t*, set_t,
                               signature_hasher, signature_equal_to>;
      public:
        signature_multimap(minimizer& the_minimizer,
                           const size_t class_bound)
          : super_type(1,
                       signature_hasher(the_minimizer, class_bound),
                       signature_equal_to(the_minimizer, class_bound))
          , minimizer_(the_minimizer)
        {}
      }; // class signature_multimap

      void clear()
      {
        class_to_set_.clear();
        state_to_class_.clear();
        num_classes_ = 0;
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

    public:
      minimizer(const Aut& a)
        : a_(a)
        , ls_(*a_->labelset())
        , ws_(*a_->weightset())
      {
        require(is_trim(a_), std::string(me()) + ": input must be trim");

        // Fill state_to_state_output.
        for (auto s : a_->all_states())
          {
            // Get the out-states from s, by label:
            label_to_weights_and_states_t label_to_weights_and_states(*this);
            for (auto t : a_->all_out(s))
              label_to_weights_and_states[a_->label_of(t)]
                .emplace_back(std::pair<weight_t, state_t>{a_->weight_of(t),
                                                           a_->dst_of(t)});

            // Associate this information to s, as a vector sorted by label:
            state_output_t& state_output = state_to_state_output_[s];
            for (auto& l_wss : label_to_weights_and_states)
              {
                std::sort(l_wss.second.begin(),
                          l_wss.second.end(),
                          [](const weight_and_state_t& l,
                             const weight_and_state_t& r)
                          {
                            if (l.second < r.second)
                              return true;
                            else if (r.second < l.second)
                              return false;
                            else
                              return weightset_t::less_than(l.first, r.first);
                          });
                state_output.emplace_back(state_output_for_label_t{l_wss.first,
                      std::move(l_wss.second)});
              }
          }
      }

      /// Build the initial classes, and split until fix point.
      void build_classes_()
      {
        // Don't even bother splitting into final and non-final
        // states: post will be set apart anyway because of its
        // signature.
        std::unordered_set<class_t> classes;
        {
          const auto& all = a_->all_states();
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

                // If a class is too small to be split omit it from
                // further consideration.
                if (c_states.size() < 2)
                  {
                    i = classes.erase(i);
                    continue;
                  }

                // Try to find distinguishable states in c_states:
                signature_multimap signature_to_state(*this, num_classes_);
                for (auto s : c_states)
                  signature_to_state[& state_to_state_output_[s]].emplace_back(s);
                if (2 <= signature_to_state.size())
                  {
                    go_on = true;
                    i = classes.erase(i);
                    for (auto& p: signature_to_state)
                      {
                        class_t c2 = make_class(std::move(p.second), c);
                        classes.insert(c2);
                        c = class_invalid;
                      }
                  }
                else
                  ++i;
              } // for on classes
          }
        while (go_on);
      }

      /// The minimized automaton.
      automaton_t operator()()
      {
        build_classes_();
        return quotient(a_, class_to_set_);
      }
    };

  } // weighted::

  template <typename Aut>
  inline
  Aut
  minimize_weighted(const Aut& a)
  {
    detail_weighted::minimizer<Aut> minimize(a);
    return minimize();
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_MINIMIZE_WEIGHTED_HH
