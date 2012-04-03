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

namespace vcsn
{
  // Invalid transition or state.
  constexpr unsigned invalid = -1U;

  template <class Alphabet, class WeightSet, class Kind>
  class mutable_automaton
  {
  public:
    typedef Alphabet alphabet_t;
    typedef WeightSet weightset_t;
    typedef typename weightset_t::value_t weight_t;
    typedef typename label_trait<Kind, Alphabet>::label_t label_t;

    typedef unsigned state_t;
    typedef unsigned transition_t;
  protected:
    const alphabet_t& a_;
    const weightset_t& ws_;
    static const weightset_t& st_ws_;

    typedef transition_tuple<state_t, label_t, WeightSet> stored_transition_t;

    typedef std::vector<stored_transition_t> tr_store_t;
    typedef std::vector<transition_t> tr_cont_t;

    struct stored_state_t
    {
      weight_t initial;
      weight_t final;
      tr_cont_t succ;
      tr_cont_t pred;
    };

    typedef std::vector<stored_state_t> st_store_t;
    typedef std::vector<state_t> st_cont_t;

    typedef std::vector<unsigned> free_store_t;

    st_cont_t initials_;
    st_cont_t finals_;
    st_store_t states_;
    free_store_t states_fs_;
    tr_store_t transitions_;
    free_store_t transitions_fs_;

  public:

    mutable_automaton(const alphabet_t& a)
      : a_(a), ws_(st_ws_)
    {
    }

    mutable_automaton(const alphabet_t& a,
		      const weightset_t& ws)
      : a_(a), ws_(ws)
    {
    }

    mutable_automaton(mutable_automaton&& that)
      : a_(that.a_), ws_(that.ws_)
    {
      std::swap(initials_, that.initials_);
      std::swap(finals_, that.finals_);
      std::swap(states_, that.states_);
      std::swap(transitions_, that.transitions_);
    }

    ~mutable_automaton()
    {
    }

    const weightset_t&
    weightset() const
    {
      return ws_;
    }

    const alphabet_t&
    alphabet() const
    {
      return a_;
    }

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
      ss.final = ss.initial = ws_.zero();
      return s;
    }

    bool
    has_state(state_t s) const
    {
      // Any number outside our container is not a state.
      // (This includes "invalid".)
      if (s >= states_.size())
	return false;
      const stored_state_t& ss = states_[s];

      // Erased states have 'invalid' has their first successor.
      return ss.succ.empty() || ss.succ.front() != invalid;
    }


  protected:
    void
    del_transitions(tr_cont_t& tc)
    {
      transitions_fs_.insert(transitions_fs_.end(), tc.begin(), tc.end());
      tc.clear();
    }

  public:
    void
    del_state(state_t s)
    {
      assert(has_state(s));
      stored_state_t& ss = states_[s];
      del_transitions(ss.succ);
      del_transitions(ss.pred);
      ss.succ.push_back(invalid); // So has_state() can work.
      states_fs_.push_back(s);
    }

    const weight_t&
    get_initial_weight(state_t s) const
    {
      return states_[s].initial;
    }

    const weight_t&
    get_final_weight(state_t s) const
    {
      return states_[s].final;
    }

    void
    set_initial(state_t s, weight_t k)
    {
      assert(has_state(s));
      stored_state_t& ss = states_[s];

      weight_t oldk = ss.initial;
      if (oldk == k)
	return;

      ss.initial = k;
      if (ws_.is_zero(k))
	{
	  st_cont_t::iterator pos = std::find(begin(initials_),
					      end(initials_), s);
	  // pos must be here because the old weight was nonzero.
	  assert(pos != end(initials_));

	  // Remove the state from the initials_ vector.  Since this
	  // vector is not ordered, we can just fill the hole with the
	  // last value.
	  *pos = initials_.back();
	  initials_.pop_back();
	}
      else if (ws_.is_zero(oldk))
	{
	  initials_.push_back(s);
	}
    }

    void
    set_initial(state_t s)
    {
      set_initial(s, ws_.unit());
    }

