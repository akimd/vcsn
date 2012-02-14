#ifndef VCSN_CORE_MUTABLE_AUTOMATON_HH
#define VCSN_CORE_MUTABLE_AUTOMATON_HH

#include "kind.hh"
#include <vector>
#include <unordered_set>
#include <algorithm>

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

    void
    add_initial(state_t s, weight_t k)
    {
      set_initial(s, w_.add(s->initial, k));
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

    void
    add_final(state_t s, weight_t k)
    {
      set_final(s, w_.add(s->final, k));
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
    set_transition(state_t src, state_t dst, label_t w, weight_t k)
    {
      transition_t t = get_transition(src, dst, w);
      if (t)
	{
	  t->label = w;
	  t->weight = k;
	}
      else
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


  protected:
    // Restrict the interface of a container to begin/end.
    template<class C>
    struct container_range
    {
    public:
      container_range(C& cont) : cont_(cont) {}
      typename C::iterator begin() { return cont_.begin(); }
      typename C::iterator end()   { return cont_.end(); }
    private:
      C& cont_;
    };

  public:

    container_range<tr_cont_t>
    transitions() { return container_range<tr_cont_t>(transitions_); }

    container_range<st_cont_t>
    states() { return container_range<st_cont_t>(states_); }

    container_range<st_cont_t>
    initials() { return container_range<st_cont_t>(initials_); }

    container_range<st_cont_t>
    finals() { return container_range<st_cont_t>(finals_); }

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
