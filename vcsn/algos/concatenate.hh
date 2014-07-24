#ifndef VCSN_ALGOS_CONCATENATE_HH
# define VCSN_ALGOS_CONCATENATE_HH

# include <unordered_map>
# include <vector>

# include <vcsn/algos/copy.hh>
# include <vcsn/algos/product.hh> // join_automata
# include <vcsn/algos/standard.hh>
# include <vcsn/algos/sum.hh>
# include <vcsn/core/mutable-automaton.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/dyn/polynomial.hh>
# include <vcsn/dyn/weight.hh>
# include <vcsn/misc/raise.hh> // require

namespace vcsn
{
  /*------------------------------------.
  | concatenate(automaton, automaton).  |
  `------------------------------------*/

  /// Append automaton \a b to \a res.
  ///
  /// \pre The context of \a res must include that of \a b.
  /// \pre both are standard.
  template <typename A, typename B>
  A&
  concatenate_here(A& res, const B& b)
  {
    require(is_standard(res), __func__, ": lhs must be standard");
    require(is_standard(b), __func__, ": rhs must be standard");

    using automaton_t = A;
    const auto& ls = *res->labelset();
    const auto& bls = *b->labelset();
    const auto& ws = *res->weightset();
    const auto& bws = *b->weightset();

    // The set of the current (left-hand side) final transitions.
    auto ftr_ = res->final_transitions();
    // Store these transitions by copy.
    using transs_t = std::vector<transition_t_of<automaton_t>>;
    transs_t ftr{ begin(ftr_), end(ftr_) };

    state_t_of<B> b_initial = b->dst_of(b->initial_transitions().front());
    // State in B -> state in Res.
    // The initial state of b is not copied.
    std::unordered_map<state_t_of<B>, state_t_of<A>> m;
    m.emplace(b->post(), res->post());
    for (auto s: b->states())
      if (!b->is_initial(s))
        m.emplace(s, res->new_state());

    // Import all the B transitions, except the initial ones
    // and those from its (genuine) initial state.
    //
    // FIXME: provide generalized copy() that returns the map of
    // states orig -> copy.
    for (auto t: b->all_transitions())
      if (b->src_of(t) != b->pre() && b->src_of(t) != b_initial)
        res->new_transition(m[b->src_of(t)], m[b->dst_of(t)],
                           ls.conv(bls, b->label_of(t)),
                           ws.conv(bws, b->weight_of(t)));

    // Branch all the final transitions of res to the successors of
    // b's initial state.
    for (auto t1: ftr)
      {
        // Remove the previous final transition first, as we might add
        // a final transition for the same state later.
        //
        // For instance on "{2}a+({3}\e+{5}a)", the final state s1 of
        // {2}a will be made final thanks to {3}\e.  So if we compute
        // the new transitions from s1 and then remove t1, we will
        // have removed the fact that s1 is final thanks to {3}\e.
        //
        // Besides, s1 will become final with weight {3}, which might
        // interfere with {5}a too.
        auto s1 = res->src_of(t1);
        auto w1 = res->weight_of(t1);
        res->del_transition(t1);
        for (auto t2: b->all_out(b_initial))
          res->set_transition(s1,
                             m[b->dst_of(t2)],
                             ls.conv(bls, b->label_of(t2)),
                             ws.mul(w1,
                                    ws.conv(bws, b->weight_of(t2))));
      }
    return res;
  }

