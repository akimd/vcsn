#pragma once

#include <vcsn/algos/accessible.hh> // is_useless
#include <vcsn/algos/complement.hh>
#include <vcsn/algos/complete.hh>
#include <vcsn/algos/determinize.hh>
#include <vcsn/algos/derived-term.hh>
#include <vcsn/algos/weight.hh>
#include <vcsn/algos/letterize.hh> // rea
#include <vcsn/algos/conjunction.hh> // conjunction
#include <vcsn/algos/reduce.hh>
#include <vcsn/algos/strip.hh>
#include <vcsn/algos/add.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/value.hh>

namespace vcsn
{

  /*---------------------------------------.
  | are_equivalent(automaton, automaton).  |
  `---------------------------------------*/

  /// Check equivalence between free Boolean automata.
  template <Automaton Aut1, Automaton Aut2>
  auto
  are_equivalent(const Aut1& a1, const Aut2& a2)
    -> std::enable_if_t<(std::is_same_v<weightset_t_of<Aut1>, b>
                          && std::is_same_v<weightset_t_of<Aut2>, b>),
                         bool>
  {
    const auto& l = realtime(a1);
    const auto& r = realtime(a2);
    return (is_useless(difference(l, r))
            && is_useless(difference(r, l)));
  }


  /// Check equivalence between automata on fields, or Z.
  template <Automaton Aut1, Automaton Aut2>
  auto
  are_equivalent(const Aut1& a1, const Aut2& a2)
    -> decltype((std::enable_if_t<!std::is_same<weightset_t_of<Aut1>, b>()>(),
                 a2->weightset()->sub(a2->weightset()->zero(),
                                      a2->weightset()->one()),
                 true))
  {
    const auto& ws2 = *a2->weightset();
    const auto& l = realtime(a1);
    const auto& r = realtime(a2);
    // d = l + -r.
    auto d = add(l,
                 lweight(ws2.sub(ws2.zero(), ws2.one()),
                         r));
    return is_empty(reduce(d));
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut1, Automaton Aut2>
      bool
      are_equivalent(const automaton& aut1, const automaton& aut2)
      {
        const auto& a1 = aut1->as<Aut1>();
        const auto& a2 = aut2->as<Aut2>();
        return ::vcsn::are_equivalent(a1, a2);
      }
    }
  }


  /*-----------------------------------------.
  | are_equivalent(expression, expression).  |
  `-----------------------------------------*/

  /// Check equivalence between two expressions.
  template <typename ExpSet1, typename ExpSet2>
  auto
  are_equivalent(const ExpSet1& rs1,
                 const typename ExpSet1::value_t r1,
                 const ExpSet2& rs2,
                 const typename ExpSet2::value_t r2)
    -> bool
  {
    // We use derived-term because it supports all our operators.
    // FIXME: bench to see if standard would not be a better bet in
    // the other cases.
    return are_equivalent(strip(derived_term(rs1, r1)),
                          strip(derived_term(rs2, r2)));
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge (are_equivalent).
      template <typename ExpSet1, typename ExpSet2>
      bool
      are_equivalent_expression(const expression& r1, const expression& r2)
      {
        const auto& l = r1->as<ExpSet1>();
        const auto& r = r2->as<ExpSet2>();
        return ::vcsn::are_equivalent(l.valueset(), l.value(),
                                      r.valueset(), r.value());
      }
    }
  }


  /*-----------------------------------.
  | difference(automaton, automaton).  |
  `-----------------------------------*/

  /// An automaton that computes weights of \a lhs, but not by \a rhs.
  template <Automaton Lhs, Automaton Rhs>
  fresh_automaton_t_of<Lhs>
  difference(const Lhs& lhs, const Rhs& rhs)
  {
    // Meet complement()'s requirements.
    auto r = strip(rhs);
    if (!is_deterministic(r))
      r = complete(strip(determinize(r)));
    else if (!is_complete(r))
      r = complete(r);
    return strip(conjunction(lhs, complement(r)));
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Lhs, Automaton Rhs>
      automaton
      difference(const automaton& lhs, const automaton& rhs)
      {
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return ::vcsn::difference(l, r);
      }
    }
  }

  /*--------------------------------------.
  | difference(expression, expression).   |
  `--------------------------------------*/

  /// Difference of expressions.
  template <typename ExpSet>
  typename ExpSet::value_t
  difference(const ExpSet& rs,
             const typename ExpSet::value_t& lhs,
             const typename ExpSet::value_t& rhs)
  {
    return rs.conjunction(lhs, rs.complement(rhs));
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (difference).
      template <typename ExpSetLhs, typename ExpSetRhs>
      expression
      difference_expression(const expression& lhs, const expression& rhs)
      {
        const auto& l = lhs->as<ExpSetLhs>();
        const auto& r = rhs->as<ExpSetLhs>();
        return {l.valueset(), ::vcsn::difference(l.valueset(),
                                                 l.value(),
                                                 r.value())};
      }
    }
  }
}
