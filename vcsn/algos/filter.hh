#pragma once

#include <algorithm> // std::max
#include <boost/range/irange.hpp>
#include <vcsn/algos/copy.hh>
#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/misc/crange.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/misc/unordered_set.hh>
#include <vcsn/misc/vector.hh>
#include <vcsn/misc/sparse-set.hh>
#include <vcsn/misc/static-if.hh>

namespace vcsn
{
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
    /// Parametrized on whether or not to consider transitions while filtering,
    /// and whether or not the filtering sets are excluding or including elements.
    template <Automaton Aut, bool Trans = false, bool Exclusion = false>
    class filter_automaton_impl
      : public automaton_decorator<Aut>
      , public optional_container<sparse_set, Trans>
    {
    public:
      using automaton_t = Aut;
      using self_t = filter_automaton_impl;
      using super_t = automaton_decorator<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;
      using label_t = label_t_of<automaton_t>;

      /// States set that we want to filter.
      /// Using std::unordered_set because when test states_t and
      /// accessible states are std::unordered_set the
      /// score(perfomance) is better(x2) than using std::set.
      using states_t = sparse_set;
      using transitions_t = sparse_set;
      using optional_container_t = optional_container<transitions_t, Trans>;

      using tr_cont_t = std::vector<transition_t>;

      using super_t::pre;
      using super_t::post;
      using super_t::src_of;
      using super_t::dst_of;

      filter_automaton_impl(const automaton_t& input,
                            const states_t& ss = {},
                            const transitions_t& ts = {})
        : super_t(input), optional_container_t(ts), ss_(ss)
      {
        set_max_size(states_size(input));

        if (!Exclusion)
          {
            ss_.emplace(input->pre());
            ss_.emplace(input->post());
          }
      }

      template <bool U = Trans>
      std::enable_if_t<U, void>
      set_max_size(unsigned size)
      {
        ss_.set_max_size(size);
        optional_container_t::cont_.set_max_size(transitions_size(aut_));
      }

      template <bool U = Trans>
      std::enable_if_t<!U, void>
      set_max_size(unsigned size)
      {
        ss_.set_max_size(size);
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

      template <typename Container, typename Elt>
      bool filter(const Container& c, Elt e) const
      {
        return Exclusion ^ has(c, e);
      }

      bool state_has_name(state_t s) const
      {
        assert(filter(ss_, s));
        return aut_->state_has_name(s);
      }

      bool has_state(state_t s) const
      {
        return filter(ss_, s) && aut_->has_state(s);
      }

      using super_t::has_transition;

      template <bool U = Trans>
      std::enable_if_t<U, bool>
      has_transition(transition_t t) const
      {
        return static_if<Trans>(
                      [this](auto t)
                      {
                        return filter(this->cont_, t) && aut_->has_transition(t);
                      },
                      [](auto) { return true; })(t);
      }

      std::ostream& print_state_name(state_t s, std::ostream& o,
                                     format fmt = {},
                                     bool delimit = false) const
      {
        assert(filter(ss_, s));
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
                                  return pred(s) && filter(ss_, s);
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
                    && filter(ss_, aut_->src_of(t))
                    && filter(ss_, aut_->dst_of(t));
           });
      }

      /// All the outgoing transitions.
      auto all_out(state_t s) const
      {
        return vcsn::detail::all_out(aut_, s,
                                     [this](transition_t t)
                                     {
                                       return has_transition(t)
                                              && filter(ss_, aut_->dst_of(t));
                                     });
      }

      /// All the incoming transitions.
      auto all_in(state_t s) const
      {
        return vcsn::detail::all_in(aut_, s,
                                    [this](transition_t t)
                                    {
                                      return has_transition(t)
                                             && filter(ss_, aut_->src_of(t));
                                    });
      }

      void
      hide_state(state_t s)
      {
        if (Exclusion)
          ss_.insert(s);
        else
          ss_.erase(s);
      }

      /// Hide a transition from the automaton.
      /// Only available if the filter_automaton filters with the transitions.
      template <bool U = Trans>
      std::enable_if_t<U, void>
      hide_trans(transition_t t)
      {
        if (Exclusion)
          optional_container_t::cont_.insert(t);
        else
          optional_container_t::cont_.erase(t);
      }

      fresh_automaton_t_of<automaton_t>
      strip() const
      {
        return ::vcsn::copy(aut_,
                            [this](state_t_of<automaton_t> s)
                            {
                              return has(this->ss_, s);
                            });
      }

    protected:
      /// The "full" automaton whose some states are hidden.
      using super_t::aut_;

    private:
      /// The states we keep.
      states_t ss_;
   };
  }

  template <Automaton Aut, bool Trans = false, bool Exclusion = false>
  using filter_automaton =
    std::shared_ptr<detail::filter_automaton_impl<Aut, Trans, Exclusion>>;

  /// Get an automaton who is a part state set \a ss of \a aut.
  template <Automaton Aut, bool Trans = false, bool Exclusion = false>
  filter_automaton<Aut, Trans, Exclusion>
  filter(const Aut& aut,
         const std::unordered_set<state_t_of<Aut>>& ss,
         const std::unordered_set<transition_t_of<Aut>>& ts)
  {
    auto rcss = sparse_set(states_size(aut));
    for (auto s : ss)
      rcss.emplace(s);
    auto rcst = sparse_set(transitions_size(aut));
    for (auto t : ts)
      rcst.emplace(t);
    return make_shared_ptr<filter_automaton<Aut, Trans, Exclusion>>(aut, rcss, rcst);
  }

  /// Get an automaton who is a part state set \a ss of \a aut.
  template <Automaton Aut, bool Trans = false, bool Exclusion = false>
  filter_automaton<Aut, Trans, Exclusion>
  filter(const Aut& aut,
         const std::unordered_set<state_t_of<Aut>>& ss
           = std::unordered_set<state_t_of<Aut>>{})
  {
    auto rcss = sparse_set(states_size(aut));
    auto rcst = sparse_set(transitions_size(aut));
    for (auto s : ss)
      {
        rcss.emplace(s);
        if (aut->has_state(s))
        {
          for (auto t : all_out(aut, s))
            rcst.emplace(t);
          for (auto t : all_in(aut, s))
            rcst.emplace(t);
        }
      }
    return make_shared_ptr<filter_automaton<Aut, Trans, Exclusion>>(aut, rcss, rcst);
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
        auto ss = std::unordered_set<state_t_of<Aut>>{};
        // FIXME: this is wrong, of course.
        for (auto s: states)
          ss.emplace(s + 2);
        return ::vcsn::filter(a, ss);
      }
    }
  }
}
