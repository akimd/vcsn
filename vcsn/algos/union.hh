#ifndef VCSN_ALGOS_UNION_HH
# define VCSN_ALGOS_UNION_HH

# include <unordered_map>

# include <vcsn/algos/copy.hh>
# include <vcsn/algos/product.hh> // join_automata
# include <vcsn/core/mutable-automaton.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton

namespace vcsn
{
  /*----------.
  | union_a.  |
  `----------*/

  /// Merge transitions of \a b into those of \a res.
  ///
  /// \pre AutIn <: AutOut.
  template <typename A, typename B>
  inline
  A&
  union_here(A& res, const B& b)
  {
    ::vcsn::copy_into(b, res);
    return res;
  }

  /// Union of two automata.
  template <typename A, typename B>
  inline
  auto
  union_a(const A& lhs, const B& rhs)
    -> decltype(join_automata(lhs, rhs))
  {
    auto res = join_automata(lhs, rhs);
    union_here(res, lhs);
    union_here(res, rhs);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /*---------------.
      | dyn::union_a.  |
      `---------------*/

      template <typename Lhs, typename Rhs>
      inline
      automaton
      union_a(const automaton& lhs, const automaton& rhs)
      {
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return make_automaton(::vcsn::union_a(l, r));
      }

      REGISTER_DECLARE(union_a,
                       (const automaton&, const automaton&) -> automaton);
    }
  }
}

#endif // !VCSN_ALGOS_UNION_HH
