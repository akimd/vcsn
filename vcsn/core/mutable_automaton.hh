#ifndef VCSN_CORE_MUTABLE_AUTOMATON_HH
# define VCSN_CORE_MUTABLE_AUTOMATON_HH

# include <vector>

# include <algorithm>
# include <cassert>
# include <list>
# include <boost/range/irange.hpp>

# include "crange.hh"
# include "kind.hh"
# include "transition.hh"
# include "vcsn/weights/poly.hh"
# include "entryiter.hh"

namespace vcsn
{
  template <class GenSet, class WeightSet, class Kind>
  class mutable_automaton
  {
  public:
    typedef GenSet genset_t;
    typedef WeightSet weightset_t;
    typedef Kind kind_t;
    typedef polynomials<GenSet, WeightSet> entryset_t;

    typedef unsigned state_t;
    typedef unsigned transition_t;

    typedef typename label_trait<Kind, GenSet>::label_t label_t;
    typedef typename weightset_t::value_t weight_t;
    typedef typename entryset_t::value_t entry_t;
  protected:
    static const weightset_t& st_ws_;
    const entryset_t es_;

    typedef transition_tuple<state_t, label_t, WeightSet> stored_transition_t;

    typedef std::vector<stored_transition_t> tr_store_t;
    typedef std::vector<transition_t> tr_cont_t;

    struct stored_state_t
    {
      tr_cont_t succ;
      tr_cont_t pred;
    };

    typedef std::vector<stored_state_t> st_store_t;
    typedef std::vector<state_t> st_cont_t;

    typedef std::vector<unsigned> free_store_t;

    st_store_t states_;
    free_store_t states_fs_;
    tr_store_t transitions_;
    free_store_t transitions_fs_;

    label_t prepost_label_;

  public:
    mutable_automaton(const genset_t& gs,
		      const weightset_t& ws)
      : es_(gs, ws), states_(2),
	prepost_label_(gs.template special<label_t>())
    {
    }

    mutable_automaton(const genset_t& gs)
      : mutable_automaton(gs, st_ws_)
    {
    }

    mutable_automaton(mutable_automaton&& that)
      : es_(that.es_)
    {
      std::swap(states_, that.states_);
      std::swap(states_fs_, that.states_fs_);
      std::swap(transitions_, that.transitions_);
      std::swap(transitions_fs_, that.transitions_fs_);
    }

    ~mutable_automaton()
    {
    }

    // Related sets
    ///////////////

    const weightset_t& weightset() const { return es_.weightset(); }
    const genset_t&  genset() const  { return es_.genset(); }
    const entryset_t&  entryset() const  { return es_; }

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
	return weightset().zero();
      else
	return weight_of(t);
    }

