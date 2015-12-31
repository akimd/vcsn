#pragma once

#include <algorithm>
#include <cassert>
#include <vector>

#include <boost/range/algorithm/find.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/irange.hpp>

#include <vcsn/core/automaton.hh>
#include <vcsn/core/fwd.hh>
#include <vcsn/core/transition.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/misc/crange.hh>
#include <vcsn/misc/format.hh>
#include <vcsn/misc/index.hh>
#include <vcsn/misc/memory.hh>
#include <vcsn/misc/symbol.hh>

namespace vcsn
{
  namespace detail
  {
  /// Bidirectional automata.
  template <typename Context>
  class mutable_automaton_impl
  {
  public:
    using context_t = Context;
    using self_t = mutable_automaton_impl;
    /// The (shared pointer) type to use if we have to create an
    /// automaton of the same (underlying) type.
    template <typename Ctx = Context>
    using fresh_automaton_t = mutable_automaton<Ctx>;
    using labelset_t = labelset_t_of<context_t>;
    using weightset_t = weightset_t_of<context_t>;
    using kind_t = typename context_t::kind_t;

    using labelset_ptr = typename context_t::labelset_ptr;
    using weightset_ptr = typename context_t::weightset_ptr;

    /// Lightweight state handle (or index).
    struct state_tag {};
    using state_t = index_t_impl<state_tag>;
    /// Lightweight transition handle (or index).
    struct transition_tag {};
    using transition_t = index_t_impl<transition_tag>;
    /// Transition label.
    using label_t = typename labelset_t::value_t;
    /// Transition weight.
    using weight_t = typename weightset_t::value_t;

  protected:
    /// The algebraic type of this automaton.
    context_t ctx_;

    /// Data stored per transition.
    using stored_transition_t = transition_tuple<state_t, label_t, weight_t>;

    /// All the automaton's transitions.
    using tr_store_t = std::vector<stored_transition_t>;
    /// All the incoming/outgoing transition handles of a state.
    using tr_cont_t = std::vector<transition_t>;

    /// Data stored for each state.
    struct stored_state_t
    {
      /// Outgoing transitions.
      tr_cont_t succ;
      /// Incoming transitions.
      tr_cont_t pred;
    };

    /// All the automaton's states.
    using st_store_t = std::vector<stored_state_t>;

    /// A list of unused indexes in the states/transitions tables.
    using free_store_t = std::vector<unsigned>;

    st_store_t states_;
    /// Free indexes in states_.
    free_store_t states_fs_;
    tr_store_t transitions_;
    /// Free indexes in transitions_.
    free_store_t transitions_fs_;
    /// Label for initial and final transitions.
    label_t prepost_label_;

  public:
    mutable_automaton_impl() = delete;
    mutable_automaton_impl(const mutable_automaton_impl&) = delete;
    mutable_automaton_impl(const context_t& ctx)
      : ctx_{ctx}
      , states_{2}
      , prepost_label_(ctx.labelset()->special())
    {}

    mutable_automaton_impl(mutable_automaton_impl&& that)
      : ctx_(that.ctx_)
      , prepost_label_(that.prepost_label_)
    {
      *this = std::move(that);
    }

    mutable_automaton_impl& operator=(mutable_automaton_impl&& that)
    {
      if (this != &that)
        {
          ctx_ = std::move(that.ctx_);
          prepost_label_ = std::move(that.prepost_label_);
          std::swap(states_, that.states_);
          std::swap(states_fs_, that.states_fs_);
          std::swap(transitions_, that.transitions_);
          std::swap(transitions_fs_, that.transitions_fs_);
        }
      return *this;
    }


    /*----------------.
    | Related sets.   |
    `----------------*/

    static symbol sname()
    {
      static auto res
        = symbol{"mutable_automaton<" + context_t::sname() + '>'};
      return res;
    }

    std::ostream& print_set(std::ostream& o, format fmt = {}) const
    {
      o << "mutable_automaton<";
      context().print_set(o, fmt);
      return o << '>';
    }

    const context_t& context() const { return ctx_; }
    const weightset_ptr& weightset() const { return ctx_.weightset(); }
    const labelset_ptr& labelset() const { return ctx_.labelset(); }


    /*----------------------------------.
    | Special states and transitions.   |
    `----------------------------------*/

