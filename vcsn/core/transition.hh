#ifndef VCSN_CORE_TRANSITION_HH
# define VCSN_CORE_TRANSITION_HH

#include "vcsn/weights/b.hh"

namespace vcsn
{
  template<class State, class Label>
  struct possibly_labeled_transition_tuple
  {
    State src;
    State dst;
    Label label;
  };

  template<class State>
  struct possibly_labeled_transition_tuple<State, void>
  {
    State src;
    State dst;
  };

  template<class State, class Label, class WeightSet>
  struct transition_tuple
    : possibly_labeled_transition_tuple<State, Label>
  {
    typedef typename WeightSet::value_t weight_t;
    weight_t weight;

    weight_t get_weight() const { return weight; }
    void set_weight(weight_t& k) { weight = k; }
  };

  // For Boolean automata we do not store the weights, which are
  // always true.  We have to specialize on the WeightSet, not on
  // weight_t, because 'bool' could be used to encode a different
  // WeightSet (like F₂ a.k.a. ℤ/2ℤ) for which we want to store the
  // two values.
  template<class State, class Label>
  struct transition_tuple<State, Label, b>
    : possibly_labeled_transition_tuple<State, Label>
  {
    typedef typename b::value_t weight_t;

    weight_t get_weight() const { return true; }
    void set_weight(weight_t& k) { assert(k == true); }
  };

}

#endif // VCSN_CORE_TRANSITION_HH