    weight_t
    get_final_weight(state_t s) const
    {
      transition_t t = get_transition(s, post(), prepost_label_);
      if (t == null_transition())
	return weightset().zero();
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
	std::find_if(begin(succ), end(succ), [=,&l,&dst] (const transition_t& t)
		     { const stored_transition_t& st = transitions_[t];
		       return st.dst == dst && this->genset().equals(st.label, l); });
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

    state_t src_of(transition_t t) const     { return transitions_[t].src; }
    state_t dst_of(transition_t t) const     { return transitions_[t].dst; }
    label_t label_of(transition_t t) const   { return transitions_[t].label; }

    weight_t weight_of(transition_t t) const
    {
      return transitions_[t].get_weight();
    }

    // Convert the label to a word, in the case of a labels_are_letters.
    // Same as label_of for labels_are_words.
    typename genset_t::word_t
    word_label_of(transition_t t) const
    {
      return genset().to_word(label_of(t));
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
      set_initial(s, weightset().unit());
    }

    weight_t
    add_initial(state_t s, weight_t k)
    {
      return add_transition(pre(), s, prepost_label_, k);
    }

    void
    unset_initial(state_t s)
    {
      set_initial(s, weightset().zero());
    }

    void
    set_final(state_t s, weight_t k)
    {
      set_transition(s, post(), prepost_label_, k);
    }

    void
    set_final(state_t s)
    {
      set_final(s, weightset().unit());
    }

    weight_t
    add_final(state_t s, weight_t k)
    {
      return add_transition(s, post(), prepost_label_, k);
    }

    void
    unset_final(state_t s)
    {
      set_final(s, weightset().zero());
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
      assert(src != pre() || dst != post());
      // It's illegal to leave post()
      assert(src != post());
      // It's illegal to go to post()
      assert(dst != pre());

      transition_t t = get_transition(src, dst, l);
      if (t != null_transition())
	{
	  if (!weightset().is_zero(k))
	    {
	      stored_transition_t& st = transitions_[t];
	      st.label = l;
	      st.set_weight(k);
	    }
	  else
	    {
	      del_transition(t);
	      t = null_transition();
	    }
	}
      else if (!weightset().is_zero(k))
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
	  st.label = l;
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
	  k = weightset().add(weight_of(t), k);
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
      return add_transition(src, dst, l, weightset().unit());
    }

    transition_t
    set_transition(state_t src, state_t dst, label_t l)
    {
      return set_transition(src, dst, l, weightset().unit());
    }

    weight_t
    set_weight(transition_t t, weight_t k)
    {
      if (weightset().is_zero(k))
	del_transition(t);
      else
	transitions_[t].set_weight(k);
      return k;
    }

    weight_t
    add_weight(transition_t t, weight_t k)
    {
      k = weightset().add(weight_of(t), k);
      set_weight(t, k);
      return k;
    }

    // Iteration on states and transitions
    //////////////////////////////////////

    typedef container_filter_range<boost::integer_range<state_t>> states_output_t;
  protected:
    states_output_t
    state_range(state_t b, state_t e) const
    {
      return states_output_t
	(boost::irange<state_t>(b, e), [=] (state_t i) {
	  const stored_state_t& ss = states_[i];
	  return (ss.succ.empty()
		  || ss.succ.front() != this->null_transition());
	});
    }

  public:
    // all states excluding pre()/post()
    states_output_t
    states() const     { return state_range(post() + 1, states_.size()); }

    // all states including pre()/post()
    states_output_t
    all_states() const { return state_range(0U, states_.size()); }

    typedef container_filter_range<boost::integer_range<transition_t>>
      transitions_output_t;

    transitions_output_t
    transitions() const
    {
      return transitions_output_t
	(boost::irange<transition_t>(0U, transitions_.size()),
	 [=] (transition_t i) {
	  state_t src = transitions_[i].src;
	  if (src == this->null_state() || src == this->pre())
	    return false;
	  return transitions_[i].dst != this->post();
	});
    }

    transitions_output_t
    all_transitions() const
    {
      return transitions_output_t
	(boost::irange<transition_t>(0U, transitions_.size()),
	 [=] (transition_t i) {
	  state_t src = transitions_[i].src;
	  return src != this->null_state();
	});
    }

    container_range<tr_cont_t&>
    initial_transitions() const
    {
      return out(pre());
    }

    container_range<st_cont_t&>
    final_transitions() const
    {
      return in(post());
    }

    // Invalidated by del_transition() and del_state().
    container_filter_range<const tr_cont_t&>
    out(state_t s) const
    {
      assert(has_state(s));
      return container_filter_range<const tr_cont_t&>
	(states_[s].succ,
	 [=] (transition_t i) { return transitions_[i].dst != this->post(); });
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
      return container_filter_range<const tr_cont_t&>
	(ss.succ,
	 [=,&l] (transition_t i) {
	  return this->genset().equals(transitions_[i].label, l);
	});
    }

    // Invalidated by del_transition() and del_state().
    container_filter_range<const tr_cont_t&>
    in(state_t s) const
    {
      assert(has_state(s));
      return container_filter_range<const tr_cont_t&>
	(states_[s].succ,
	 [=] (transition_t i) { return transitions_[i].src != this->pre(); });
    }

    // Invalidated by del_transition() and del_state().
    container_range<tr_cont_t&>
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
      return container_filter_range<const tr_cont_t&>
	(ss.pred,
	 [=,&l] (transition_t i) {
	  return this->genset().equals(transitions_[i].label, l);
	});
    }

    // Invalidated by del_transition() and del_state().
    container_filter_range<const tr_cont_t&>
    outin(state_t s, state_t d) const
    {
      assert(has_state(s));
      assert(has_state(d));
      const stored_state_t& ss = states_[s];
      return container_filter_range<const tr_cont_t&>
	(ss.succ,
	 [=] (transition_t i) { return this->transitions_[i].dst == d; });
    }

    // Iteration on entries
    ///////////////////////

    entry_iterator<mutable_automaton, transitions_output_t>
    entries() const
    {
      return
	entry_iterator<mutable_automaton,
		       transitions_output_t>(*this, transitions());
    }

    entry_iterator<mutable_automaton, transitions_output_t>
    all_entries() const
    {
      return
	entry_iterator<mutable_automaton,
		       transitions_output_t>(*this, all_transitions());
    }

    entry_t
    entry_at(state_t s, state_t d) const
    {
      entry_t e;
      for (auto t : outin(s, d))
	es_.assoc(e, word_label_of(t), weight_of(t));
      return e;
    }

    entry_t
    entry_at(transition_t t) const
    {
      return entry_at(src_of(t), dst_of(t));
    }
  };

  template <class GenSet, class WeightSet, class Kind>
  const typename mutable_automaton<GenSet, WeightSet, Kind>::weightset_t&
  mutable_automaton<GenSet, WeightSet, Kind>::st_ws_ = WeightSet();
}

#endif // !VCSN_CORE_MUTABLE_AUTOMATON_HH
