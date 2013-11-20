#ifndef VCSN_ALGOS_UNION_HH
# define VCSN_ALGOS_UNION_HH

# include <vcsn/dyn/automaton.hh> // dyn::make_automaton

namespace vcsn
{
  /*----------.
  | union_a.  |
  `----------*/

  /// Merge transitions of \a b into those of \a res.
  ///
  /// \precondition The context of \a res must include that of \a b.
  template <typename A, typename B>
  A&
  union_here(A& res, const B& b)
  {
    // State in B -> state in Res.
    std::map<typename B::state_t, typename A::state_t> m;
    for (auto s: b.states())
      m.emplace(s, res.new_state());
    m.emplace(b.pre(), res.pre());
    m.emplace(b.post(), res.post());

    for (auto t: b.all_transitions())
      res.new_transition(m[b.src_of(t)], m[b.dst_of(t)],
                         b.label_of(t), b.weight_of(t));
    return res;
  }

  template <class A, class B>
  A
  union_a(const A& laut, const B& raut)
  {
    using automaton_t = A;

    // Create new automata.
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
