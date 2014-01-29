#ifndef VCSN_ALGOS_ARE_EQUIVALENT_HH
# define VCSN_ALGOS_ARE_EQUIVALENT_HH

# include <vcsn/algos/complement.hh>
# include <vcsn/algos/complete.hh>
# include <vcsn/algos/determinize.hh>
# include <vcsn/algos/product.hh>
# include <vcsn/dyn/fwd.hh>

namespace vcsn
{

  /*---------------------------------------.
  | are_equivalent(automaton, automaton).  |
  `---------------------------------------*/

  template <typename Aut1, typename Aut2>
  bool
  are_equivalent(const Aut1& a1, const Aut2& a2)
  {
    return (is_useless(difference(a1, a2))
            && is_useless(difference(a2, a1)));
  }

  namespace dyn
  {
    namespace detail
    {

      /// Bridge.
      template <typename Aut1, typename Aut2>
      bool
      are_equivalent(const automaton& aut1, const automaton& aut2)
      {
        const auto& a1 = aut1->as<Aut1>();
        const auto& a2 = aut2->as<Aut2>();
        return are_equivalent(a1, a2);
      }

      REGISTER_DECLARE(are_equivalent,
                        (const automaton&, const automaton&) -> bool);
    }
  }


  /*-----------------------------------.
  | difference(automaton, automaton).  |
  `-----------------------------------*/

  /// An automaton that computes weights of \a lhs, but not by \a rhs.
  /// The return type is really the Lhs one.
  template <typename Lhs, typename Rhs>
  Lhs
  difference(const Lhs& lhs, const Rhs& rhs)
  {
    // Meet complement()'s requirements.
    const Rhs* r = &rhs;
    std::unique_ptr<Rhs> rhscd;
    if (!is_deterministic(rhs))
      {
        rhscd.reset(new Rhs{determinize(rhs, true)});
        r = rhscd.get();
      }
    else if (!is_complete(rhs))
      {
        rhscd.reset(new Rhs{complete(rhs)});
        r = rhscd.get();
      }
    return product(lhs, complement(*r));
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Lhs, typename Rhs>
      automaton
      difference(const automaton& lhs, const automaton& rhs)
      {
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return make_automaton(difference(l, r));
      }

      REGISTER_DECLARE(difference,
                        (const automaton&, const automaton&) -> automaton);
    }
  }

  /*-----------------------------.
  | difference(ratexp, ratexp).  |
  `-----------------------------*/

  /// Difference of ratexps.
  template <typename RatExpSet>
  inline
  typename RatExpSet::ratexp_t
  difference(const RatExpSet& rs,
             const typename RatExpSet::ratexp_t& lhs,
             const typename RatExpSet::ratexp_t& rhs)
  {
    return rs.intersection(lhs, rs.complement(rhs));
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSetLhs, typename RatExpSetRhs>
      ratexp
      difference_ratexp(const ratexp& lhs, const ratexp& rhs)
      {
        const auto& l = lhs->as<RatExpSetLhs>();
        const auto& r = rhs->as<RatExpSetLhs>();
        return make_ratexp(l.get_ratexpset(),
                           ::vcsn::difference<RatExpSetLhs>(l.get_ratexpset(),
                                                            l.ratexp(),
                                                            r.ratexp()));
      }

      REGISTER_DECLARE(difference_ratexp,
                       (const ratexp&, const ratexp&) -> ratexp);
    }
  }
}

#endif // !VCSN_ALGOS_ARE_EQUIVALENT_HH
