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
              typename Aut, typename... Auts>
    class lazy_tuple_automaton
      : public automaton_decorator<tuple_automaton<Aut, Auts...>>
    {
    public:

      /// The underlying automaton, output and inputs.
      using tuple_automaton_t = tuple_automaton<Aut, Auts...>;
      using state_name_t
        = typename tuple_automaton_t::element_type::state_name_t;
      using state_t
        = typename tuple_automaton_t::element_type::state_t;


      template <size_t... I>
      using seq
        = typename tuple_automaton_t::element_type::template seq<I...>;

      using self_t = lazy_tuple_automaton;
      using super_t = automaton_decorator<tuple_automaton_t>;

      /// The automaton holding the logic.
      using decorated_t = Decorated;

      using weightset_t = weightset_t_of<Aut>;

      using super_t::aut_;

      lazy_tuple_automaton(Aut aut, const Auts&... auts)
        : super_t{make_tuple_automaton(aut, auts...)}
        , transition_maps_{{auts, ws_}...}
      {}

      static symbol sname_()
      {
        static symbol res(tuple_automaton_t::element_type::sname_());
        return res;
      }


      /// A map from result state to tuple of original states.
      auto origins() const
        -> decltype(aut_->origins())
      {
        return aut_->origins();
      }

      /// Whether a given state's outgoing transitions have been
      /// computed.
      bool is_lazy(state_t s) const
      {
        return !has(done_, s);
      }

      /// Complete a state: find its outgoing transitions.
      void complete_(state_t s) const
      {
        const auto& orig = origins();
        state_name_t sn = orig.at(s);
        static_cast<decorated_t&>(const_cast<self_t&>(*this)).add_transitions(s, sn);
        done_.insert(s);
      }

      /// All the outgoing transitions.
      auto all_out(state_t s) const
        -> decltype(all_out(aut_, s))
      {
        if (is_lazy(s))
          complete_(s);
        return vcsn::detail::all_out(aut_, s);
      }

    protected:

      /// The type of our transition maps: convert the weight to weightset_t,
      /// non deterministic, and including transitions to post().
      template <typename A>
      using transition_map_t = transition_map<A, weightset_t,
                                              false, true,
                                              KeepTransitions>;


      /// Conversion from state name to state number.
      template <typename... Args>
      state_t state(Args&&... args)
      {
        return aut_->state(std::forward<Args>(args)...);
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

      /// When performing the lazy construction, list of states that
      /// have been completed (i.e., their outgoing transitions have
      /// been computed).
      mutable std::set<state_t> done_ = {aut_->post()};

    };
  }
}
