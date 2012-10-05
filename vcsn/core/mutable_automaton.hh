#ifndef VCSN_CORE_MUTABLE_AUTOMATON_HH
# define VCSN_CORE_MUTABLE_AUTOMATON_HH

# include <algorithm>
# include <cassert>
# include <list>
# include <vector>

# include <boost/range/irange.hpp>

# include <vcsn/core/crange.hh>
# include <vcsn/core/entryiter.hh>
# include <vcsn/core/kind.hh>
# include <vcsn/core/transition.hh>
# include <vcsn/ctx/ctx.hh>
# include <vcsn/weights/poly.hh>

namespace vcsn
{
  class abstract_mutable_automaton
  {
  public:
    virtual const ctx::abstract_context& abstract_context() const = 0;
    virtual ~abstract_mutable_automaton() {}
  };

  template <typename Context>
  class mutable_automaton: public abstract_mutable_automaton
  {
  public:
    using context_t = Context;
    using genset_t = typename context_t::genset_t;
    using weightset_t = typename context_t::weightset_t;
    using kind_t = typename context_t::kind_t;

    using genset_ptr = typename context_t::genset_ptr;
    using weightset_ptr = typename context_t::weightset_ptr;
    using entryset_t = polynomialset<context_t>;

    using state_t = unsigned;
    using transition_t = unsigned;

    using label_t = typename context_t::label_t;
    using weight_t = typename weightset_t::value_t;
    using entry_t = typename entryset_t::value_t;

  protected:
    const entryset_t es_;

    using stored_transition_t = transition_tuple<state_t, label_t, weight_t>;

    using tr_store_t = std::vector<stored_transition_t>;
    using tr_cont_t = std::vector<transition_t>;

    struct stored_state_t
    {
      tr_cont_t succ;
      tr_cont_t pred;
    };

    using st_store_t = std::vector<stored_state_t>;
    using st_cont_t = std::vector<state_t>;

    using free_store_t = std::vector<unsigned>;

    st_store_t states_;
    free_store_t states_fs_;
    tr_store_t transitions_;
    free_store_t transitions_fs_;
    label_t prepost_label_;

  public:
    mutable_automaton(const context_t& ctx)
      : es_{ctx}
      , states_{2}
      , prepost_label_(ctx.genset()->template special<label_t>())
    {
    }

    mutable_automaton(mutable_automaton&& that)
      : es_{that.es_}
      , prepost_label_(that.prepost_label_)
    {
      std::swap(states_, that.states_);
      std::swap(states_fs_, that.states_fs_);
      std::swap(transitions_, that.transitions_);
      std::swap(transitions_fs_, that.transitions_fs_);
    }

    // Related sets
    ///////////////

    const context_t& context() const { return es_.context(); }
    virtual const ctx::abstract_context& abstract_context() const
    {
      return context();
    }
    const weightset_ptr& weightset() const { return es_.weightset(); }
    const genset_ptr& genset() const { return es_.genset(); }
    const entryset_t& entryset() const { return es_; }

    // Special states and transitions
    /////////////////////////////////

    // pseudo-initial and final states.
    // The code below assumes that pre() and post() are the first
    // two states of the automaton.  In other words, all other states
    // have greater numbers.  We also assume that pre()<post().
    static constexpr state_t      pre()  { return 0U; }
    static constexpr state_t      post()  { return 1U; }
    // Invalid transition or state.
    static constexpr state_t      null_state()      { return -1U; }
    static constexpr transition_t null_transition() { return -1U; }

    label_t prepost_label() const
    {
      return prepost_label_;
    }

    // Statistics
    /////////////

    size_t num_states() const { return states_.size() - states_fs_.size() - 2; }
    size_t num_initials() const { return states_[pre()].succ.size(); }
    size_t num_finals() const { return states_[post()].pred.size(); }
    size_t num_transitions() const
    {
      return (transitions_.size()
              - transitions_fs_.size() - num_initials() - num_finals());
    }

    // Queries on states
    ////////////////////

    bool
    has_state(state_t s) const
    {
      // Any number outside our container is not a state.
      // (This includes "null_state()".)
      if (s >= states_.size())
        return false;
      const stored_state_t& ss = states_[s];

      // Erased states have 'invalid' has their first successor.
      return ss.succ.empty() || ss.succ.front() != null_transition();
    }

