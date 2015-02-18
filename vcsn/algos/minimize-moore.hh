#pragma once

#include <unordered_map>
#include <vector>

#include <vcsn/algos/accessible.hh> // is_trim
#include <vcsn/algos/is-deterministic.hh>
#include <vcsn/algos/quotient.hh>
#include <vcsn/core/transition-map.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/weightset/fwd.hh> // b

namespace vcsn
{

  /*--------------------------------------.
  | minimization with Moore's algorithm.  |
  `--------------------------------------*/
  namespace detail_moore
  {
    template <typename Aut>
    class minimizer
    {
      static_assert(std::is_same<weightset_t_of<Aut>, b>::value,
                    "minimize: moore: requires Boolean weights");
      static_assert(labelset_t_of<Aut>::is_free(),
                    "minimize: moore: requires free labelset");

      using automaton_t = Aut;

      /// Input automaton, supplied at construction time.
      const automaton_t& a_;

      /// The generators.
      const typename labelset_t_of<Aut>::genset_t gs_;

      using label_t = label_t_of<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using class_t = unsigned;
      using classes_t = std::vector<class_t>;
      using set_t = std::vector<state_t>;
      using state_to_class_t = std::unordered_map<state_t, class_t>;
      using target_class_to_states_t = std::unordered_map<class_t, set_t>;
      using class_to_set_t = std::vector<set_t>;
      using class_to_state_t = std::vector<state_t>;

      constexpr static const char* me() { return "minimize-moore"; }

      /// An invalid class.
      constexpr static class_t class_invalid = -1;
      unsigned num_classes_ = 0;

      // First two classes are reserved, and are empty.
      class_to_set_t class_to_set_;
      state_to_class_t state_to_class_;
      class_to_state_t class_to_res_state_;

      std::ostream& print_(const set_t& ss, std::ostream& o) const
      {
        o << '{';
        const char* sep = "";
        for (auto s : ss)
          {
            o << sep << s;
            sep = ", ";
          }
        return o << '}';
      }
      std::ostream& print_(const class_to_set_t& c2ss, std::ostream& o) const
      {
        const char* sep = "";
        for (unsigned i = 0; i < c2ss.size(); ++i)
          {
            o << sep << '[' << i << "] = ";
            print_(c2ss[i], o);
            sep = "\n";
          }
        return o;
      }

      /// An auxiliary data structure enabling fast access to
      /// transitions from a given state and label, in random order.
      using transition_map_t
        = detail::transition_map<automaton_t,
                                 weightset_t_of<automaton_t>,
                                 /* Deterministic: */ true,
                                 /* AllOut:        */ true>;
      transition_map_t transition_map_;

      void clear()
      {
        class_to_set_.clear();
        state_to_class_.clear();
        num_classes_ = 0;
        class_to_res_state_.clear();
        transition_map_.clear();
      }

      /// Make a new class with the given set of states.
      class_t make_class(set_t&& set, class_t number = class_invalid)
      {
        assert(! set.empty());

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
        const auto& map = transition_map_[s];
        auto dst = map.find(l);
        if (dst == std::end(map))
          return class_invalid;
        else
          return state_to_class_[dst->second.dst];
      }

    public:
      minimizer(const Aut& a)
        : a_(a)
        , gs_(a_->labelset()->genset())
        , transition_map_(a)
      {
        // We _really_ need determinism here.  See for instance
        // minimization of standard(aa+a) (not a+aa).
        require(is_deterministic(a_), me(), ": input must be deterministic");
        require(is_trim(a_), me(), ": input must be trim");
      }

      /// Build the initial classes, and split until fix point.
      void build_classes_()
      {
        // Initialization: two classes, partitioning final and
        // non-final states.
        make_class({a_->pre()});
        make_class({a_->post()});
        {
          set_t nonfinals, finals;
          for (auto s : a_->states())
            if (a_->is_final(s))
              finals.emplace_back(s);
            else
              nonfinals.emplace_back(s);
          if (! nonfinals.empty())
            make_class(std::move(nonfinals));
          if (! finals.empty())
            make_class(std::move(finals));
        }

        bool go_on = true;
        while (go_on)
          {
            go_on = false;
            for (class_t c = 0; c < num_classes_; ++c)
              {
                const set_t& c_states = class_to_set_[c];
                for (auto l : gs_)
                  {
                    // See the "alreadyminimal" test comment in
                    // tests/python/minimize.py.
                    target_class_to_states_t target_class_to_c_states;
                    for (auto s : c_states)
                      {
                        auto c2 = out_class(s, l);
                        target_class_to_c_states[c2].emplace_back(s);
                      }

                    // Are there at least two keys?
                    //
                    // std::unordered_map::size is said to be O(1).
                    if (2 <= target_class_to_c_states.size())
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
      }

      /// Return the quotient.
      partition_automaton<automaton_t> operator()()
      {
        build_classes_();
        return quotient(a_, class_to_set_);
      }
    };
  }

  /// Minimize automaton \a a using the Moore algorithm.
  template <typename Aut>
  inline
  auto
  minimize_moore(const Aut& a)
    -> partition_automaton<Aut>
  {
    detail_moore::minimizer<Aut> minimize(a);
    return minimize();
  }

} // namespace vcsn
