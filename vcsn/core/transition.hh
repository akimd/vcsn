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

  template<class State, class Label, class Weight>
  struct transition_tuple
    : possibly_labeled_transition_tuple<State, Label>
  {
    typedef Weight weight_t;
    weight_t weight;

    weight_t get_weight() const { return weight; }
    void set_weight(weight_t& k) { weight = k; }
  };

  // We do not store the Boolean weights, which are assumed to be
  // always true.  This is correct for weight in the Boolean ring, as
  // well as for for those in the F₂ (a.k.a. ℤ/2ℤ) field, both encoded
  // using the bool type.
  template<class State, class Label>
  struct transition_tuple<State, Label, bool>
    : possibly_labeled_transition_tuple<State, Label>
  {
    typedef bool weight_t;

    weight_t get_weight() const { return true; }
    void set_weight(weight_t& k) { assert(k == true); }
  };

}

#endif // !VCSN_CORE_TRANSITION_HH
