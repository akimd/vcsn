#pragma once

#include <vcsn/core/automaton.hh>
#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/core/transition-map.hh>
#include <vcsn/core/tuple-automaton.hh>

namespace vcsn
{
  namespace detail
  {
    /// Decorator implementing the laziness for an algorithm.
    ///
    /// The caller must inherit from lazy_tuple_automaton, and pass itself as
    /// first template parameter of lazy_automaton (Curiously Recurring
    /// Template Pattern), e.g.:
    /// class A : lazy_tuple_automaton<A, ...>
    ///
    /// The caller must also define the add_transitions method:
    /// void add_transitions(const state_t src,
    ///                      const state_name_t& psrc)
    ///
    /// This method tells the lazy decorator how to compute the outgoing
    /// transitions for a state.
    ///
    /// \tparam Decorated        the class inheriting from lazy_tuple_automaton
    /// \tparam KeepTransitions  see vcsn/core/transition-map.hh
    /// \tparam Aut              the output automaton type
    /// \tparam Auts             the input automaton types
    template <typename Decorated,
              bool KeepTransitions,
              bool Lazy,
              Automaton Aut, Automaton... Auts>
    class lazy_tuple_automaton
      : public automaton_decorator<tuple_automaton<Aut, Auts...>>
    {
    public:

      /// The underlying automaton, output and inputs.
      using tuple_automaton_t = tuple_automaton<Aut, Auts...>;
      using tuple_automaton_impl = typename tuple_automaton_t::element_type;
      using state_name_t = typename tuple_automaton_impl::state_name_t;
      using state_t = typename tuple_automaton_impl::state_t;


      template <size_t... I>
      using seq = typename tuple_automaton_impl::template seq<I...>;

      using self_t = lazy_tuple_automaton;
      using super_t = automaton_decorator<tuple_automaton_t>;

      /// The automaton holding the logic.
      using decorated_t = Decorated;

      using weightset_t = weightset_t_of<Aut>;

      using super_t::aut_;

      // Clang 3.5 is wrong about ws_ being used before being defined.
#if defined __clang__
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wuninitialized"
#endif
      lazy_tuple_automaton(Aut aut, const Auts&... auts)
        : super_t{make_tuple_automaton(aut, auts...)}
        , transition_maps_{transition_map_t<Auts>{auts, ws_}...}
      {
        if (Lazy)
          this->set_lazy(this->pre());
      }
#if defined __clang__
# pragma clang diagnostic pop
#endif
      template <typename... T>
      static symbol sname_(const T&... t)
      {
        static auto res = symbol{tuple_automaton_impl::sname_(t...)};
        return res;
      }


      /// A map from result state to tuple of original states.
      auto origins() const
        -> decltype(aut_->origins())
      {
        return aut_->origins();
      }

      /// Complete a state: find its outgoing transitions.
      void complete_(state_t s) const
      {
        const auto& orig = origins();
        state_name_t sn = orig.at(s);
        auto& self = static_cast<decorated_t&>(const_cast<self_t&>(*this));
        if (Lazy)
          self.set_lazy(s, false);
        self.add_transitions(s, sn);
      }

      /// All the outgoing transitions.
      auto all_out(state_t s) const
        -> decltype(all_out(aut_, s))
      {
        if (this->is_lazy(s))
          complete_(s);
        return vcsn::detail::all_out(aut_, s);
      }

    protected:

      /// The type of our transition maps: convert the weight to weightset_t,
      /// non deterministic, and including transitions to post().
      template <Automaton A>
      using transition_map_t = transition_map<A, weightset_t,
                                              false, true,
                                              KeepTransitions>;

      /// Conversion from state name to state number.
      template <typename... Args>
      state_t state(Args&&... args)
      {
        return aut_->template state<Lazy>(std::forward<Args>(args)...);
      }

      /// The outgoing tuple of transitions from state tuple \a ss.
      std::tuple<typename transition_map_t<Auts>::map_t&...>
      out_(const state_name_t& ss)
      {
        return out_(ss, aut_->indices);
      }

      template <size_t... I>
      std::tuple<typename transition_map_t<Auts>::map_t&...>
      out_(const state_name_t& ss, seq<I...>)
      {
        return std::tie(std::get<I>(transition_maps_)[std::get<I>(ss)]...);
      }


      /// The resulting weightset.
      const weightset_t& ws_ = *aut_->weightset();

      /// Transition caches.
      std::tuple<transition_map_t<Auts>...> transition_maps_;
    };
  }
}
