#pragma once

#include <cassert>

#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/empty.hh>

namespace vcsn
{

  /*------------------------------------.
  | possibly_labeled_transition_tuple.  |
  `------------------------------------*/

  /// Transition on non-empty label.
  template <typename State, typename Label>
  struct possibly_labeled_transition_tuple
  {
    using label_t = Label;
    possibly_labeled_transition_tuple(State s, State d, label_t l)
      : src{s}, dst{d}, label{l}
    {}

    State src;
    State dst;

    label_t get_label() const { return label; }
    void set_label(label_t& l) { label = l; }

  private:
    Label label;
  };

  /// Transition on empty label.
  template <typename State>
  struct possibly_labeled_transition_tuple<State, empty_t>
  {
    using label_t = empty_t;
    possibly_labeled_transition_tuple(State s, State d, label_t)
      : src{s}, dst{d}
    {}

    State src;
    State dst;

    label_t get_label() const { return {}; }
    void set_label(label_t) {}
  };


  /*-------------------.
  | transition_tuple.  |
  `-------------------*/

  /// Transition with label and non Boolean weight.
  template <typename State, typename Label, typename Weight>
  struct transition_tuple
    : possibly_labeled_transition_tuple<State, Label>
  {
    using super_t = possibly_labeled_transition_tuple<State, Label>;
    using weight_t = Weight;
    transition_tuple(State s, State d, Label l, weight_t w)
      : super_t{s, d, l}
      , weight{w}
    {}

    weight_t get_weight() const { return weight; }
    void set_weight(weight_t& k) { weight = k; }

  private:
    weight_t weight;
  };

  /// Transition with label and Boolean weight.
  ///
  /// We do not store the Boolean weights, which are assumed to be
  /// always true.  This is correct for weight in the Boolean ring, as
  /// well as for those in the F₂ (a.k.a. ℤ/2ℤ) field, both encoded
  /// using the bool type.
  template <typename State, typename Label>
  struct transition_tuple<State, Label, bool>
    : possibly_labeled_transition_tuple<State, Label>
  {
    using super_t = possibly_labeled_transition_tuple<State, Label>;
    using weight_t = bool;
    transition_tuple(State s, State d, Label l, weight_t)
      : super_t{s, d, l}
    {}
    weight_t get_weight() const { return true; }
    void set_weight(weight_t& k) ATTRIBUTE_PURE { (void) k; assert(k == true); }
  };

}
