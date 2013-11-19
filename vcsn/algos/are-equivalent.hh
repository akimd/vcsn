#ifndef VCSN_ALGOS_ARE_EQUIVALENT_HH
# define VCSN_ALGOS_ARE_EQUIVALENT_HH

# include <vcsn/algos/complement.hh>
# include <vcsn/algos/complete.hh>
# include <vcsn/algos/determinize.hh>
# include <vcsn/algos/product.hh>
# include <vcsn/dyn/fwd.hh>

namespace vcsn
{

  /*-------------.
  | difference.  |
  `-------------*/

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

      template <typename Lhs, typename Rhs>
      automaton
      difference(const automaton& lhs, const automaton& rhs)
      {
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return make_automaton(difference(l, r));
      }

      REGISTER_DECLARE2(difference,
                        (const automaton&, const automaton&) -> automaton);
    }
  }

  /*-----------------.
  | are_equivalent.  |
  `-----------------*/

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

      REGISTER_DECLARE2(are_equivalent,
                        (const automaton&, const automaton&) -> bool);
    }
  }
}

#endif // !VCSN_ALGOS_ARE_EQUIVALENT_HH
