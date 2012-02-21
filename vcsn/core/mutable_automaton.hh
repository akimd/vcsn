#ifndef VCSN_CORE_MUTABLE_AUTOMATON_HH
#define VCSN_CORE_MUTABLE_AUTOMATON_HH

#include "kind.hh"
#include <vector>
#include <unordered_set>
#include <algorithm>
#include "crange.hh"
#include <cassert>

namespace vcsn
{
  template<class Alphabet, class Weights, class Kind>
  class mutable_automaton
  {
  protected:
    class stored_state_t;
    class stored_transition_t;
  public:
    typedef Alphabet alphabet_t;
    typedef Weights weights_t;
    typedef typename weights_t::value_t weight_t;
    typedef typename label_trait<Kind, Alphabet>::label_t label_t;

    typedef stored_state_t* state_t;
    typedef stored_transition_t* transition_t;
  protected:
    const alphabet_t& a_;
    const weights_t& w_;

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

    mutable_automaton(const alphabet_t& a, const weights_t& w = weights_t())
      : a_(a), w_(w)
    {
    }

    ~mutable_automaton()
    {
      for (auto t : transitions_)
	delete t;
      for (auto s : states_)
	delete s;
    }

    const weights_t&
    get_weights() const
    {
      return w_;
    }

    const alphabet_t&
    get_alphabet() const
    {
      return a_;
    }

    state_t
    add_state()
    {
      state_t res = new stored_state_t;
      res->final = res->initial = w_.zero();
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
      if (w_.is_zero(k))
	initials_.erase(s);
      else
	initials_.insert(s);
    }

    void
    set_initial(state_t s)
    {
      set_initial(s, w_.unit());
    }

    weight_t
    add_initial(state_t s, weight_t k)
    {
      k = w_.add(s->initial, k);
      set_initial(s, k);
      return k;
    }

    bool
    is_initial(state_t s)
    {
      return !w_.is_zero(s->initial);
    }

    void
    set_final(state_t s, weight_t k)
    {
      s->final = k;
      if (w_.is_zero(k))
	finals_.erase(s);
      else
	finals_.insert(s);
    }

    void
    set_final(state_t s)
    {
      set_final(s, w_.unit());
    }

    weight_t
    add_final(state_t s, weight_t k)
    {
      k = w_.add(s->final, k);
      set_final(s, k);
      return k;
    }

    bool
    is_final(state_t s)
    {
      return !w_.is_zero(s->final);
    }

    size_t nb_states() const { return states_.size(); }
    size_t nb_transitions() const { return transitions_.size(); }
    size_t nb_initials() const { return initials_.size(); }
    size_t nb_finals() const { return finals_.size(); }

    transition_t
    get_transition(state_t src, state_t dst, label_t w)
    {
      tr_vector_t& succ = src->succ;
      auto i =
	std::find_if(begin(succ), end(succ),
		     [&] (const transition_t& t)
		     { return t->dst == dst && a_.equals(t->label, w); });
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
    has_transition(state_t src, state_t dst, label_t w)
    {
      return get_transition(src, dst, w);
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
    del_transition(state_t src, state_t dst, label_t w)
    {
      transition_t t = get_transition(src, dst, w);
      if (t)
	del_transition(t);
    }

    transition_t
    set_transition(state_t src, state_t dst, label_t w, weight_t k)
    {
      transition_t t = get_transition(src, dst, w);
      if (t)
	{
	  if (!w_.is_zero(k))
	    {
	      t->label = w;
	      t->weight = k;
	    }
	  else
	    {
	      del_transition(t);
	      t = 0;
	    }
	}
      else if (!w_.is_zero(k))
	{
	  t = new stored_transition_t;
	  t->src = src;
	  t->dst = dst;
	  t->label = w;
	  t->weight = k;
	  transitions_.insert(t);
	  src->succ.push_back(t);
	  dst->pred.push_back(t);
	}
      return t;
    }

    transition_t
    set_transition(state_t src, state_t dst, label_t w)
    {
      return set_transition(src, dst, w, w_.unit());
    }

    weight_t
    set_transition(transition_t t, weight_t k)
    {
      if (w_.is_zero(k))
	del_transition(t);
      else
	t->weight = k;
      return k;
    }

    weight_t
    add_transition(state_t src, state_t dst, label_t w, weight_t k)
    {
      transition_t t = get_transition(src, dst, w);
      if (t)
	{
	  k = w_.add(t->weight, k);
	  set_transition(t, k);
	}
      else
	{
	  set_transition(src, dst, w, k);
	}
      return k;
    }

    weight_t
    add_transition(state_t src, state_t dst, label_t w)
    {
      return add_transition(src, dst, w, w_.unit());
    }

    weight_t
    add_transition(transition_t t, weight_t k)
    {
      k = w_.add(t->weight, k);
      set_transition(t, k);
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
    out(state_t s, label_t w)
    {
      return container_filter_range<tr_vector_t>
	(s->succ,
	 [&] (transition_t i) { return a_.equals(i->label, w); });
    }

    // Invalidated by del_transition() and del_state().
    container_range<tr_vector_t>
    in(state_t s) { return container_range<tr_vector_t>(s->pred); }

    // Invalidated by del_transition() and del_state().
    container_filter_range<tr_vector_t>
    in(state_t s, label_t w)
    {
      return container_filter_range<tr_vector_t>
	(s->pred,
	 [&] (transition_t i) { return a_.equals(i->label, w); });
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
}

#endif // MUTABLE_AUTOMATON
