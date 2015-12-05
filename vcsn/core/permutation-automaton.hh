#pragma once

#include <map>
#include <queue>
#include <unordered_map>

#include <vcsn/algos/copy.hh> // make_fresh_automaton
#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/core/fwd.hh> // permutation_automaton

namespace vcsn
{
  namespace detail
  {
    /// An automaton isomorphic to another one.
    ///
    /// \tparam Aut the type of the wrapped input automaton.
    template <typename Aut>
    class permutation_automaton_impl
      : public automaton_decorator<fresh_automaton_t_of<Aut>>
    {
    public:
      /// Input automaton type.
      using automaton_t = Aut;
      using context_t = context_t_of<automaton_t>;
      /// Generated automaton type.
      template <typename Ctx = context_t>
      using fresh_automaton_t = fresh_automaton_t_of<automaton_t, Ctx>;
      using super_t = automaton_decorator<fresh_automaton_t<>>;

      /// Symbolic state name: input automaton state type.
      using state_name_t = state_t_of<automaton_t>;
      /// Sorted automaton state type.
      using state_t = state_t_of<fresh_automaton_t<>>;

    public:
      permutation_automaton_impl(const automaton_t& input)
        : super_t(make_fresh_automaton(input))
        , input_(input)
      {
        map_[input_->pre()] = super_t::pre();
        map_[input_->post()] = super_t::post();
        todo_.push({input_->pre(), super_t::pre()});
      }

      /// Static name.
      static symbol sname()
      {
        static auto res = symbol{"permutation_automaton<"
                          + automaton_t::element_type::sname() + '>'};
        return res;
      }

      std::ostream& print_set(std::ostream& o, format fmt = {}) const
      {
        o << "permutation_automaton<";
        input_->print_set(o, fmt);
        return o << '>';
      }

      bool state_has_name(state_t) const
      {
        return true;
      }

      std::ostream&
      print_state_name(state_t s, std::ostream& o,
                       format fmt = {},
                       bool delimit = false) const
      {
        return input_->print_state_name(origins().at(s), o, fmt, delimit);
      }

      state_t
      state(state_name_t s)
      {
        // Benches show that the map_.emplace technique is slower, and
        // then that operator[] is faster than emplace.
        state_t res;
        auto i = map_.find(s);
        if (i == std::end(map_))
          {
            res = super_t::new_state();
            map_[s] = res;
            todo_.push({s, res});
          }
        else
          res = i->second;
        return res;
      }

      /// A map from each state to the origin state set it stands for.
      using origins_t = std::map<state_t, state_name_t>;

      /// Ordered map: state -> its derived term.
      const origins_t&
      origins() const
      {
        if (origins_.empty())
          for (const auto& p: map_)
            origins_[p.second] = p.first;
        return origins_;
      }

      using pair_t = std::pair<state_name_t, state_t>;
      std::queue<pair_t> todo_;

      /// Input-state -> sorted-state.
      std::unordered_map<state_name_t, state_t> map_;

      mutable origins_t origins_;

      /// Input automaton.
      const automaton_t input_;
    }; // class
  } // namespace detail
} // namespace vcsn
