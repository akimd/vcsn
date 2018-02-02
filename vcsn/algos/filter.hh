#pragma once

#include <algorithm> // std::max
#include <optional>

#include <boost/range/irange.hpp>

#include <vcsn/algos/copy.hh>
#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/core/automaton.hh> // all_transitions
#include <vcsn/dyn/fwd.hh>
#include <vcsn/misc/crange.hh>
#include <vcsn/misc/dynamic_bitset.hh>
#include <vcsn/misc/sparse-set.hh>
#include <vcsn/misc/vector.hh>
#include <vcsn/misc/static-if.hh>

namespace vcsn
{
  /*---------------.
  | Function tag.  |
  `---------------*/

  CREATE_FUNCTION_TAG(filter);

  namespace detail
  {
    /// Enables or not the presence of a container in a class.
    template <typename Container, bool Has = false>
    class optional_container
    {
    public:
      template <typename... Args>
      optional_container(Args&&...)
      {}
    };

    template <typename Container>
    class optional_container<Container, true>
    {
    public:
      template <typename... Args>
      optional_container(Args&&... args)
        : cont_(std::forward<Args>(args)...)
      {}
    protected:
      Container cont_;
    };

    /// Hide some states of an automaton.
    ///
    /// Parametrized on whether or not to consider transitions while filtering.
    template <Automaton Aut, bool Trans = false>
    class filter_automaton_impl
      : public automaton_decorator<Aut>
      , public optional_container<dynamic_bitset, Trans>
    {
    public:
      using automaton_t = Aut;
      using self_t = filter_automaton_impl;
      using super_t = automaton_decorator<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;
      using label_t = label_t_of<automaton_t>;

      using states_t = dynamic_bitset;
      using transitions_t = dynamic_bitset;
      using optional_container_t = optional_container<transitions_t, Trans>;

      using tr_cont_t = std::vector<transition_t>;

      using super_t::pre;
      using super_t::post;
      using super_t::src_of;
      using super_t::dst_of;

      /// Build a filtered view of an automaton.
      ///
      /// \param input  automaton whose states/transitions to filter.
      /// \param ss     set of states to reveal.  If {}, hide none.
      /// \param ts     set of transitions to reveal.  If {}, hide none.
      filter_automaton_impl(const automaton_t& input,
                            const std::optional<states_t>& ss = {},
                            const std::optional<transitions_t>& ts = {})
        : super_t(input)
          // FIXME: wasting space allocating the transition vector,
          // even if we don't use it.
        , optional_container_t(ts ? *ts : transitions_t(transitions_size(input)))
        , ss_(ss ? *ss : states_t(states_size(input)))
      {
        if (ss)
          {
            ss_.set(input->pre());
            ss_.set(input->post());
          }
        else
          unhide_all_states();
        if constexpr (Trans)
          if (!ts)
            unhide_all_transitions();
        properties_.update(filter_ftag{});
      }

      /// Static name.
      static symbol sname()
      {
        static auto res = symbol{"filter_automaton<"
                          + automaton_t::element_type::sname() + '>'};
        return res;
      }

      std::ostream& print_set(std::ostream& o, format fmt = {}) const
      {
        o << "filter_automaton<";
        aut_->print_set(o, fmt);
        return o << '>';
      }

      bool state_has_name(state_t s) const
      {
        assert(has(ss_, s));
        return aut_->state_has_name(s);
      }

      bool has_state(state_t s) const
      {
        return has(ss_, s) && aut_->has_state(s);
      }

      using super_t::has_transition;

      template <bool U = Trans>
      std::enable_if_t<U, bool>
      has_transition(transition_t t) const
      {
        return has(this->cont_, t) && aut_->has_transition(t);
      }

      template <bool U = Trans>
      std::enable_if_t<!U, bool>
      has_transition(transition_t t) const
      {
        return aut_->has_transition(t);
      }

      std::ostream& print_state_name(state_t s, std::ostream& o,
                                     format fmt = {},
                                     bool delimit = false) const
      {
        assert(has(ss_, s));
        return aut_->print_state_name(s, o, fmt, delimit);
      }

      size_t num_states() const
      {
        return states().size();
      }

      size_t num_all_states() const
      {
        return all_states().size();
      }

      template <typename Pred>
      auto all_states(Pred pred) const
      {
        return aut_->all_states([this, pred](state_t s)
                                {
                                  return pred(s) && has(ss_, s);
                                });
      }

      auto all_states() const
      {
        return all_states([](state_t) { return true; });
      }

      auto states() const
      {
        return all_states([this](state_t s)
                          {
                            // When transposing post() < pre().
                            return std::max(pre(), post()) < s;
                          });
      }

      /// All the transition indexes between all states (including pre
      /// and post).
      auto all_transitions() const
      {
        return vcsn::detail::all_transitions
          (aut_,
           [this](transition_t t)
           {
             return has_transition(t)
                    && has(ss_, aut_->src_of(t))
                    && has(ss_, aut_->dst_of(t));
           });
      }

      /// All the outgoing transitions.
      auto all_out(state_t s) const
      {
        return vcsn::detail::all_out(aut_, s,
                                     [this](transition_t t)
                                     {
                                       return has_transition(t)
                                              && has(ss_, aut_->dst_of(t));
                                     });
      }

      /// All the incoming transitions.
      auto all_in(state_t s) const
      {
        return vcsn::detail::all_in(aut_, s,
                                    [this](transition_t t)
                                    {
                                      return has_transition(t)
                                             && has(ss_, aut_->src_of(t));
                                    });
      }

      void
      hide_state(state_t s)
      {
        if (s < ss_.size())
          ss_.reset(s);
      }

      /// Hide a transition from the automaton.
      /// Only available if the filter_automaton filters with the transitions.
      template <bool U = Trans>
      std::enable_if_t<U, void>
      hide_transition(transition_t t)
      {
        if (t < optional_container_t::cont_.size())
          optional_container_t::cont_.reset(t);
      }

      /// Reveal a state from the original automaton.
      void
      unhide_state(state_t s)
      {
        if (s < ss_.size())
          ss_.set(s);
      }

      /// Reveal a transition from the original automaton.
      /// Only available if the filter_automaton filters with the transitions.
      template <bool U = Trans>
      std::enable_if_t<U, void>
      unhide_transition(transition_t t)
      {
        if (t < optional_container_t::cont_.size())
          optional_container_t::cont_.set(t);
      }

      /// Hide all the states of the original automaton.
      /// The filter automaton is now empty.
      /// automaton.
      void
      hide_all_states()
      {
        ss_.reset();
        ss_.set(aut_->pre());
        ss_.set(aut_->post());
      }

      /// Reveal all the states of the original automaton.
      /// The filter automaton is now composed of all the states of the original
      /// automaton.
      void
      unhide_all_states()
      {
        ss_.set();
      }

      template <bool U = Trans>
      std::enable_if_t<U, void>
      unhide_all_transitions()
      {
        optional_container_t::cont_.set();
      }

      template <bool U = Trans>
      std::enable_if_t<U, void>
      hide_all_transitions()
      {
        optional_container_t::cont_.reset();
      }

      fresh_automaton_t_of<automaton_t>
      strip() const
      {
        auto state_filter = [this](state_t_of<automaton_t> s)
                            {
                              return has(this->ss_, s);
                            };
        auto transition_filter = [this](transition_t_of<automaton_t> t)
                                 {
                                   return this->has_transition(t);
                                 };
        return ::vcsn::copy(aut_, state_filter, transition_filter);
      }

      /// Accessor to the property cache.
      auto& properties()
      {
        return properties_;
      }

    protected:
      /// The "full" automaton whose some states are hidden.
      using super_t::aut_;

    private:
      /// The states we keep.
      states_t ss_;
      /// Cache of properties, which can be empty values ("unknown").
      property_cache properties_;
   };
  }