  /// Concatenate two standard automata.
  template <typename A, typename B>
  inline
  auto
  concatenate(const A& lhs, const B& rhs)
    -> decltype(join_automata(lhs, rhs))
  {
    require(is_standard(lhs), __func__, ": lhs must be standard");
    require(is_standard(rhs), __func__, ": rhs must be standard");
    auto res = join_automata(lhs, rhs);
    ::vcsn::copy_into(lhs, res);
    concatenate_here(res, rhs);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Lhs, typename Rhs>
      automaton
      concatenate(const automaton& lhs, const automaton& rhs)
      {
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return make_automaton(::vcsn::concatenate(l, r));
      }

      REGISTER_DECLARE(concatenate,
                       (const automaton&, const automaton&) -> automaton);
    }
  }

  /*-----------------------------.
  | chain(automaton, min, max).  |
  `-----------------------------*/

  template <typename Aut>
  Aut
  chain(const Aut& aut, int min, int max)
  {
    Aut res =
      make_shared_ptr<Aut>(aut->context());
    if (max == -1)
      {
        res = star(aut);
        if (min != -1)
          res = concatenate(chain(aut, min, min), res);
      }
    else
      {
        if (min == -1)
          min = 0;
        if (min == 0)
          {
            // automatonset::one().
            auto s = res->new_state();
            res->set_initial(s);
            res->set_final(s);
          }
        else
          {
            res = vcsn::copy(aut);
            for (int n = 1; n < min; ++n)
              res = concatenate(res, aut);
          }
        if (min < max)
          {
            // Aut sum = automatonset.one();
            Aut sum = make_shared_ptr<Aut>(aut->context());
            {
              auto s = sum->new_state();
              sum->set_initial(s);
              sum->set_final(s);
            }
            for (int n = 1; n <= max - min; ++n)
              sum = vcsn::sum(sum, chain(aut, n, n));
            res = vcsn::concatenate(res, sum);
          }
      }
    return res;
  }



  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Int1, typename Int2>
      automaton
      chain(const automaton& a, int min, int max)
      {
        const auto& aut = a->as<Aut>();
        return make_automaton(::vcsn::chain(aut, min, max));
      }

      REGISTER_DECLARE(chain,
                       (const automaton& aut, int min, int max) -> automaton);
    }
  }


  /*------------------------------.
  | concatenate(ratexp, ratexp).  |
  `------------------------------*/

  /// Concatenation/product of polynomials/ratexps.
  template <typename ValueSet>
  inline
  typename ValueSet::value_t
  concatenate(const ValueSet& vs,
              const typename ValueSet::value_t& lhs,
              const typename ValueSet::value_t& rhs)
  {
    return vs.mul(lhs, rhs);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSetLhs, typename RatExpSetRhs>
      ratexp
      concatenate_ratexp(const ratexp& lhs, const ratexp& rhs)
      {
        const auto& l = lhs->as<RatExpSetLhs>();
        const auto& r = rhs->as<RatExpSetRhs>();
        auto rs = join(l.ratexpset(), r.ratexpset());
        auto lr = rs.conv(l.ratexpset(), l.ratexp());
        auto rr = rs.conv(r.ratexpset(), r.ratexp());
        return make_ratexp(rs, ::vcsn::concatenate(rs, lr, rr));
      }

      REGISTER_DECLARE(concatenate_ratexp,
                       (const ratexp&, const ratexp&) -> ratexp);
    }
  }


  /*--------------------------.
  | chain(ratexp, min, max).  |
  `--------------------------*/

  template <typename RatExpSet>
  typename RatExpSet::ratexp_t
  chain(const RatExpSet& rs, const typename RatExpSet::ratexp_t& r,
        int min, int max)
  {
    typename RatExpSet::ratexp_t res;
    if (max == -1)
      {
        res = rs.star(r);
        if (min != -1)
          res = rs.mul(chain(rs, r, min, min), res);
      }
    else
      {
        if (min == -1)
          min = 0;
        if (min == 0)
          res = rs.one();
        else
          {
            res = r;
            for (int n = 1; n < min; ++n)
              res = rs.mul(res, r);
          }
        if (min < max)
          {
            typename RatExpSet::ratexp_t sum = rs.one();
            for (int n = 1; n <= max - min; ++n)
              sum = rs.add(sum, chain(rs, r, n, n));
            res = rs.mul(res, sum);
          }
      }
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSet, typename Int1, typename Int2>
      ratexp
      chain_ratexp(const ratexp& re, int min, int max)
      {
        const auto& r = re->as<RatExpSet>();
        return make_ratexp(r.ratexpset(),
                           ::vcsn::chain(r.ratexpset(), r.ratexp(), min, max));
      }

      REGISTER_DECLARE(chain_ratexp,
                       (const ratexp&, int, int) -> ratexp);
    }
  }


  /*------------------------------.
  | mul(polynomial, polynomial).  |
  `------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename PolynomialSetLhs, typename PolynomialSetRhs>
      polynomial
      concatenate_polynomial(const polynomial& lhs, const polynomial& rhs)
      {
        const auto& l = lhs->as<PolynomialSetLhs>();
        const auto& r = rhs->as<PolynomialSetRhs>();
        auto rs = join(l.polynomialset(), r.polynomialset());
        auto lr = rs.conv(l.polynomialset(), l.polynomial());
        auto rr = rs.conv(r.polynomialset(), r.polynomial());
        return make_polynomial(rs, concatenate(rs, lr, rr));
      }

      REGISTER_DECLARE(concatenate_polynomial,
                       (const polynomial&, const polynomial&) -> polynomial);
    }
  }

  /*----------------------.
  | mul(weight, weight).  |
  `----------------------*/

  /// Product of weights.
  template <typename ValueSet>
  inline
  typename ValueSet::value_t
  multiply(const ValueSet& vs,
           const typename ValueSet::value_t& lhs,
           const typename ValueSet::value_t& rhs)
  {
    return vs.mul(lhs, rhs);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename WeightSetLhs, typename WeightSetRhs>
      weight
      multiply_weight(const weight& lhs, const weight& rhs)
      {
        const auto& l = lhs->as<WeightSetLhs>();
        const auto& r = rhs->as<WeightSetRhs>();
        auto rs = join(l.weightset(), r.weightset());
        auto lr = rs.conv(l.weightset(), l.weight());
        auto rr = rs.conv(r.weightset(), r.weight());
        return make_weight(rs, multiply(rs, lr, rr));
      }

      REGISTER_DECLARE(multiply_weight,
                       (const weight&, const weight&) -> weight);
    }
  }
}

#endif // !VCSN_ALGOS_CONCATENATE_HH
