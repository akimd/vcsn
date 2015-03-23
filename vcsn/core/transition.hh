#ifndef VCSN_CORE_TRANSITION_HH
# define VCSN_CORE_TRANSITION_HH

# include <vcsn/empty.hh>

# include <vcsn/misc/attributes.hh>

namespace vcsn
{

  /*------------------------------------.
  | possibly_labeled_transition_tuple.  |
  `------------------------------------*/

  template <typename State, class Label>
  struct possibly_labeled_transition_tuple
  {
    State src;
    State dst;

    using label_t = Label;
    label_t get_label() const { return label; }
    void set_label(label_t& l) { label = l; }

  private:
    Label label;
  };

  template <typename State>
  struct possibly_labeled_transition_tuple<State, empty_t>
  {
    State src;
    State dst;

    using label_t = empty_t;
    label_t get_label() const { return {}; }
    void set_label(label_t) {}
  };


  /*-------------------.
  | transition_tuple.  |
  `-------------------*/

  template <typename State, class Label, class Weight>
  struct transition_tuple
    : possibly_labeled_transition_tuple<State, Label>
  {
    using weight_t = Weight;
    weight_t get_weight() const { return weight; }
    void set_weight(weight_t& k) { weight = k; }

  private:
    weight_t weight;
  };

  // We do not store the Boolean weights, which are assumed to be
  // always true.  This is correct for weight in the Boolean ring, as
  // well as for those in the F₂ (a.k.a. ℤ/2ℤ) field, both encoded
  // using the bool type.
  template <typename State, class Label>
  struct transition_tuple<State, Label, bool>
    : possibly_labeled_transition_tuple<State, Label>
  {
    using weight_t = bool;
    weight_t get_weight() const { return true; }
    void set_weight(weight_t& k) ATTRIBUTE_PURE { (void) k; assert(k == true); }
  };

}

#endif // !VCSN_CORE_TRANSITION_HH