  template <Automaton Aut, bool Trans = false>
  using filter_automaton =
    std::shared_ptr<detail::filter_automaton_impl<Aut, Trans>>;

  /// Build a filtered view of an automaton.
  ///
  /// \param aut  automaton whose states/transitions to filter.
  /// \param ss   set of states to reveal.  If {}, hide none.
  /// \param ts   set of transitions to reveal.  If {}, hide none.
  template <Automaton Aut, bool Trans = false>
  filter_automaton<Aut, Trans>
  filter(const Aut& aut,
         std::optional<dynamic_bitset> ss = {},
         std::optional<dynamic_bitset> ts = {})
  {
    return make_shared_ptr<filter_automaton<Aut, Trans>>(aut, ss, ts);
  }

  /// Build a filtered view of an automaton.
  ///
  /// \param aut  automaton whose states/transitions to filter.
  /// \param ss   set of states to reveal.
  template <Automaton Aut>
  filter_automaton<Aut>
  filter(const Aut& aut, const std::unordered_set<state_t_of<Aut>>& ss)
  {
    return filter(aut, make_dynamic_bitset(ss, states_size(aut)));
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut, typename Unsigneds>
      automaton
      filter(const automaton& aut, const std::vector<unsigned>& states)
      {
        const auto& a = aut->as<Aut>();
        // FIXME: This is a problem for lazy automaton.
        auto size = states_size(a);
        auto ss = dynamic_bitset(size);
        for (auto s: states)
          if (s + 2 < size)
            ss.set(s + 2);
        return ::vcsn::filter(a, ss);
      }
    }
  }
}
