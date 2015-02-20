#pragma once

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <vcsn/algos/accessible.hh> // is_trim
#include <vcsn/misc/algorithm.hh> // same_domain
#include <vcsn/misc/indent.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/zip-maps.hh>

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

      /// An auxiliary data structure enabling fast access to
      /// transitions from a given state and label, in random order.
      using transition_map_t
        = detail::transition_map<automaton_t,
                                 weightset_t_of<automaton_t>,
                                 /* Deterministic: */ false,
                                 /* AllOut:        */ true>;
      transition_map_t transition_map_;

      /// Outgoing transitions of a state: a map label -> destinations.
      using state_output_t = typename transition_map_t::map_t;

      /// List of destinations.
      using transitions_t = typename transition_map_t::transitions_t;

      /// Dealing with class numbers.
      struct classset
      {
        using value_t = class_t;

        using kind_t = void;
        static bool equal(class_t l, class_t r)
        {
          return l == r;
        }

        static bool less(class_t l, class_t r)
        {
          return l < r;
        }

        static size_t hash(class_t s)
        {
          return hash_value(s);
        }
      };

      /// The output of a given letter from a given state, keeping
      /// into account classes and weights.
      using class_polynomialset_t
        = polynomialset<context<classset, weightset_t>>;

      /// Class polynomialset.
      class_polynomialset_t cps_{{classset{}, ws_}};

      using class_polynomial_t = typename class_polynomialset_t::value_t;

      /// The image of \t as a polynomial of weighted classes.
      class_polynomial_t class_polynomial(const transitions_t& ts) const
      {
        class_polynomial_t res;
        for (const auto& t : ts)
          cps_.add_here(res, state_to_class_.at(t.dst), t.weight());
        return res;
      }

      friend class signature_hasher;
      class signature_hasher : public std::hash<state_output_t*>
      {
        const minimizer& minimizer_;
        unsigned num_classes_;
      public:
        signature_hasher(minimizer& the_minimizer, size_t num_classes)
          : minimizer_(the_minimizer)
          , num_classes_(num_classes)
        {}

        size_t operator()(const state_output_t* state_output_) const noexcept
        {
          const state_output_t& state_output = *state_output_;
          size_t res = 0;
          for (auto& t : state_output)
            {
              auto p = minimizer_.class_polynomial(t.second);
              // I've chosen *not* to hash the label when all
              // transitions with a given label cancel one another.
              if (! p.empty())
                std::hash_combine(res, minimizer_.ls_.hash(t.first));
              std::hash_combine(res, minimizer_.cps_.hash(p));
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

        /// Check that the image of two transition vectors on classes
        /// coincidence.
        bool match(const transitions_t& a,
                   const transitions_t& b) const noexcept
        {
          // Polynomials of classes.
          auto pa = minimizer_.class_polynomial(a);
          auto pb = minimizer_.class_polynomial(b);
          return minimizer_.cps_.equal(pa, pb);
        }

        bool operator()(const state_output_t *as_,
                        const state_output_t *bs_) const noexcept
        {
          const state_output_t& as = *as_;
          const state_output_t& bs = *bs_;

          if (!same_domain(as, bs))
            return false;
          // Check that we have the same images.
          for (auto z: zip_maps<as_pair>(as, bs))
            if (! match(std::get<0>(z.second), std::get<1>(z.second)))
              return false;

          return true;
        }
      }; // class signature_equal_to

      friend class signature_multimap;
      class signature_multimap
        : public std::unordered_map<state_output_t*, set_t,
                                    signature_hasher, signature_equal_to>
      {
        minimizer& minimizer_;
        using super_t
          = std::unordered_map<state_output_t*, set_t,
                               signature_hasher, signature_equal_to>;
      public:
        signature_multimap(minimizer& the_minimizer,
                           const size_t class_bound)
          : super_t(1,
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
        , transition_map_(a)
      {
        require(is_trim(a_), me(), ": input must be trim");
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
                  signature_to_state[& transition_map_[s]].emplace_back(s);
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
      partition_automaton<automaton_t> operator()()
      {
        build_classes_();
        return quotient(a_, class_to_set_);
      }
    };

  } // weighted::

  template <typename Aut>
  inline
  auto
  minimize_weighted(const Aut& a)
    -> partition_automaton<Aut>
  {
    detail_weighted::minimizer<Aut> minimize(a);
    return minimize();
  }
} // namespace vcsn