    weight_t
    add_initial(state_t s, weight_t k)
    {
      k = ws_.add(get_initial_weight(s), k);
      set_initial(s, k);
      return k;
    }

    void
    unset_initial(state_t s)
    {
      set_initial(s, ws_.zero());
    }

    bool
    is_initial(state_t s) const
    {
      return !ws_.is_zero(get_initial_weight(s));
    }

    void
    set_final(state_t s, weight_t k)
    {
      assert(has_state(s));
      stored_state_t& ss = states_[s];

      weight_t oldk = ss.final;
      if (oldk == k)
	return;

      ss.final = k;
      if (ws_.is_zero(k))
	{
	  st_cont_t::iterator pos = std::find(begin(finals_),
					      end(finals_), s);
	  // pos must be here because the old weight was nonzero.
	  assert(pos != end(finals_));

	  // Remove the state from the finals_ vector.  Since this
	  // vector is not ordered, we can just fill the hole with the
	  // last value.
	  *pos = finals_.back();
	  finals_.pop_back();
	}
      else if (ws_.is_zero(oldk))
	{
	  finals_.push_back(s);
	}
    }

    void
    set_final(state_t s)
    {
      set_final(s, ws_.unit());
    }

    weight_t
    add_final(state_t s, weight_t k)
    {
      k = ws_.add(get_final_weight(s), k);
      set_final(s, k);
      return k;
    }

    void
    unset_final(state_t s)
    {
      set_final(s, ws_.zero());
    }

    bool
    is_final(state_t s) const
    {
      return !ws_.is_zero(get_final_weight(s));
    }

    size_t nb_states() const { return states_.size(); }
    size_t nb_transitions() const { return transitions_.size(); }
    size_t nb_initials() const { return initials_.size(); }
    size_t nb_finals() const { return finals_.size(); }

    transition_t
    get_transition(state_t src, state_t dst, label_t l) const
    {
      assert(has_state(src));
      assert(has_state(dst));
      // FIXME: We should search in dst->pred if it is smaller
      // than src->succ.
      const tr_cont_t& succ = states_[src].succ;
      auto i =
	std::find_if(begin(succ), end(succ), [&] (const transition_t& t)
		     { const stored_transition_t& st = transitions_[t];
		       return st.dst == dst && a_.equals(st.label, l); });
      if (i == end(succ))
	return invalid;
      return *i;
    }

    bool
    has_transition(transition_t t) const
    {
      // Any number outside our container is not a transition.
      // (This includes "invalid".)
      if (t >= transitions_.size())
	return false;

      // Erased transition have invalid source state.
      return transitions_[t].src != invalid;
    }

    bool
    has_transition(state_t src, state_t dst, label_t l) const
    {
      return get_transition(src, dst, l) != invalid;
    }

    state_t src_of(transition_t t) const     { return transitions_[t].src; }
    state_t dst_of(transition_t t) const     { return transitions_[t].dst; }
    label_t label_of(transition_t t) const   { return transitions_[t].label; }
    weight_t weight_of(transition_t t) const
    {
      return transitions_[t].get_weight();
    }

    void
    del_transition(transition_t t)
    {
      assert(has_transition(t));
      stored_transition_t& st = transitions_[t];

      // Remove transition from source and destination.
      auto& succ = states_[st.src].succ;
      auto tsucc = std::find(succ.begin(), succ.end(), t);
      assert(tsucc != succ.end());
      *tsucc = succ.back();
      succ.pop_back();

      auto& pred = states_[st.dst].pred;
      auto tpred = std::find(pred.begin(), pred.end(), t);
      assert(tpred != succ.end());
      *tpred = pred.back();
      pred.pop_back();

      // Actually erase the transition.
      st.src = invalid;
      transitions_fs_.push_back(t);
    }

    void
    del_transition(state_t src, state_t dst, label_t l)
    {
      transition_t t = get_transition(src, dst, l);
      if (t != invalid)
	del_transition(t);
    }

