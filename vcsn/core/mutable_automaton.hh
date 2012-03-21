#ifndef VCSN_CORE_MUTABLE_AUTOMATON_HH
# define VCSN_CORE_MUTABLE_AUTOMATON_HH

# include <vector>
# include <unordered_set>
# include <algorithm>
# include <cassert>

# include "crange.hh"
# include "kind.hh"

namespace vcsn
{
  template <class Alphabet, class WeightSet, class Kind>
  class mutable_automaton
  {
  protected:
    class stored_state_t;
    class stored_transition_t;
  public:
    typedef Alphabet alphabet_t;
    typedef WeightSet weightset_t;
    typedef typename weightset_t::value_t weight_t;
    typedef typename label_trait<Kind, Alphabet>::label_t label_t;

    typedef stored_state_t* state_t;
    typedef stored_transition_t* transition_t;
  protected:
    const alphabet_t& a_;
    const weightset_t& ws_;
    static const weightset_t& st_ws_;

    struct stored_transition_t
    {
      state_t src;
      label_t label;
      weight_t weight;
      state_t dst;
    };

    typedef std::vector<transition_t> tr_vector_t;

    struct stored_state_t
    {
      weight_t initial;
      weight_t final;
      tr_vector_t succ;
      tr_vector_t pred;
    };

    typedef std::unordered_set<state_t> st_cont_t;
    typedef std::unordered_set<transition_t> tr_cont_t;

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

    ~mutable_automaton()
    {
      for (auto t : transitions_)
	delete t;
      for (auto s : states_)
	delete s;
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
      state_t res = new stored_state_t;
      res->final = res->initial = ws_.zero();
      states_.insert(res);
      return res;
    }

    bool
    has_state(state_t s) const
    {
      return states_.find(s) != states_.end();
    }

    void
    del_state(state_t s)
    {
      auto i = states_.find(s);
      assert(i != states_.end());
      for (auto t : s->succ)
	transitions_.erase(t);
      for (auto t : s->pred)
	transitions_.erase(t);
      states_.erase(i);
    }

    weight_t
    get_initial_weight(state_t s)
    {
      return s->initial;
    }

    weight_t
    get_final_weight(state_t s)
    {
      return s->final;
    }

    void
    set_initial(state_t s, weight_t k)
    {
      s->initial = k;
      if (ws_.is_zero(k))
	initials_.erase(s);
      else
	initials_.insert(s);
    }

    void
    set_initial(state_t s)
    {
      set_initial(s, ws_.unit());
    }

    weight_t
    add_initial(state_t s, weight_t k)
    {
      k = ws_.add(s->initial, k);
      set_initial(s, k);
      return k;
    }

    void
    unset_initial(state_t s)
    {
      set_initial(s, ws_.zero());
    }

    bool
    is_initial(state_t s)
    {
      return !ws_.is_zero(s->initial);
    }

    void
    set_final(state_t s, weight_t k)
    {
      s->final = k;
      if (ws_.is_zero(k))
	finals_.erase(s);
      else
	finals_.insert(s);
    }

    void
    set_final(state_t s)
    {
      set_final(s, ws_.unit());
    }

    weight_t
    add_final(state_t s, weight_t k)
    {
      k = ws_.add(s->final, k);
      set_final(s, k);
      return k;
    }

    void
    unset_final(state_t s)
    {
      set_final(s, ws_.zero());
    }

    bool
    is_final(state_t s)
    {
      return !ws_.is_zero(s->final);
    }

    size_t nb_states() const { return states_.size(); }
    size_t nb_transitions() const { return transitions_.size(); }
    size_t nb_initials() const { return initials_.size(); }
    size_t nb_finals() const { return finals_.size(); }

    transition_t
    get_transition(state_t src, state_t dst, label_t l)
    {
      tr_vector_t& succ = src->succ;
      auto i =
	std::find_if(begin(succ), end(succ),
		     [&] (const transition_t& t)
		     { return t->dst == dst && a_.equals(t->label, l); });
      if (i == end(succ))
	return 0;
      return *i;
    }