    // pseudo-initial and final states.
    // The code below assumes that pre() and post() are the first
    // two states of the automaton.  In other words, all other states
    // have greater numbers.  We also assume that pre()<post().
    static constexpr state_t      pre()  { return 0U; }
    static constexpr state_t      post()  { return 1U; }
    /// Invalid  state.
    static constexpr state_t      null_state()      { return -1U; }
    /// Invalid transition.
    static constexpr transition_t null_transition() { return -1U; }

    /// Label for preinitial and postfinal transitions.
    label_t prepost_label() const
    {
      return prepost_label_;
    }


    /*--------------.
    | Statistics.   |
    `--------------*/

    size_t num_all_states() const { return states_.size() - states_fs_.size(); }
    size_t num_states() const { return num_all_states() - 2; }
    size_t num_initials() const { return states_[pre()].succ.size(); }
    size_t num_finals() const { return states_[post()].pred.size(); }
    size_t num_transitions() const
    {
      return (transitions_.size()
              - transitions_fs_.size() - num_initials() - num_finals());
    }


    /*---------------------.
    | Queries on states.   |
    `---------------------*/

    /// Whether state s belongs to the automaton.
    bool
    has_state(state_t s) const
    {
      // Any number outside our container is not a state.
      // (This includes "null_state()".)
      if (states_.size() <= s)
        return false;
      else
        {
          const stored_state_t& ss = states_[s];
          // Erased states have 'invalid' as their first successor.
          return ss.succ.empty() || ss.succ.front() != null_transition();
        }
    }

    /// Whether state s belongs to the automaton.
    bool
    state_is_strict(state_t s) const
    {
      assert(has_state(s)); (void) s;
      return true;
    }

    /// Whether s is initial.
    bool
    is_initial(state_t s) const
    {
      return has_transition(pre(), s, prepost_label_);
    }

    /// Whether s is final.
    bool
    is_final(state_t s) const
    {
      return has_transition(s, post(), prepost_label_);
    }

    /// Initial weight of s.
    ATTRIBUTE_PURE
    weight_t
    get_initial_weight(state_t s) const
    {
      transition_t t = get_transition(pre(), s, prepost_label_);
      if (t == null_transition())
        return weightset()->zero();
      else
        return weight_of(t);
    }

    /// Final weight of s.
    ATTRIBUTE_PURE
    weight_t
    get_final_weight(state_t s) const
    {
      transition_t t = get_transition(s, post(), prepost_label_);
      if (t == null_transition())
        return weightset()->zero();
      else
        return weight_of(t);
    }


    /*--------------------------.
    | Queries on transitions.   |
    `--------------------------*/

    transition_t
    get_transition(state_t src, state_t dst, label_t l) const
    {
      assert(has_state(src));
      assert(has_state(dst));
      const tr_cont_t& succ = states_[src].succ;
      const tr_cont_t& pred = states_[dst].pred;
      const auto& ls = *this->labelset();
      if (succ.size() <= pred.size())
        {
          auto i =
            boost::find_if(succ,
                           [this,l,ls,dst] (transition_t t)
                           {
                             return (dst_of(t) == dst
                                     && ls.equal(label_of(t), l));
                           });
          if (i != end(succ))
            return *i;
        }
      else
        {
          auto i =
            boost::find_if(pred,
                           [this,l,ls,src] (transition_t t)
                           {
                             return (src_of(t) == src
                                     && ls.equal(label_of(t), l));
                           });
          if (i != end(pred))
            return *i;
        }
      return null_transition();
    }

    bool
    has_transition(state_t src, state_t dst, label_t l) const
    {
      return get_transition(src, dst, l) != null_transition();
    }

    bool
    has_transition(transition_t t) const
    {
      // Any number outside our container is not a transition.
      // (This includes "null_transition()".)
      if (t >= transitions_.size())
        return false;

      // Erased transition have invalid source state.
      return transitions_[t].src != null_state();
    }

    state_t src_of(transition_t t) const   { return transitions_[t].src; }
    state_t dst_of(transition_t t) const   { return transitions_[t].dst; }
    label_t label_of(transition_t t) const
    {
      return transitions_[t].get_label();
    }

    weight_t weight_of(transition_t t) const
    {
      return transitions_[t].get_weight();
    }


