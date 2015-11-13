#pragma once

#include <map>

#include <vcsn/algos/standard.hh> // is_standard
#include <vcsn/algos/union.hh>
#include <vcsn/core/join-automata.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/dyn/expression.hh> // dyn::make_expression
#include <vcsn/dyn/expansion.hh> // dyn::make_expansion
#include <vcsn/dyn/polynomial.hh>
#include <vcsn/dyn/weight.hh>
#include <vcsn/misc/raise.hh> // require

namespace vcsn
{

  /*----------------------------.
  | sum(automaton, automaton).  |
  `----------------------------*/

  /// Merge transitions of \a b into those of \a res.
  ///
  /// \pre The context of \a res must include that of \a b.
  /// \pre res and b must be standard.
  template <typename A, typename B>
  A&
  sum_here(A& res, const B& b, standard_tag)
  {
    require(is_standard(res), __func__, ": lhs must be standard");
    require(is_standard(b), __func__, ": rhs must be standard");

    // State in B -> state in Res.
    std::map<state_t_of<B>, state_t_of<A>> m;
    state_t_of<A> initial = res->dst_of(res->initial_transitions().front());
    for (auto s: b->states())
      m.emplace(s, b->is_initial(s) ? initial : res->new_state());
    m.emplace(b->pre(), res->pre());
    m.emplace(b->post(), res->post());

    // Add b.
    for (auto t: b->all_transitions())
      // Do not add initial transitions, the unique initial state is
      // already declared as such, and its weight must remain 1.
      if (b->src_of(t) != b->pre())
        {
          if (b->dst_of(t) == b->post())
            res->add_transition(m[b->src_of(t)], m[b->dst_of(t)],
                                b->label_of(t),
                                res->weightset()->conv(*b->weightset(),
                                                       b->weight_of(t)));
          else
            res->new_transition(m[b->src_of(t)], m[b->dst_of(t)],
                                b->label_of(t),
                                res->weightset()->conv(*b->weightset(),
                                                       b->weight_of(t)));
        }
    return res;
  }


  template <typename A, typename B>
  inline
  auto
  sum(const A& lhs, const B& rhs, standard_tag)
    -> decltype(join_automata(lhs, rhs))
  {
    auto res = join_automata(lhs, rhs);
    // A standard automaton has a single initial state.
    res->set_initial(res->new_state());
    sum_here(res, lhs, standard_tag{});
    sum_here(res, rhs, standard_tag{});
    return res;
  }

  template <typename A, typename B>
  inline
  auto
  sum(const A& lhs, const B& rhs, general_tag = {})
    -> decltype(join_automata(lhs, rhs))
  {
    return union_a(lhs, rhs);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Lhs, typename Rhs, typename String>
      automaton
      sum(const automaton& lhs, const automaton& rhs, const std::string& algo)
      {
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return ::vcsn::detail::dispatch_standard(algo,
            [l, r](auto tag)
            {
              return make_automaton(::vcsn::sum(l, r, tag));
            },
        l, r);
      }
    }
  }


  /*---------------------.
  | sum(Value, Value).   |
  `---------------------*/

  /// Sums of values.
  template <typename ValueSet>
  inline
  typename ValueSet::value_t
  sum(const ValueSet& vs,
      const typename ValueSet::value_t& lhs,
      const typename ValueSet::value_t& rhs)
  {
    return vs.add(lhs, rhs);
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge (sum).
      template <typename PolynomialSetLhs, typename PolynomialSetRhs>
      polynomial
      sum_polynomial(const polynomial& lhs, const polynomial& rhs)
      {
        const auto& l = lhs->as<PolynomialSetLhs>();
        const auto& r = rhs->as<PolynomialSetRhs>();
        auto rs = join(l.polynomialset(), r.polynomialset());
        auto lr = rs.conv(l.polynomialset(), l.polynomial());
        auto rr = rs.conv(r.polynomialset(), r.polynomial());
        return make_polynomial(rs, sum(rs, lr, rr));
      }
    }
  }


  /*-----------------------------.
  | sum(expansion, expansion).   |
  `-----------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (sum).
      template <typename ExpansionSetLhs, typename ExpansionSetRhs>
      expansion
      sum_expansion(const expansion& lhs, const expansion& rhs)
      {
        const auto& l = lhs->as<ExpansionSetLhs>();
        const auto& r = rhs->as<ExpansionSetRhs>();
        auto rs = join(l.expansionset(), r.expansionset());
        auto lr = rs.conv(l.expansionset(), l.expansion());
        auto rr = rs.conv(r.expansionset(), r.expansion());
        return make_expansion(rs, ::vcsn::sum(rs, lr, rr));
      }
    }
  }

  /*-------------------------------.
  | sum(expression, expression).   |
  `-------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (sum).
      template <typename ExpSetLhs, typename ExpSetRhs>
      expression
      sum_expression(const expression& lhs, const expression& rhs)
      {
        const auto& l = lhs->as<ExpSetLhs>();
        const auto& r = rhs->as<ExpSetRhs>();
        auto rs = join(l.expressionset(), r.expressionset());
        auto lr = rs.conv(l.expressionset(), l.expression());
        auto rr = rs.conv(r.expressionset(), r.expression());
        return make_expression(rs, ::vcsn::sum(rs, lr, rr));
      }
    }
  }


  /*----------------------.
  | sum(weight, weight).  |
  `----------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (sum).
      template <typename WeightSetLhs, typename WeightSetRhs>
      weight
      sum_weight(const weight& lhs, const weight& rhs)
      {
        const auto& l = lhs->as<WeightSetLhs>();
        const auto& r = rhs->as<WeightSetRhs>();
        auto rs = join(l.weightset(), r.weightset());
        auto lr = rs.conv(l.weightset(), l.weight());
        auto rr = rs.conv(r.weightset(), r.weight());
        return make_weight(rs, sum(rs, lr, rr));
      }
    }
  }

}