    bool
    has_transition(transition_t t)
    {
      return transitions_.find(t) != transitions_.end();
    }

    bool
    has_transition(state_t src, state_t dst, label_t l)
    {
      return get_transition(src, dst, l);
    }

    void
    del_transition(transition_t t)
    {
      assert(has_transition(t));

      transitions_.erase(t);

      auto& succ = t->src->succ;
      auto ts = std::find(succ.begin(), succ.end(), t);
      assert(ts != succ.end());
      *ts = succ.back();
      succ.pop_back();

      auto& pred = t->dst->pred;
      auto tp = std::find(pred.begin(), pred.end(), t);
      assert(tp != succ.end());
      *tp = pred.back();
      pred.pop_back();
    }

    void
    del_transition(state_t src, state_t dst, label_t l)
    {
      if (transition_t t = get_transition(src, dst, l))
	del_transition(t);
    }

    transition_t
    set_transition(state_t src, state_t dst, label_t l, weight_t k)
    {
      transition_t t = get_transition(src, dst, l);
      if (t)
	{
	  if (!ws_.is_zero(k))
	    {
	      t->label = l;
	      t->weight = k;
	    }
	  else
	    {
	      del_transition(t);
	      t = 0;
	    }
	}
      else if (!ws_.is_zero(k))
	{
	  t = new stored_transition_t;
	  t->src = src;
	  t->dst = dst;
	  t->label = l;
	  t->weight = k;
	  transitions_.insert(t);
	  src->succ.push_back(t);
	  dst->pred.push_back(t);
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
	t->weight = k;
      return k;
    }

    weight_t
    add_transition(state_t src, state_t dst, label_t l, weight_t k)
    {
      if (transition_t t = get_transition(src, dst, l))
	{
	  k = ws_.add(t->weight, k);
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
      k = ws_.add(t->weight, k);
      set_weight(t, k);
      return k;
    }



  public:

    container_range<tr_cont_t>
    transitions() { return container_range<tr_cont_t>(transitions_); }

    container_range<st_cont_t>
    states() { return container_range<st_cont_t>(states_); }

    container_range<st_cont_t>
    initials() { return container_range<st_cont_t>(initials_); }

    container_range<st_cont_t>
    finals() { return container_range<st_cont_t>(finals_); }

    // Invalidated by del_transition() and del_state().
    container_range<tr_vector_t>
    out(state_t s) { return container_range<tr_vector_t>(s->succ); }

    // Invalidated by del_transition() and del_state().
    container_filter_range<tr_vector_t>
    out(state_t s, label_t l)
    {
      return container_filter_range<tr_vector_t>
	(s->succ,
	 [&] (transition_t i) { return a_.equals(i->label, l); });
    }

    // Invalidated by del_transition() and del_state().
    container_range<tr_vector_t>
    in(state_t s) { return container_range<tr_vector_t>(s->pred); }

    // Invalidated by del_transition() and del_state().
    container_filter_range<tr_vector_t>
    in(state_t s, label_t l)
    {
      return container_filter_range<tr_vector_t>
	(s->pred,
	 [&] (transition_t i) { return a_.equals(i->label, l); });
    }

    // Invalidated by del_transition() and del_state().
    container_filter_range<tr_vector_t>
    outin(state_t s, state_t d)
    {
      return container_filter_range<tr_vector_t>
	(s->succ, [&] (transition_t i) { return (i->dst == d); });
    }

    state_t src_of(transition_t t) const     { return t->src; }
    state_t dst_of(transition_t t) const     { return t->dst; }
    label_t label_of(transition_t t) const   { return t->label; }
    weight_t weight_of(transition_t t) const { return t->weight; }

  protected:

    st_cont_t initials_;
    st_cont_t finals_;
    st_cont_t states_;
    tr_cont_t transitions_;
  };

  template <class Alphabet, class WeightSet, class Kind>
  const typename mutable_automaton<Alphabet, WeightSet, Kind>::weightset_t&
  mutable_automaton<Alphabet, WeightSet, Kind>::st_ws_ = WeightSet();
}

#endif // !MUTABLE_AUTOMATON