    transition_t
    set_transition(state_t src, state_t dst, label_t l, weight_t k)
    {
      transition_t t = get_transition(src, dst, l);
      if (t != invalid)
	{
	  if (!ws_.is_zero(k))
	    {
	      stored_transition_t& st = transitions_[t];
	      st.label = l;
	      st.set_weight(k);
	    }
	  else
	    {
	      del_transition(t);
	      t = invalid;
	    }
	}
      else if (!ws_.is_zero(k))
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

    transition_t
    set_transition(state_t src, state_t dst, label_t l)
    {
      return set_transition(src, dst, l, ws_.unit());
    }

    weight_t
    set_weight(transition_t t, weight_t k)
    {
      if (ws_.is_zero(k))
	del_transition(t);
      else
	transitions_[t].set_weight(k);
      return k;
    }

    weight_t
    add_transition(state_t src, state_t dst, label_t l, weight_t k)
    {
      transition_t t = get_transition(src, dst, l);
      if (t != invalid)
	{
	  k = ws_.add(weight_of(t), k);
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
      return add_transition(src, dst, l, ws_.unit());
    }

    weight_t
    add_weight(transition_t t, weight_t k)
    {
      k = ws_.add(weight_of(t), k);
      set_weight(t, k);
      return k;
    }

    container_filter_range<boost::integer_range<transition_t> >
    transitions() const
    {
      return container_filter_range<boost::integer_range<transition_t> >
	(boost::irange<transition_t>(0U, transitions_.size()),
	 [&] (transition_t i) { return transitions_[i].src != invalid; });
    }

    container_filter_range<boost::integer_range<state_t> >
    states() const
    {
      return container_filter_range<boost::integer_range<state_t> >
	(boost::irange<state_t>(0U, states_.size()),
	 [&] (state_t i) {
	  const stored_state_t& ss = states_[i];
	  return ss.succ.empty() || ss.succ.front() != invalid;
	});
    }

    container_range<st_cont_t>
    initials() const
    {
      return container_range<st_cont_t>(initials_);
    }

    container_range<st_cont_t>
    finals() const
    {
      return container_range<st_cont_t>(finals_);
    }

    // Invalidated by del_transition() and del_state().
    container_range<tr_cont_t>
    out(state_t s) const
    {
      assert(has_state(s));
      const stored_state_t& ss = states_[s];
      return container_range<tr_cont_t>(ss.succ);
    }

    // Invalidated by del_transition() and del_state().
    container_filter_range<tr_cont_t>
    out(state_t s, label_t l) const
    {
      assert(has_state(s));
      const stored_state_t& ss = states_[s];
      return container_filter_range<tr_cont_t>
	(ss.succ,
	 [&] (transition_t i) { return a_.equals(transitions_[i].label, l); });
    }

    // Invalidated by del_transition() and del_state().
    container_range<tr_cont_t>
    in(state_t s) const
    {
      assert(has_state(s));
      const stored_state_t& ss = states_[s];
      return container_range<tr_cont_t>(ss.pred);
    }

    // Invalidated by del_transition() and del_state().
    container_filter_range<tr_cont_t>
    in(state_t s, label_t l) const
    {
      assert(has_state(s));
      const stored_state_t& ss = states_[s];
      return container_filter_range<tr_cont_t>
	(ss.pred,
	 [&] (transition_t i) { return a_.equals(transitions_[i].label, l); });
    }

    // Invalidated by del_transition() and del_state().
    container_filter_range<tr_cont_t>
    outin(state_t s, state_t d) const
    {
      assert(has_state(s));
      assert(has_state(d));
      const stored_state_t& ss = states_[s];
      return container_filter_range<tr_cont_t>
	(ss.succ,
	 [&] (transition_t i) { return (transitions_[i].dst == d); });
    }

  };

  template <class Alphabet, class WeightSet, class Kind>
  const typename mutable_automaton<Alphabet, WeightSet, Kind>::weightset_t&
  mutable_automaton<Alphabet, WeightSet, Kind>::st_ws_ = WeightSet();
}

#endif // !MUTABLE_AUTOMATON
