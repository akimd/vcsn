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
      /// State index type.
      using state_t = state_t_of<Aut>;
      /// Transition index type.
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
        : maps_(aut->all_states().back() + 1)
        , aut_(aut)
        , ws_(ws)
      {}

      transition_map(const Aut& aut)
        : transition_map(aut, *aut->weightset())
      {}

      transition_map(transition_map&& that)
        : maps_(std::move(that.maps_))
        , aut_(std::move(that.aut_))
        , ws_(that.ws_)
      {}

      /// Outgoing transitions of state \a s, sorted by label.
      map_t& operator[](state_t s)
      {
        // We might be working on a lazy automaton, so be prepared to
        // find states that did not exist when we created this
        // transition map.
        if (maps_.size() <= s)
          {
            auto capacity = maps_.capacity();
            while (capacity <= s)
              capacity *= 2;
            maps_.reserve(capacity);
            maps_.resize(s + 1);
          }
        auto& res = maps_[s];
        if (!res)
          {
            res = std::make_unique<map_t>();
            build_map_(*res, s);
          }
        return *res;
      }

    private:
      /// For each state number, its transition map.
      using maps_t = std::vector<std::unique_ptr<map_t>>;

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

      /// Build the transition map for state \a s, store at \a map.
      void
      build_map_(map_t& map, state_t s)
      {
        for (auto t: all_out(aut_, s))
          if (AllOut || !aut_->labelset()->is_special(aut_->label_of(t)))
            {
              auto w = ws_.conv(*aut_->weightset(), aut_->weight_of(t));
              insert_<Deterministic>(map,
                                     aut_->label_of(t),
                                     transition{w, aut_->dst_of(t), t});
            }
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
