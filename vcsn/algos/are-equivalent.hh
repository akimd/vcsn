#ifndef VCSN_ALGOS_ARE_EQUIVALENT_HH
# define VCSN_ALGOS_ARE_EQUIVALENT_HH

# include <deque>
# include <queue>
# include <map>

# include <vcsn/algos/accessible.hh>
# include <vcsn/algos/product.hh>
# include <vcsn/dyn/fwd.hh>

namespace vcsn
{

  /*-------------.
  | difference.  |
  `-------------*/

  /// An automaton that computes weights of \a lhs, but not by \a rhs.
  template <typename Rhs, typename Lhs>
  Rhs
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
    // Meet complement()'s requirements.
    const Aut1* aut1 = &a1;
    std::unique_ptr<Aut1> a1cd;
    if (!is_deterministic(a1))
      {
        a1cd.reset(new Aut1{determinize(a1, true)});
        aut1 = a1cd.get();
      }
    else if (!is_complete(a1))
      {
        a1cd.reset(new Aut1{complete(a1)});
        aut1 = a1cd.get();
      }

    const Aut2* aut2 = &a2;
    std::unique_ptr<Aut2> a2cd;
    if (!is_deterministic(a2))
      {
        a2cd.reset(new Aut2{determinize(a2, true)});
        aut2 = a2cd.get();
      }
    else if (!is_complete(a2))
      {
        a2cd.reset(new Aut2{complete(a2)});
        aut2 = a2cd.get();
      }

    return (is_useless(product(*aut1, complement(*aut2)))
            && is_useless(product(*aut2, complement(*aut1))));
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
