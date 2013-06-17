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

  template <typename Aut1, typename Aut2>
  bool
  are_equivalent(const Aut1& a1, const Aut2& a2)
  {
    // Meet complement()'s requirements.
    const Aut1* aut1 = &a1;
    std::unique_ptr<Aut1> a1cd;
    if (!is_deterministic(a1))
      {
        a1cd.reset(new Aut1{determinize(a1)});
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
        a2cd.reset(new Aut2{determinize(a2)});
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

      /*----------------------.
      | dyn::are_equivalent.  |
      `----------------------*/

      template <typename Aut1, typename Aut2>
      bool
      are_equivalent(const automaton& aut1, const automaton& aut2)
      {
        const auto& a1 = dynamic_cast<const Aut1&>(*aut1);
        const auto& a2 = dynamic_cast<const Aut2&>(*aut2);
        return are_equivalent(a1, a2);
      }

      using are_equivalent_t =
        auto (const automaton& lhs, const automaton& rhs) -> bool;
      bool are_equivalent_register(const std::string& lctx,
                                   const std::string& rctx,
                                   are_equivalent_t fn);
    }
  }
}

#endif // !VCSN_ALGOS_ARE_EQUIVALENT_HH
