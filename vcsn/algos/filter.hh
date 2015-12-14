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
    template <typename Aut>
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

      // FIXME: clang workaround.
      template <typename Pred>
      struct has_state_p
      {
        bool operator()(state_t s) const
        {
          return pred(s) && has(aut_.ss_, s);
        }
        const self_t& aut_;
        Pred pred;
      };

      template <typename Pred>
      auto all_states(Pred pred) const
      {
        return aut_->all_states(has_state_p<Pred>{*this, pred});
      }

      // FIXME: clang workaround.
      struct all_states_p
      {
        bool operator()(state_t) const { return true; };
      };

      auto all_states() const
      {
        return all_states(all_states_p{});
      }

      // FIXME: clang workaround.
      struct visible_state_p
      {
        bool operator()(state_t s) const
        {
          // When transposing post() < pre().
          return std::max(pre(), post()) < s;
        }
        const self_t& aut_;
      };

      auto states() const
      {
        return all_states(visible_state_p{*this});
      }

      // FIXME: clang workaround.
      template <typename Pred>
      struct has_transition_p
      {
        bool operator()(transition_t t) const
        {
          return (pred(t)
                  && has(aut_.ss_, aut_.src_of(t))
                  && has(aut_.ss_, aut_.dst_of(t)));
        }
        const self_t& aut_;
        Pred pred;
      };

      template <typename Pred>
      auto all_transitions(Pred pred) const
      {
        return aut_->all_transitions(has_transition_p<Pred>{*this, pred});
      }

      // FIXME: clang workaround.
      struct all_transitions_p
      {
        bool operator()(transition_t) const { return true; };
      };

      /// All the transition indexes between all states (including pre
      /// and post).
      auto all_transitions() const
      {
        return all_transitions(all_transitions_p{});
      }

      /// All the transition indexes between visible states.
      auto transitions() const
      {
        auto pred
          = has_transition_p<all_transitions_p>{*this, all_transitions_p{}};
        return all_transitions(pred);
      }

      // FIXME: clang workaround.
      template <typename Pred>
      struct has_dst_p
      {
        bool operator()(transition_t t) const
        {
          return pred(t) && has(aut_.ss_, aut_.dst_of(t));
        }
        const self_t& aut_;
        Pred pred;
      };

      template <typename Pred>
      auto all_out(state_t s, Pred pred) const
      {
        return aut_->all_out(s, has_dst_p<Pred>{*this, pred});
      }

      auto all_out(state_t s) const
      {
        return all_out(s, all_transitions_p{});
      }

      // FIXME: clang workaround.
      struct not_to_post_p
      {
        bool operator()(transition_t t) const
        {
          return aut_.dst_of(t) != aut_.post();
        }
        const self_t& aut_;
      };

      auto out(state_t s) const
      {
        return all_out(s, not_to_post_p{*this});
      }

      // FIXME: clang workaround.
      struct label_equal_p
      {
        bool operator()(transition_t t) const
        {
          return aut_.labelset()->equal(aut_.label_of(t), label_);
        }
        const self_t& aut_;
        label_t label_;
      };

      auto out(state_t s, label_t l) const
      {
        return all_out(s, label_equal_p{*this, l});
      }

      // FIXME: clang workaround.
      template <typename Pred>
      struct has_src_p
      {
        bool operator()(transition_t t) const
        {
          return pred(t) && has(aut_.ss_, aut_.src_of(t));
        }
        const self_t& aut_;
        Pred pred;
      };

      template <typename Pred>
      auto all_in(state_t s, Pred pred) const
      {
        return aut_->all_in(s, has_src_p<Pred>{*this, pred});
      }

      auto all_in(state_t s) const
      {
        return all_in(s, all_transitions_p{});
      }

      // FIXME: clang workaround.
      struct not_from_pre_p
      {
        bool operator()(transition_t t) const
        {
          return aut_.src_of(t) != aut_.pre();
        }
        const self_t& aut_;
      };

      auto in(state_t s) const
      {
        return all_in(s, not_from_pre_p{*this});
      }

      auto in(state_t s, label_t l) const
      {
        return all_in(s, label_equal_p{*this, l});
      }

      fresh_automaton_t_of<automaton_t>
      strip() const
      {
        return ::vcsn::copy(aut_, ss_);
      }

      /// Indexes of transitions to visible initial states.
      auto initial_transitions() const
      {
        return out(pre());
      }

      /// Indexes of transitions from visible final states.
      auto final_transitions() const
      {
        return in(post());
      }

    protected:
      /// The "full" automaton whose some states are hidden.
      using super_t::aut_;

    private:
      /// The states we keep.
      states_t ss_;
   };
  }

  template <typename Aut>
  using filter_automaton =
    std::shared_ptr<detail::filter_automaton_impl<Aut>>;

  /// Get an automaton who is a part state set \a ss of \a aut.
  template <typename Aut>
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
      template <typename Aut, typename Unsigneds>
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