    /*---------------------.
    | Edition of states.   |
    `---------------------*/

  protected:
    /// Remove t from the outgoing transitions of the source state.
    void
    del_transition_from_src(transition_t t)
    {
      stored_transition_t& st = transitions_[t];
      auto& succ = states_[st.src].succ;
      auto tsucc = boost::range::find(succ, t);
      assert(tsucc != succ.end());
      *tsucc = std::move(succ.back());
      succ.pop_back();
    }

    /// Remove t from the ingoing transition of the destination state.
    void
    del_transition_from_dst(transition_t t)
    {
      stored_transition_t& st = transitions_[t];
      auto& pred = states_[st.dst].pred;
      auto tpred = boost::range::find(pred, t);
      assert(tpred != pred.end());
      *tpred = std::move(pred.back());
      pred.pop_back();
    }

    void
    del_transition_container(tr_cont_t& tc, bool from_succ)
    {
      for (auto t: tc)
        {
          if (from_succ)
            del_transition_from_dst(t);
          else
            del_transition_from_src(t);
          transitions_[t].src = null_state();
        }
      transitions_fs_.insert(transitions_fs_.end(), tc.begin(), tc.end());
      tc.clear();
    }

  public:
    /// Create a new state and return its id.
    state_t
    new_state()
    {
      state_t res;
      if (states_fs_.empty())
        {
          res = states_.size();
          states_.emplace_back();
        }
      else
        {
          res = states_fs_.back();
          states_fs_.pop_back();
          stored_state_t& ss = states_[res];
          // De-invalidate this state: remove the invalid transition.
          ss.succ.clear();
        }
      return res;
    }

    std::ostream&
    print_state(state_t s, std::ostream& o) const
    {
      if (s == pre())
        o << "pre";
      else if (s == post())
        o << "post";
      else
        o << s - 2;
      return o;
    }

    std::ostream&
    print_state_name(state_t s, std::ostream& o,
                     format = {},
                     bool = false) const
    {
      return print_state(s, o);
    }

    static constexpr bool
    state_has_name(state_t)
    {
      return false;
    }

    void
    del_state(state_t s)
    {
      assert(has_state(s));
      assert(s > post()); // Cannot erase pre() and post().
      stored_state_t& ss = states_[s];
      del_transition_container(ss.pred, false);
      del_transition_container(ss.succ, true);
      ss.succ.emplace_back(null_transition()); // So has_state() can work.
      states_fs_.emplace_back(s);
    }

    void
    set_initial(state_t s, weight_t w)
    {
      set_transition(pre(), s, prepost_label_, w);
    }

    void
    set_initial(state_t s)
    {
      set_initial(s, weightset()->one());
    }

    transition_t
    add_initial(state_t s, weight_t w)
    {
      return add_transition(pre(), s, prepost_label_, w);
    }

    void
    unset_initial(state_t s)
    {
      set_initial(s, weightset()->zero());
    }

    void
    set_final(state_t s, weight_t w)
    {
      set_transition(s, post(), prepost_label_, w);
    }

    void
    set_final(state_t s)
    {
      set_final(s, weightset()->one());
    }

    transition_t
    add_final(state_t s, weight_t w)
    {
      return add_transition(s, post(), prepost_label_, w);
    }

    void
    unset_final(state_t s)
    {
      set_final(s, weightset()->zero());
    }


    /*--------------------------.
    | Edition of transitions.   |
    `--------------------------*/

    void
    del_transition(transition_t t)
    {
      assert(has_transition(t));
      // Remove transition from source and destination.
      del_transition_from_src(t);
      del_transition_from_dst(t);
      // Actually erase the transition.
      transitions_[t].src = null_state();
      transitions_fs_.emplace_back(t);
    }

    /// Remove the transition (src, l, dst).
    void
    del_transition(state_t src, state_t dst, label_t l)
    {
      transition_t t = get_transition(src, dst, l);
      if (t != null_transition())
        del_transition(t);
    }

