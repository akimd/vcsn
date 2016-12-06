#pragma once

#include <boost/range/algorithm/lexicographical_compare.hpp>

#include <vcsn/core/automaton.hh>

namespace vcsn
{

  /*-----------------------------------.
  | less_than(automaton, automaton).   |
  `-----------------------------------*/

  /// Whether lhs precedes rhs.
  ///
  /// Comparing two automata for order is really dubious.  Even
  /// comparing for equality is really difficult, as is proved by
  /// are-isomorphic.  Yet, to be able to use automata as weights, we
  /// need to be able to order-compare two automata, and to hash an
  /// automaton.
  ///
  /// Both problem are ill-defined from a semantical point of view, in
  /// automata theory, but from an implementation point of view, we
  /// can decide for some order between automata.  For instance, by
  /// lexicographical comparison of all their transitions.
  template <Automaton Lhs, Automaton Rhs>
  bool
  less_than(const Lhs& lhs, const Rhs& rhs)
  {
    return boost::range::lexicographical_compare
      (lhs->all_transitions(),
       rhs->all_transitions(),
       [&lhs, &rhs](transition_t_of<Lhs> t1, transition_t_of<Rhs> t2)
       {
         // First, on src.
         if (lhs->src_of(t1) < rhs->src_of(t2))
           return true;
         else if (rhs->src_of(t2) < lhs->src_of(t1))
           return false;
         // Second, on the label.
         else if (lhs->labelset()->less(lhs->label_of(t1), rhs->label_of(t2)))
           return true;
         else if (lhs->labelset()->less(rhs->label_of(t2), lhs->label_of(t1)))
           return false;
         // Third, on the weight.
         else if (lhs->weightset()->less(lhs->weight_of(t1), rhs->weight_of(t2)))
           return true;
         else if (lhs->weightset()->less(rhs->weight_of(t2), lhs->weight_of(t1)))
           return false;
         // Last, on dst.
         else
           return lhs->dst_of(t1) < rhs->dst_of(t2);
       });
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (less_than).
      template <Automaton Lhs, Automaton Rhs>
      bool
      less_than(const automaton& lhs, const automaton& rhs)
      {
        /// FIXME: will not work for automaton of different types.
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return ::vcsn::less_than(l, r);
      }
    }
  }
} // namespace vcsn
