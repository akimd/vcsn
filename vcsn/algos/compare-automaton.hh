#pragma once

#include <boost/range/algorithm/lexicographical_compare.hpp>

#include <vcsn/core/automaton.hh>

namespace vcsn
{

  /*---------------------------------.
  | compare(automaton, automaton).   |
  `---------------------------------*/

  /// Comparison between lhs and rhs.
  ///
  /// Comparing two automata for order is really dubious.  Even
  /// comparing for equality is really difficult, as is proved by
  /// are-isomorphic.  Yet, to be able to use automata as weights, we
  /// need to be able to order-compare two automata, and to hash an
  /// automaton.
  ///
  /// Both problems are ill-defined from a semantical point of view,
  /// in automata theory, but from an implementation point of view, we
  /// can decide for some order between automata.  For instance, by
  /// lexicograpical comparison of all their transitions.
  template <Automaton Lhs, Automaton Rhs>
  int
  compare(const Lhs& lhs, const Rhs& rhs)
  {
    return detail::lexicographical_cmp
      (lhs->all_transitions(),
       rhs->all_transitions(),
       [&lhs, &rhs](transition_t_of<Lhs> t1, transition_t_of<Rhs> t2)
       {
         // First, on src.
         if (auto res = int(lhs->src_of(t1)) - int(rhs->src_of(t2)))
           return res;
         // Second, on the label.
         else if (auto res = lhs->labelset()->compare(lhs->label_of(t1),
                                                      rhs->label_of(t2)))
           return res;
         // Third, on the weight.
         else if (auto res = lhs->weightset()->compare(lhs->weight_of(t1),
                                                       rhs->weight_of(t2)))
           return res;
         // Last, on dst.
         else
           return int(lhs->dst_of(t1)) - int(rhs->dst_of(t2));
       });
  }

  /// Whether lhs is equal to rhs.
  template <Automaton Lhs, Automaton Rhs>
  bool
  are_equal(const Lhs& lhs, const Rhs& rhs)
  {
    return compare(lhs, rhs) == 0;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (are_equal).
      template <Automaton Lhs, Automaton Rhs>
      bool
      are_equal(const automaton& lhs, const automaton& rhs)
      {
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return ::vcsn::are_equal(l, r);
      }
    }
  }

  /// Whether lhs precedes rhs.
  template <Automaton Lhs, Automaton Rhs>
  bool
  less_than(const Lhs& lhs, const Rhs& rhs)
  {
    return compare(lhs, rhs) < 0;
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
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return ::vcsn::less_than(l, r);
      }
    }
  }
} // namespace vcsn
