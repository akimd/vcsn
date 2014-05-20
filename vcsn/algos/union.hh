#ifndef VCSN_ALGOS_UNION_HH
# define VCSN_ALGOS_UNION_HH

# include <unordered_map>

# include <vcsn/algos/copy.hh>
# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton

namespace vcsn
{
  /*----------.
  | union_a.  |
  `----------*/

  /// Merge transitions of \a b into those of \a res.
  ///
  /// \precondition AutIn <: AutOut.
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
  mutable_automaton<join_t<context_t_of<A>, context_t_of<B>>>
  union_a(const A& laut, const B& raut)
  {
    using automaton_t
      = mutable_automaton<join_t<context_t_of<A>, context_t_of<B>>>;
    // Create new automaton.
    auto ctx = join(laut.context(), raut.context());
    automaton_t res(ctx);

    union_here(res, laut);
    union_here(res, raut);

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
        return make_automaton(union_a(l, r));
      }

      REGISTER_DECLARE(union_a,
                       (const automaton&, const automaton&) -> automaton);
    }
  }
}

#endif // !VCSN_ALGOS_UNION_HH
