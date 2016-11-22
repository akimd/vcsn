#pragma once

#include <map>

#include <vcsn/algos/conjunction.hh>
#include <vcsn/algos/determinize.hh>
#include <vcsn/algos/standard.hh> // is_standard
#include <vcsn/algos/tags.hh>
#include <vcsn/core/join-automata.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/dyn/value.hh>
#include <vcsn/misc/raise.hh> // require

namespace vcsn
{

  /*----------------------------.
  | add(automaton, automaton).  |
  `----------------------------*/

  /// Merge transitions of \a b into those of \a res.
  ///
  /// \pre The context of \a res must include that of \a b.
  /// \pre res and b must be standard.
  template <Automaton Aut1, Automaton Aut2>
  Aut1&
  add_here(Aut1& res, const Aut2& b, standard_tag)
  {
    require(is_standard(res), __func__, ": lhs must be standard");
    require(is_standard(b), __func__, ": rhs must be standard");

    // State in B -> state in Res.
    auto m = std::map<state_t_of<Aut2>, state_t_of<Aut1>>
      {
        {b->pre(), res->pre()},
        {b->post(), res->post()},
      };
    state_t_of<Aut1> initial = res->dst_of(detail::initial_transitions(res).front());
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

  template <Automaton Aut1, Automaton Aut2>
  auto
  add_here(Aut1& res, const Aut2& b, deterministic_tag)
  {
    add_here(res, b, general_tag{});
    res = determinize(res)->strip();
    return res;
  }

  template <Automaton Aut1, Automaton Aut2>
  auto
  add(const Aut1& lhs, const Aut2& rhs, deterministic_tag)
  {
    constexpr bool bb = (std::is_same<weightset_t_of<Aut1>, b>::value
                        && std::is_same<weightset_t_of<Aut2>, b>::value);
    return detail::static_if<bb>(
      [] (const auto& lhs, const auto& rhs)
      {
        auto prod
          = detail::make_product_automaton<false>(join_automata(lhs, rhs),
                                                  lhs, rhs);
        prod->add();
        return prod->strip();
      },
      [] (const auto& lhs, const auto& rhs)
      {
        return determinize(add(lhs, rhs, general_tag{}))->strip();
      })(lhs, rhs);
  }

  /// Merge transitions of \a b into those of \a res.
  ///
  /// \pre Aut2 <: Aut1.
  template <Automaton Aut1, Automaton Aut2>
  Aut1&
  add_here(Aut1& res, const Aut2& b, general_tag)
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
  add(const Aut1& lhs, const Aut2& rhs, Tag tag = {})
    -> decltype(join_automata(lhs, rhs))
  {
    auto res = join_automata(lhs, rhs);
    copy_into(lhs, res);
    add_here(res, rhs, tag);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Lhs, Automaton Rhs, typename String>
      automaton
      add(const automaton& lhs, const automaton& rhs, const std::string& algo)
      {
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return ::vcsn::detail::dispatch_tags(algo,
            [l, r](auto tag)
            {
              return automaton(::vcsn::add(l, r, tag));
            },
        l, r);
      }
    }
  }


  /*---------------------.
  | add(Value, Value).   |
  `---------------------*/

  /// Sums of values.
  template <typename ValueSet>
  typename ValueSet::value_t
  add(const ValueSet& vs,
      const typename ValueSet::value_t& lhs,
      const typename ValueSet::value_t& rhs)
  {
    return vs.add(lhs, rhs);
  }


  /*-----------------------------.
  | add(expansion, expansion).   |
  `-----------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (add).
      template <typename ExpansionSetLhs, typename ExpansionSetRhs>
      expansion
      add_expansion(const expansion& lhs, const expansion& rhs)
      {
        auto join_elts = join<ExpansionSetLhs, ExpansionSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts),
                ::vcsn::add(std::get<0>(join_elts),
                            std::get<1>(join_elts), std::get<2>(join_elts))};
      }
    }
  }

  /*-------------------------------.
  | add(expression, expression).   |
  `-------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (add).
      template <typename ExpSetLhs, typename ExpSetRhs>
      expression
      add_expression(const expression& lhs, const expression& rhs)
      {
        auto join_elts = join<ExpSetLhs, ExpSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts),
                ::vcsn::add(std::get<0>(join_elts),
                            std::get<1>(join_elts), std::get<2>(join_elts))};
      }
    }
  }


  /*-------------------------------.
  | add(polynomial, polynomial).   |
  `-------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (add).
      template <typename PolynomialSetLhs, typename PolynomialSetRhs>
      polynomial
      add_polynomial(const polynomial& lhs, const polynomial& rhs)
      {
        auto join_elts = join<PolynomialSetLhs, PolynomialSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts),
                add(std::get<0>(join_elts),
                    std::get<1>(join_elts), std::get<2>(join_elts))};
      }
    }
  }


  /*----------------------.
  | add(weight, weight).  |
  `----------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (add).
      template <typename WeightSetLhs, typename WeightSetRhs>
      weight
      add_weight(const weight& lhs, const weight& rhs)
      {
        auto join_elts = join<WeightSetLhs, WeightSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts),
                add(std::get<0>(join_elts),
                    std::get<1>(join_elts), std::get<2>(join_elts))};
      }
    }
  }
}