    bool
    is_initial(state_t s) const
    {
      return has_transition(pre(), s, prepost_label_);
    }

    bool
    is_final(state_t s) const
    {
      return has_transition(s, post(), prepost_label_);
    }

    weight_t
    get_initial_weight(state_t s) const
    {
      transition_t t = get_transition(pre(), s, prepost_label_);
      if (t == null_transition())
        return weightset()->zero();
      else
        return weight_of(t);
    }

    weight_t
    get_final_weight(state_t s) const
    {
      transition_t t = get_transition(s, post(), prepost_label_);
      if (t == null_transition())
        return weightset()->zero();
      else
        return weight_of(t);
    }

    // Queries on transitions
    /////////////////////////

    transition_t
    get_transition(state_t src, state_t dst, label_t l) const
    {
      assert(has_state(src));
      assert(has_state(dst));
      // FIXME: We should search in dst->pred if it is smaller
      // than src->succ.
      const tr_cont_t& succ = states_[src].succ;
      auto i =
        std::find_if(begin(succ), end(succ),
                     [this,l,dst] (const transition_t& t) -> bool
                     {
                       const stored_transition_t& st = transitions_[t];
                       return (st.dst == dst
                               && this->genset()->equals(st.get_label(), l));
                     });
      if (i == end(succ))
        return null_transition();
      return *i;
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

    // Convert the label to a word, in the case of a labels_are_letters.
    // Same as label_of for labels_are_words.
    typename genset_t::word_t
    word_label_of(transition_t t) const
    {
      return genset()->to_word(label_of(t));
    }

    // Edition of states
    ////////////////////

  protected:
    // Remove t from the outgoing transitions of the source state.
    void
    del_transition_from_src(transition_t t)
    {
      stored_transition_t& st = transitions_[t];
      auto& succ = states_[st.src].succ;
      auto tsucc = std::find(succ.begin(), succ.end(), t);
      assert(tsucc != succ.end());
      *tsucc = succ.back();
      succ.pop_back();
    }

    // Remove t from the ingoing transition of the destination state.
    void
    del_transition_from_dst(transition_t t)
    {
      stored_transition_t& st = transitions_[t];
      auto& pred = states_[st.dst].pred;
      auto tpred = std::find(pred.begin(), pred.end(), t);
      assert(tpred != pred.end());
      *tpred = pred.back();
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
    state_t
    new_state()
    {
      state_t s;
      if (states_fs_.empty())
        {
          s = states_.size();
          states_.resize(s + 1);
        }
      else
        {
          s = states_fs_.back();
          states_fs_.pop_back();
        }
      stored_state_t& ss = states_[s];
      // De-invalidate this state: remove the invalid transition.
      ss.succ.clear();
      return s;
    }

    void
    del_state(state_t s)
    {
      assert(has_state(s));
      assert(s > post()); // Cannot erase pre() and post().
      stored_state_t& ss = states_[s];
      del_transition_container(ss.pred, false);
      del_transition_container(ss.succ, true);
      ss.succ.push_back(null_transition()); // So has_state() can work.
      states_fs_.push_back(s);
    }

    void
    set_initial(state_t s, weight_t k)
    {
      set_transition(pre(), s, prepost_label_, k);
    }

    void
    set_initial(state_t s)
    {
      set_initial(s, weightset()->unit());
    }

    weight_t
    add_initial(state_t s, weight_t k)
    {
      return add_transition(pre(), s, prepost_label_, k);
    }

    void
    unset_initial(state_t s)
    {
      set_initial(s, weightset()->zero());
    }

    void
    set_final(state_t s, weight_t k)
    {
      set_transition(s, post(), prepost_label_, k);
    }

    void
    set_final(state_t s)
    {
      set_final(s, weightset()->unit());
    }

    weight_t
    add_final(state_t s, weight_t k)
    {
      return add_transition(s, post(), prepost_label_, k);
    }

    void
    unset_final(state_t s)
    {
      set_final(s, weightset()->zero());
    }

    // Edition of transitions
    /////////////////////////

    void
    del_transition(transition_t t)
    {
      assert(has_transition(t));
      // Remove transition from source and destination.
      del_transition_from_src(t);
      del_transition_from_dst(t);
      // Actually erase the transition.
      transitions_[t].src = null_state();
      transitions_fs_.push_back(t);
    }

    void
    del_transition(state_t src, state_t dst, label_t l)
    {
      transition_t t = get_transition(src, dst, l);
      if (t != null_transition())
        del_transition(t);
    }

    transition_t
    set_transition(state_t src, state_t dst, label_t l, weight_t k)
    {
      // It's illegal to connect pre() to post().
      // FIXME: reenable except for labels_are_empty.
      // assert(src != pre() || dst != post());
      // It's illegal to leave post().
      assert(src != post());
      // It's illegal to go to pre().
      assert(dst != pre());

      transition_t t = get_transition(src, dst, l);
      if (t != null_transition())
        {
          if (!weightset()->is_zero(k))
            {
              stored_transition_t& st = transitions_[t];
              st.set_label(l);
              st.set_weight(k);
            }
          else
            {
              del_transition(t);
              t = null_transition();
            }
        }
      else if (!weightset()->is_zero(k))
        {
          if (transitions_fs_.empty())
            {
              t = transitions_.size();
              transitions_.resize(t + 1);
            }
          else
            {
              t = transitions_fs_.back();
              transitions_fs_.pop_back();
            }
          stored_transition_t& st = transitions_[t];
          st.src = src;
          st.dst = dst;
          st.set_label(l);
          st.set_weight(k);
          states_[src].succ.push_back(t);
          states_[dst].pred.push_back(t);
        }
      return t;
    }

    weight_t
    add_transition(state_t src, state_t dst, label_t l, weight_t k)
    {
      transition_t t = get_transition(src, dst, l);
      if (t != null_transition())
        {
          k = weightset()->add(weight_of(t), k);
          set_weight(t, k);
        }
      else
        {
          set_transition(src, dst, l, k);
        }
      return k;
    }

    weight_t
    add_transition(state_t src, state_t dst, label_t l)
    {
      return add_transition(src, dst, l, weightset()->unit());
    }

    transition_t
    set_transition(state_t src, state_t dst, label_t l)
    {
      return set_transition(src, dst, l, weightset()->unit());
    }

    std::string
    format_transition(transition_t t) const
    {
      std::ostringstream o;
      o << src_of(t)
        << " -- {" << weightset()->format(weight_of(t)) << "}"
        << genset()->format(word_label_of(t))
        << " --> " << dst_of(t);
      return o.str();
    }

    weight_t
    set_weight(transition_t t, weight_t k)
    {
      if (weightset()->is_zero(k))
        del_transition(t);
      else
        transitions_[t].set_weight(k);
      return k;
    }

    weight_t
    add_weight(transition_t t, weight_t k)
    {
      return set_weight(t, weightset()->add(weight_of(t), k));
    }

    weight_t
    lmul_weight(transition_t t, weight_t k)
    {
      return set_weight(t, weightset()->mul(k, weight_of(t)));
    }

    weight_t
    rmul_weight(transition_t t, weight_t k)
    {
      return set_weight(t, weightset()->mul(weight_of(t), k));
    }

    // Iteration on states and transitions
    //////////////////////////////////////

    using states_output_t =
      container_filter_range<boost::integer_range<state_t>>;

  protected:
    states_output_t
    state_range(state_t b, state_t e) const
    {
      return
        {boost::irange<state_t>(b, e),
         [this] (state_t i) -> bool
         {
           const stored_state_t& ss = states_[i];
           return (ss.succ.empty()
                   || ss.succ.front() != this->null_transition());
         }};
    }

  public:
    // all states excluding pre()/post()
    states_output_t
    states() const     { return state_range(post() + 1, states_.size()); }

    // all states including pre()/post()
    states_output_t
    all_states() const { return state_range(0U, states_.size()); }

    using transitions_output_t =
      container_filter_range<boost::integer_range<transition_t>>;

    transitions_output_t
    transitions() const
    {
      return
        {
          boost::irange<transition_t>(0U, transitions_.size()),
          [this] (transition_t i) -> bool
            {
              state_t src = transitions_[i].src;
              if (src == this->null_state() || src == this->pre())
                return false;
              return transitions_[i].dst != this->post();
            }
        };
    }

    transitions_output_t
    all_transitions() const
    {
      return
        {
          boost::irange<transition_t>(0U, transitions_.size()),
          [this] (transition_t i)
            {
              return transitions_[i].src != this->null_state();
            }
        };
    }

    container_filter_range<const tr_cont_t&>
    initial_transitions() const
    {
      return out(pre());
    }

    container_filter_range<const tr_cont_t&>
    final_transitions() const
    {
      return in(post());
    }

    // Invalidated by del_transition() and del_state().
    container_filter_range<const tr_cont_t&>
    out(state_t s) const
    {
      assert(has_state(s));
      return {states_[s].succ,
              [this] (transition_t i)
                { return transitions_[i].dst != this->post(); }};
    }

    // Invalidated by del_transition() and del_state().
    container_range<const tr_cont_t&>
    all_out(state_t s) const
    {
      assert(has_state(s));
      return states_[s].succ;
    }

    // Invalidated by del_transition() and del_state().
    container_filter_range<const tr_cont_t&>
    out(state_t s, const label_t& l) const
    {
      assert(has_state(s));
      const stored_state_t& ss = states_[s];
      return {ss.succ,
              [this,l] (transition_t i) {
                return this->genset()->equals(transitions_[i].get_label(), l);
            }};
    }

    // Invalidated by del_transition() and del_state().
    container_filter_range<const tr_cont_t&>
    in(state_t s) const
    {
      assert(has_state(s));
      return {states_[s].pred,
              [this] (transition_t i)
                { return transitions_[i].src != this->pre(); }};
    }

    // Invalidated by del_transition() and del_state().
    container_range<const tr_cont_t&>
    all_in(state_t s) const
    {
      assert(has_state(s));
      return states_[s].pred;
    }

    // Invalidated by del_transition() and del_state().
    container_filter_range<const tr_cont_t&>
    in(state_t s, const label_t& l) const
    {
      assert(has_state(s));
      const stored_state_t& ss = states_[s];
      return {ss.pred,
              [this,l] (transition_t i) {
                return this->genset()->equals(transitions_[i].get_label(), l);
        }};
    }

    // Invalidated by del_transition() and del_state().
    container_filter_range<const tr_cont_t&>
    outin(state_t s, state_t d) const
    {
      assert(has_state(s));
      assert(has_state(d));
      const stored_state_t& ss = states_[s];
      return {ss.succ,
              [this,d] (transition_t i)
                { return this->transitions_[i].dst == d; }};
    }



    /*----------.
    | entries.  |
    `----------*/

    entry_iterator<mutable_automaton, transitions_output_t>
    entries() const
    {
      return { *this, transitions() };
    }

    entry_iterator<mutable_automaton, transitions_output_t>
    all_entries() const
    {
      return { *this, all_transitions() };
    }

    entry_t
    entry_at(state_t s, state_t d) const
    {
      entry_t res;
      for (auto t : outin(s, d))
        es_.assoc(res, word_label_of(t), weight_of(t));
      return res;
    }

    entry_t
    entry_at(transition_t t) const
    {
      return entry_at(src_of(t), dst_of(t));
    }

    void
    add_entry(state_t src, state_t dst, const entry_t& es)
    {
      add_entry_<context_t>(src, dst, es);
    }

    template <typename Ctx>
    void
    add_entry_(state_t src, state_t dst,
               if_lae<Ctx, const entry_t&> es)
    {
      for (auto e: es)
        add_transition(src, dst, {}, e.second);
    }

    template <typename Ctx>
    void
    add_entry_(state_t src, state_t dst,
               if_lal<Ctx, const entry_t&> es)
    {
      for (auto e: es)
        // FIXME: Hack.  entries are always about words, but we
        // want letters.  "e.first[0]" is a hack for lal.
        add_transition(src, dst, e.first[0], e.second);
    }

    template <typename Ctx>
    void
    add_entry_(state_t src, state_t dst,
              if_law<Ctx, const entry_t&> es)
    {
      for (auto e: es)
        add_transition(src, dst, e.first, e.second);
    }

    // Remove all the transitions between s and d.
    void
    del_entry(state_t s, state_t d)
    {
      // FIXME: Beware that according to the comments of outin,
      // del_transition invalidates its result.
      for (auto t : outin(s, d))
        del_transition(t);
    }
  };

  template <typename Context>
  mutable_automaton<Context>
  make_mutable_automaton(const Context& ctx)
  {
    return {ctx};
  }
}

#endif // !VCSN_CORE_MUTABLE_AUTOMATON_HH