    /// Create a transition between two states.  There must not exist
    /// a previous transition with same (src, dst, l).
    ///
    /// \param src  source state
    /// \param dst  destination state
    /// \param l    label of the transition
    /// \param w    weight of the transition
    ///
    /// \pre the label is _not checked_, for efficiency.
    /// \pre ! has_transition(src, dst, l).
    transition_t
    new_transition(state_t src, state_t dst, label_t l, weight_t w)
    {
      assert(!has_transition(src, dst, l));
      if (weightset()->is_zero(w))
        return null_transition();
      else
        {
          // FIXME: When src == pre() || dst == post(), label must be empty.
          transition_t t;
          if (transitions_fs_.empty())
            {
              t = transitions_.size();
              transitions_.emplace_back(src, dst, l, w);
            }
          else
            {
              t = transitions_fs_.back();
              transitions_fs_.pop_back();
              stored_transition_t& st = transitions_[t];
              st.src = src;
              st.dst = dst;
              st.set_label(l);
              st.set_weight(w);
            }
          states_[src].succ.emplace_back(t);
          states_[dst].pred.emplace_back(t);
          return t;
        }
    }

    /// Copy the label of a transition between two states, creating a new
    /// transition.
    /// There must not exist a previous transition with same (src, dst,
    /// label_of(l)).
    ///
    /// \param src  source state
    /// \param dst  destination state
    /// \param aut  the automaton whose transition will be copied
    /// \param t    transition of \a aut whose label and weight are to copy
    /// \param transpose   whether label and weight should be transposed
    ///
    /// \pre ! has_transition(src, dst, label_of(l)).
    template <typename A>
    transition_t
    new_transition_copy(state_t src, state_t dst,
                        const A& aut, typename A::element_type::transition_t t,
                        bool transpose = false)
    {
      auto l = aut->label_of(t);
      auto w = aut->weight_of(t);
      if (transpose)
        {
          l = aut->labelset()->transpose(l);
          w = aut->weightset()->transpose(w);
        }
      return new_transition(src, dst,
                            labelset()->conv(*aut->labelset(), l),
                            weightset()->conv(*aut->weightset(), w));
    }

    /// Same as above, with weight one.
    transition_t
    new_transition(state_t src, state_t dst, label_t l)
    {
      return new_transition(src, dst, l, weightset()->one());
    }

    /// Set a transition between two states.  Override any possible
    /// existing transition with same states and label.
    ///
    /// \param src  source state
    /// \param dst  destination state
    /// \param l    label of the transition
    /// \param w    weight of the transition
    ///
    /// \pre the label is _not checked_, for efficiency.  Letters out
    /// of the alphabet will be accepted.
    transition_t
    set_transition(state_t src, state_t dst, label_t l, weight_t w)
    {
      // It's illegal to connect pre() to post().
      // FIXME: reenable except for labels_are_one.
      // assert(src != pre() || dst != post());
      // It's illegal to leave post().
      assert(src != post());
      // It's illegal to go to pre().
      assert(dst != pre());

      transition_t t = get_transition(src, dst, l);
      if (t == null_transition())
        t = new_transition(src, dst, l, w);
      else if (weightset()->is_zero(w))
        {
          del_transition(t);
          t = null_transition();
        }
      else
        {
          stored_transition_t& st = transitions_[t];
          st.set_label(l);
          st.set_weight(w);
        }
      return t;
    }

    /// Same as above, with unit weight.
    transition_t
    set_transition(state_t src, state_t dst, label_t l)
    {
      return set_transition(src, dst, l, weightset()->one());
    }

    /// Add a transition between two states.  Merge with an existing
    /// one with same label.
    ///
    /// \param src  source state
    /// \param dst  destination state
    /// \param l    label of the transition
    /// \param w    weight of the transition
    ///
    /// \pre the label is _not checked_, for efficiency.  Letters out
    /// of the alphabet will be accepted.
    transition_t
    add_transition(state_t src, state_t dst, label_t l, weight_t w)
    {
      transition_t t = get_transition(src, dst, l);
      if (t == null_transition())
        t = new_transition(src, dst, l, w);
      else
        {
          w = weightset()->add(weight_of(t), w);
          t = set_weight(t, w);
        }
      return t;
    }

    /// Same as above, with weight one.
    transition_t
    add_transition(state_t src, state_t dst, label_t l)
    {
      return add_transition(src, dst, l, weightset()->one());
    }

