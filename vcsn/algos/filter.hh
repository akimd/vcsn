#pragma once

#include <algorithm> // std::max
#include <boost/range/irange.hpp>
#include <vcsn/algos/copy.hh>
#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/misc/crange.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/misc/unordered_set.hh>
#include <vcsn/misc/vector.hh>

namespace vcsn
{
  namespace detail
  {
    /// Hide some states of an automaton.
    template <Automaton Aut>
    class filter_automaton_impl
      : public automaton_decorator<Aut>
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
      using states_t = std::unordered_set<state_t>;

      using tr_cont_t = std::vector<transition_t>;

      using super_t::pre;
      using super_t::post;
      using super_t::src_of;
      using super_t::dst_of;

      filter_automaton_impl(const automaton_t& input, const states_t& ss)
        : super_t(input), ss_(ss)
      {
        ss_.emplace(input->pre());
        ss_.emplace(input->post());
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
             return (has(ss_, aut_->src_of(t))
                     && has(ss_, aut_->dst_of(t)));
           });
      }

      /// All the outgoing transitions.
      auto all_out(state_t s) const
      {
        return vcsn::detail::all_out(aut_, s,
                                     [this](transition_t t)
                                     {
                                       return has(ss_, aut_->dst_of(t));
                                     });
      }

      /// All the incoming transitions.
      auto all_in(state_t s) const
      {
        return vcsn::detail::all_in(aut_, s,
                                    [this](transition_t t)
                                    {
                                      return has(ss_, aut_->src_of(t));
                                    });
      }

      fresh_automaton_t_of<automaton_t>
      strip() const
      {
        return ::vcsn::copy(aut_, ss_);
      }

    protected:
      /// The "full" automaton whose some states are hidden.
      using super_t::aut_;

    private:
      /// The states we keep.
      states_t ss_;
   };
  }

  template <Automaton Aut>
  using filter_automaton =
    std::shared_ptr<detail::filter_automaton_impl<Aut>>;

  /// Get an automaton who is a part state set \a ss of \a aut.
  template <Automaton Aut>
  filter_automaton<Aut>
  filter(const Aut& aut, const std::unordered_set<state_t_of<Aut>>& ss)
  {
    return make_shared_ptr<filter_automaton<Aut>>(aut, ss);
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
        return make_automaton(::vcsn::filter(a, ss));
      }
    }
  }
}
