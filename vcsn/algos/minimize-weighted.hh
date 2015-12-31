#pragma once

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <vcsn/algos/tags.hh>
#include <vcsn/algos/accessible.hh> // is_trim
#include <vcsn/misc/indent.hh>
#include <vcsn/misc/raise.hh>

namespace vcsn
{

  /*-------------------------------------------------------------------------.
  | minimization with signatures; general version working on any weightset.  |
  `-------------------------------------------------------------------------*/

  namespace detail_weighted
  {
    template <Automaton Aut>
    class minimizer
    {
      using automaton_t = Aut;

      /// Input automaton, supplied at construction time.
      automaton_t a_;

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

      /// A signature: for each label, the outgoing class polynomial.
      using signature_t = std::map<label_t, class_polynomial_t>;
      /// The signature of state \a s.
      signature_t signature(state_t s) const
      {
        auto res = signature_t{};
        for (auto t: all_out(a_, s))
          cps_.add_here(res[a_->label_of(t)],
                        state_to_class_.at(a_->dst_of(t)),
                        a_->weight_of(t));
        return res;
      }

      struct signature_hasher
      {
        size_t operator()(const signature_t& sig) const noexcept
        {
          size_t res = 0;
          for (auto& t : sig)
            {
              hash_combine_hash(res, minimizer_.ls_.hash(t.first));
              hash_combine_hash(res, minimizer_.cps_.hash(t.second));
            }
          return res;
        }

        const minimizer& minimizer_;
      }; // class signature_hasher

      struct signature_equal_to
      {
        bool operator()(const signature_t& as,
                        const signature_t& bs) const noexcept
        {
          if (as.size() != bs.size())
            return false;

          using std::begin; using std::end;
          for (auto i = begin(as), i_end = end(as), j = begin(bs);
               i != i_end;
               ++i, ++j)
            if (!minimizer_.ls_.equal(i->first, j->first)
                || !minimizer_.cps_.equal(i->second, j->second))
              return false;
          return true;
        }

        const minimizer& minimizer_;
      }; // class signature_equal_to

      /// Cluster states per signature.
      using signature_multimap
        = std::unordered_map<signature_t, set_t,
                             signature_hasher, signature_equal_to>;

      void clear()
      {
        class_to_set_.clear();
        state_to_class_.clear();
        num_classes_ = 0;
      }

      /// Make a new class with the given set of states.
      ///
      /// \param set
      ///    the states belonging to this class
      /// \param number
      ///    the class number to use.  If \a class_invalid, allocate one.
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
        require(is_trim(a_), me(), ": input must be trim");
      }

      /// The minimized automaton.
      class_to_set_t& classes()
      {
        build_classes_();
        return class_to_set_;
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

        for (bool go_on = true; go_on; /* Nothing. */)
          {
            go_on = false;
            for (auto i = std::begin(classes), end = std::end(classes);
                 i != end;
                 /* Nothing. */)
              {
                auto c = *i;
                const set_t& c_states = class_to_set_.at(c);

                // Look for distinguishable states in c_states:
                // cluster the signatures.
                auto signature_to_state
                  = signature_multimap{1,
                                       signature_hasher{*this},
                                       signature_equal_to{*this}};
                for (auto s: c_states)
                  signature_to_state[signature(s)].emplace_back(s);

                if (2 <= signature_to_state.size())
                  {
                    // Split class c.
                    go_on = true;
                    i = classes.erase(i);
                    // To keep class numbers contiguous, reuse 'c' as
                    // first class number, and then use new one (via
                    // "c = class_invalid" below).
                    for (auto& p: signature_to_state)
                      {
                        bool singleton = p.second.size() == 1;
                        class_t c2 = make_class(std::move(p.second), c);
                        if (!singleton)
                          classes.insert(c2);
                        c = class_invalid;
                      }
                  }
                else
                  ++i;
              } // for on classes
          }
      }
    };

  } // weighted::

  template <Automaton Aut>
  inline
  auto
  minimize(const Aut& a, weighted_tag)
    -> quotient_t<Aut>
  {
    auto minimize = detail_weighted::minimizer<Aut>{a};
    return quotient(a, minimize.classes());
  }
} // namespace vcsn
