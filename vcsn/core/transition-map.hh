#pragma once

#include <type_traits>

#include <vcsn/misc/map.hh> // vcsn::less

namespace vcsn
{
  namespace detail
  {

    /// Cache the outgoing transitions of an automaton as efficient
    /// maps label -> vector<(weight, dst)>.  Easy to zip.
    /// \tparam Aut
    ///    The automaton type.
    /// \tparam WeightSet
    ///    The set of weights into which the weights will be converted.
    /// \tparam Deterministic
    ///    Whether the automaton is guaranteed to be deterministic.
    ///    If it is, transition_map[state][label] returns a single
    ///    result, otherwise a vector.
    /// \tparam AllOut
    ///    Whether even the transitions to post() (via the
    ///    special label) are to be included.
    /// \tparam KeepTransitions
    ///    If enabled, add a "transition" field to the cache that
    ///    allows to recover the transition id.
    template <Automaton Aut,
              typename WeightSet = weightset_t_of<Aut>,
              bool Deterministic = false,
              bool AllOut = false,
              bool KeepTransitions = false>
    class transition_map
    {
    public:
      using state_t = state_t_of<Aut>;
      using transition_t = transition_t_of<Aut>;
      using weightset_t = WeightSet;
      using weight_t = typename weightset_t::value_t;

      /// Outgoing signature: weight, destination.
      template <typename Weight = weight_t,
                bool KeepTransitions_ = false, typename Dummy = void>
      struct transition_
      {
        transition_(weight_t w, state_t d, transition_t)
          : wgt(w)
          , dst(d)
        {}
        /// The (converted) weight.
        weight_t wgt;
        weight_t weight() const { return wgt; }
        state_t dst;
      };

      /// Boolean outgoing signature: destination.
      template <typename Dummy>
      struct transition_<bool, false, Dummy>
      {
        transition_(weight_t, state_t d, transition_t)
          : dst(d)
        {}
        static constexpr weight_t weight() { return true; }
        state_t dst;
      };

      /// Weighted outgoing signature: weight, destination, transition
      /// identifier.
      template <typename Weight, typename Dummy>
      struct transition_<Weight, true, Dummy>
      {
        transition_(weight_t w, state_t d, transition_t t)
          : wgt(w), dst(d), transition(t)
        {}
        /// The (converted) weight.
        weight_t wgt;
        weight_t weight() const { return wgt; }
        state_t dst;
        transition_t transition;
      };

      /// Boolean outgoing signature: destination, transition
      /// identifier.
      template <typename Dummy>
      struct transition_<bool, true, Dummy>
      {
        transition_(weight_t, state_t d, transition_t t)
          : dst(d), transition(t)
        {}
        static constexpr bool weight() { return true; }
        state_t dst;
        transition_t transition;
      };

      /// Outgoing signature: weight, destination, and possibly
      /// transition identifier.
      using transition = transition_<weight_t, KeepTransitions>;

      using transitions_t
        = std::conditional_t<Deterministic,
                             transition,
                             std::vector<transition>>;
      using map_t = std::map<label_t_of<Aut>, transitions_t,
                             less<labelset_t_of<Aut>>>;

      transition_map(const Aut& aut, const weightset_t& ws)
        : aut_(aut)
        , ws_(ws)
      {}

      transition_map(const Aut& aut)
        : transition_map(aut, *aut->weightset())
      {}

      /// Outgoing transitions of state \a s, sorted by label.
      map_t& operator[](state_t s)
      {
        auto lb = maps_.lower_bound(s);
        if (lb == maps_.end() || maps_.key_comp()(s, lb->first))
          return build_map_(lb, s);
        else
          return lb->second;
      }

    private:
      using maps_t = std::map<state_t, map_t>;

      /// Insert l -> t in map.
      template <bool Deterministic_>
      void
      insert_(map_t& map,
              label_t_of<Aut> l, transition t,
              std::enable_if_t<Deterministic_>* = nullptr)
      {
        map.emplace(l, t);
      }

      /// Insert l -> t in map.
      template <bool Deterministic_>
      void
      insert_(map_t& map,
              label_t_of<Aut> l, transition t,
              std::enable_if_t<!Deterministic_>* = nullptr)
      {
        map[l].emplace_back(t);
      }

      /// Build and return the transition map for state s, store at res.
      /// Insert it in the cache.
      map_t&
      build_map_(typename maps_t::iterator lb, state_t s)
      {
        auto& res = maps_.emplace_hint(lb, s, map_t{})->second;
        for (auto t: all_out(aut_, s))
          if (AllOut || !aut_->labelset()->is_special(aut_->label_of(t)))
            {
              auto w = ws_.conv(*aut_->weightset(), aut_->weight_of(t));
              insert_<Deterministic>(res,
                                     aut_->label_of(t),
                                     transition{w, aut_->dst_of(t), t});
            }
        return res;
      }

      /// The cache.
      maps_t maps_;
      /// The automaton whose transitions are cached.
      Aut aut_;
      /// The result weightset.
      const weightset_t& ws_;
    };
  }
}
