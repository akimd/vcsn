#pragma once

#include <unordered_map>
#include <unordered_set>

#include <vcsn/algos/accessible.hh> // is_trim
#include <vcsn/algos/quotient.hh>
#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/dynamic_bitset.hh>
#include <vcsn/misc/indent.hh>
#include <vcsn/misc/map.hh> // vcsn::less
#include <vcsn/misc/raise.hh>
#include <vcsn/weightset/fwd.hh> // b

namespace vcsn
{

  /*---------------------------------------------------------.
  | minimization with Moore's algorithm: signature variant.  |
  `---------------------------------------------------------*/

  /// Request for Moore implementation of minimize (B).
  struct signature_tag {};

  namespace detail_signature
  {
    template <typename Aut>
    class minimizer
    {
      static_assert(std::is_same<weightset_t_of<Aut>, b>::value,
                    "minimize: signature: requires Boolean weights");

      using automaton_t = Aut;

      /// Input automaton, supplied at construction time.
      automaton_t a_;

      using labelset_t = labelset_t_of<automaton_t>;
      const labelset_t& ls_;

      using label_t = label_t_of<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using class_t = unsigned;
      using set_t = std::vector<state_t>;
      using state_to_class_t = std::unordered_map<state_t, class_t>;
      using class_to_set_t = std::vector<set_t>;

      constexpr static const char* me() { return "minimize-signature"; }

      /// An invalid class.
      constexpr static class_t class_invalid = -1;
      unsigned num_classes_ = 0;

      class_to_set_t class_to_set_;
      state_to_class_t state_to_class_;

      // For a given state, destination states for a specific label.
      struct state_output_for_label_t
      {
        // For some unstored state.
        label_t label;
        std::vector<state_t> to_states; // Ordered.
      };

      // This is sorted by label.
      using state_output_t = std::vector<state_output_for_label_t>;

      /// Sort of a transition map for each state:
      /// state -> vector of (label, destination states).
      std::unordered_map<state_t, state_output_t> state_to_state_output_;

      struct signature_hasher
      {
        // FIXME: G++ 4.9 requires this ctor, which is wrong.
        signature_hasher(const minimizer& m)
          : minimizer_(m)
        {}

        size_t operator()(const state_output_t& state_output) const noexcept
        {
          size_t res = 0;
          dynamic_bitset bits(num_classes_);
          for (auto& t : state_output)
            {
              const label_t& label = t.label;
              hash_combine_hash(res, minimizer_.ls_.hash(label));
              // Hash the set of classes reached with label.  Of
              // course the hash must not depend on class ordering.
              bits.reset();
              for (auto s : t.to_states)
                bits.set(state_to_class_.at(s));
              vcsn::hash_combine(res, bits);
            }
          return res;
        }

        size_t operator()(state_t s) const noexcept
        {
          return operator()(minimizer_.state_to_state_output_.at(s));
        }

        const minimizer& minimizer_;
        const state_to_class_t& state_to_class_ = minimizer_.state_to_class_;
        unsigned num_classes_ = minimizer_.num_classes_;
      }; // class signature_hasher

      struct signature_equal_to
      {
        // FIXME: G++ 4.9 requires this ctor, which is wrong.
        signature_equal_to(const minimizer& m)
          : minimizer_(m)
        {}
        bool operator()(const state_output_t& as,
                        const state_output_t& bs) const noexcept
        {
          if (as.size() != bs.size())
            return false;

          dynamic_bitset a_bits(num_classes_), b_bits(num_classes_);
          for (auto i = as.cbegin(), i_end = as.cend(), j = bs.cbegin();
               i != i_end;
               ++i, ++j)
            {
              if (! ls_.equal(i->label, j->label))
                return false;

              a_bits.reset(); b_bits.reset();
              for (auto s : i->to_states)
                a_bits.set(state_to_class_.at(s));
              for (auto s : j->to_states)
                b_bits.set(state_to_class_.at(s));
              if (a_bits != b_bits)
                return false;
            }

          return true;
        }

        bool operator()(const state_t& a, const state_t& b) const noexcept
        {
          return operator()(minimizer_.state_to_state_output_.at(a),
                            minimizer_.state_to_state_output_.at(b));
        }

        const minimizer& minimizer_;
        const labelset_t& ls_ = minimizer_.ls_;
        const state_to_class_t& state_to_class_ = minimizer_.state_to_class_;
        const size_t num_classes_ = minimizer_.num_classes_;
      }; // class signature_equal_to

      /// Cluster states per signature.
      ///
      /// This appears to be hashing on states, but we actually hash
      /// on the states' signatures behind the scene.
      using signature_multimap
        = std::unordered_map<state_t, set_t,
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
      {
        require(is_trim(a_), me(), ": input must be trim");

        // Fill state_to_state_output.
        for (auto s : a_->all_states())
          {
            // Sort of a transition-map for state s.
            using label_to_states_t
              = std::map<label_t, std::vector<state_t>, vcsn::less<labelset_t>>;

            // Get the out-states from s, by label:
            label_to_states_t label_to_states;
            for (auto t : a_->all_out(s))
              label_to_states[a_->label_of(t)].emplace_back(a_->dst_of(t));

            // Associate this information to s, as a vector sorted by label:
            state_output_t& state_output = state_to_state_output_[s];
            for (auto& l_ss : label_to_states)
              {
                boost::sort(l_ss.second);
                state_output.emplace_back(state_output_for_label_t{l_ss.first,
                      std::move(l_ss.second)});
              }
          }
      }

      /// The partition, as a list of classes.
      class_to_set_t& classes()
      {
        build_classes_();
        return class_to_set_;
      }

    private:
      /// Build the initial classes, and split until fix point.
      void build_classes_()
      {
        // The list of classes candidates for splitting.
        //
        // Don't even bother to split between final and non-final
        // states, this initialization is useless.
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
                 /* nothing. */)
              {
                auto c = *i;
                const set_t& c_states = class_to_set_.at(c);

                // Look for distinguishable states in c_states:
                // cluster the signatures.
                auto sig_to_state
                  = signature_multimap{1,
                                       signature_hasher{*this},
                                       signature_equal_to{*this}};
                for (auto s : c_states)
                  sig_to_state[s].emplace_back(s);

                if (2 <= sig_to_state.size())
                  {
                    // Split class c.
                    go_on = true;
                    i = classes.erase(i);
                    // To keep class numbers contiguous, reuse 'c' as
                    // first class number, and then use new one (via
                    // "c = class_invalid" below).
                    for (auto& p: sig_to_state)
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

  } // detail_signature::

  template <typename Aut>
  inline
  std::enable_if_t<std::is_same<weightset_t_of<Aut>, b>::value,
                    quotient_t<Aut>>
  minimize(const Aut& a, signature_tag)
  {
    auto minimize = detail_signature::minimizer<Aut>{a};
    return quotient(a, minimize.classes());
  }

  namespace dyn
  {
    namespace detail
    {
      template <typename Aut>
      ATTRIBUTE_NORETURN
      inline
      std::enable_if_t<!std::is_same<weightset_t_of<Aut>, b>::value,
                        quotient_t<Aut>>
      minimize(const Aut&, signature_tag)
      {
        raise("minimize: invalid algorithm (non-Boolean): signature");
      }
    }
  }
} // namespace vcsn