    /// Add a transition between two states, copying the label from the given
    /// transition.  Merge with an existing one with same label.
    ///
    /// \param src  source state
    /// \param dst  destination state
    /// \param aut  the automaton whose transition will be copied.
    /// \param t    transition of \a aut whose label and weight are to copy
    /// \param transpose   whether label and weight should be transposed
    template <typename A>
    transition_t
    add_transition_copy(state_t src, state_t dst,
                        const A& aut, typename A::element_type::transition_t t,
                        bool transpose = false)
    {
      auto l = aut->label_of(t);
      auto w = aut->weight_of(t);
      if (transpose)
        {
          l = aut->labelset()->transpose(l);
          w = aut->weightset()->transpose(w);
        }
      return add_transition(src, dst,
                            labelset()->conv(*aut->labelset(), l),
                            weightset()->conv(*aut->weightset(), w));
    }

    /// Print a transition, for debugging.
    std::ostream& print(transition_t t, std::ostream& o) const
    {
      print_state_name(src_of(t), o) << " -- <";
      weightset()->print(weight_of(t), o) << '>';
      labelset()->print(label_of(t), o) << " --> ";
      print_state_name(dst_of(t), o);
      return o;
    }

    transition_t
    set_weight(transition_t t, weight_t w)
    {
      if (weightset()->is_zero(w))
        {
          del_transition(t);
          return null_transition();
        }
      else
        transitions_[t].set_weight(w);
      return t;
    }

    weight_t
    add_weight(transition_t t, weight_t w)
    {
      return weight_of(set_weight(t, weightset()->add(weight_of(t), w)));
    }

    weight_t
    lmul_weight(transition_t t, weight_t w)
    {
      return weight_of(set_weight(t, weightset()->mul(w, weight_of(t))));
    }

    weight_t
    rmul_weight(transition_t t, weight_t w)
    {
      return weight_of(set_weight(t, weightset()->mul(weight_of(t), w)));
    }


    /*---------------------------------------.
    | Iteration on states and transitions.   |
    `---------------------------------------*/

  protected:
    /// The range of index numbers in [b .. e] that validate the
    /// predicate \a pred.
    template <typename Pred>
    auto state_range(state_t b, state_t e, Pred pred) const
    {
      return make_container_filter_range
        (boost::irange<state_t>(b, e),
         [this, pred]
         (state_t s)
         {
           const stored_state_t& ss = states_[s];
           return ((ss.succ.empty()
                    || ss.succ.front() != null_transition())
                   && pred(s));
         });
    }

    /// The range of state numbers in [b .. e].
    auto state_range(state_t b, state_t e) const
    {
      return state_range(b, e, [](state_t) { return true; });
    }

  public:
    /// All states including pre()/post().
    /// Guaranteed in increasing order.
    auto all_states() const
    {
      return state_range(0U, states_.size());
    }

    /// All states including pre()/post() that validate \a pred.
    /// Guaranteed in increasing order.
    template <typename Pred>
    auto all_states(Pred pred) const
    {
      return state_range(0U, states_.size(), pred);
    }

    /// All states excluding pre()/post().
    /// Guaranteed in increasing order.
    auto states() const
    {
      return state_range(post() + 1, states_.size());
    }

    /// All the transition indexes between all states (including pre and post).
    auto all_transitions() const
    {
      return make_container_filter_range
        (boost::irange<transition_t>(0U, transitions_.size()),
         [this](transition_t t)
         {
           return src_of(t) != null_state();
         });
    }

    /// Indexes of all transitions leaving state \a s.
    /// Invalidated by del_transition() and del_state().
    container_range<const tr_cont_t&>
    all_out(state_t s) const
    {
      assert(has_state(s));
      return states_[s].succ;
    }

    /// Indexes of all transitions arriving to state \a s.
    /// Invalidated by del_transition() and del_state().
    container_range<const tr_cont_t&>
    all_in(state_t s) const
    {
      assert(has_state(s));
      return states_[s].pred;
    }
  };
  }

  template <typename Context>
  mutable_automaton<Context>
  make_mutable_automaton(const Context& ctx)
  {
    return make_shared_ptr<mutable_automaton<Context>>(ctx);
  }

  template <typename Context>
  auto
  make_nullable_automaton(const Context& ctx)
  {
    return make_mutable_automaton(make_nullableset_context(ctx));
  }
}
