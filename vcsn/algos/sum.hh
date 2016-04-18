#pragma once

#include <map>

#include <vcsn/algos/determinize.hh>
#include <vcsn/algos/standard.hh> // is_standard
#include <vcsn/algos/tags.hh>
#include <vcsn/core/join-automata.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/dyn/expansion.hh> // dyn::make_expansion
#include <vcsn/dyn/expression.hh> // dyn::make_expression
#include <vcsn/dyn/polynomial.hh>
#include <vcsn/dyn/value.hh>
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
  template <Automaton Aut1, Automaton Aut2>
  Aut1&
  sum_here(Aut1& res, const Aut2& b, deterministic_tag)
  {
    // FIXME: this is a stub: there are better algorithms to compute a
    // deterministic sum of deterministic automata.
    sum_here(res, b, standard_tag{});
    res = determinize(res)->strip();
    return res;
  }

  /// Merge transitions of \a b into those of \a res.
  ///
  /// \pre The context of \a res must include that of \a b.
  /// \pre res and b must be standard.
  template <Automaton Aut1, Automaton Aut2>
  Aut1&
  sum_here(Aut1& res, const Aut2& b, standard_tag)
  {
    require(is_standard(res), __func__, ": lhs must be standard");
    require(is_standard(b), __func__, ": rhs must be standard");

    // State in B -> state in Res.
    auto m = std::map<state_t_of<Aut2>, state_t_of<Aut1>>
      {
        {b->pre(), res->pre()},
        {b->post(), res->post()},
      };
    state_t_of<Aut1> initial = res->dst_of(initial_transitions(res).front());
    for (auto s: b->states())
      m.emplace(s, b->is_initial(s) ? initial : res->new_state());

    // Add b.
    for (auto t: all_transitions(b))
      if (b->dst_of(t) == b->post())
        res->add_transition(m[b->src_of(t)], m[b->dst_of(t)],
                            b->label_of(t),
                            res->weightset()->conv(*b->weightset(),
                                                   b->weight_of(t)));
      // Do not add initial transitions, the unique initial state is
      // already declared as such, and its weight must remain 1.
      else if (b->src_of(t) != b->pre())
        res->new_transition(m[b->src_of(t)], m[b->dst_of(t)],
                            b->label_of(t),
                            res->weightset()->conv(*b->weightset(),
                                                   b->weight_of(t)));
    return res;
  }

  /// Merge transitions of \a b into those of \a res.
  ///
  /// \pre Aut2 <: Aut1.
  template <Automaton Aut1, Automaton Aut2>
  Aut1&
  sum_here(Aut1& res, const Aut2& b, general_tag)
  {
    copy_into(b, res);
    return res;
  }

  /// The sum of two automata.
  ///
  /// \param lhs  the first automaton.
  /// \param rhs  the second automaton.
  /// \param tag  whether to use constructs for standard automata.
  template <Automaton Aut1, Automaton Aut2, typename Tag = general_tag>
  auto
  sum(const Aut1& lhs, const Aut2& rhs, Tag tag = {})
    -> decltype(join_automata(lhs, rhs))
  {
    auto res = join_automata(lhs, rhs);
    copy_into(lhs, res);
    sum_here(res, rhs, tag);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Lhs, Automaton Rhs, typename String>
      automaton
      sum(const automaton& lhs, const automaton& rhs, const std::string& algo)
      {
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return ::vcsn::detail::dispatch_tags(algo,
            [l, r](auto tag)
            {
              return automaton(::vcsn::sum(l, r, tag));
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
        auto join_elts = join<PolynomialSetLhs, PolynomialSetRhs>(lhs, rhs);
        return make_polynomial(std::get<0>(join_elts),
                               sum(std::get<0>(join_elts),
                                   std::get<1>(join_elts),
                                   std::get<2>(join_elts)));
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
        auto rs = join(l.valueset(), r.valueset());
        auto lr = rs.conv(l.valueset(), l.value());
        auto rr = rs.conv(r.valueset(), r.value());
        return {rs, ::vcsn::sum(rs, lr, rr)};
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
        auto join_elts = join<ExpSetLhs, ExpSetRhs>(lhs, rhs);
        return make_expression(std::get<0>(join_elts),
                               ::vcsn::sum(std::get<0>(join_elts),
                                           std::get<1>(join_elts),
                                           std::get<2>(join_elts)));
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
        auto rs = join(l.valueset(), r.valueset());
        auto lr = rs.conv(l.valueset(), l.value());
        auto rr = rs.conv(r.valueset(), r.value());
        return {rs, sum(rs, lr, rr)};
      }
    }
  }
}
