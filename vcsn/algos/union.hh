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
    for (auto t: b.states())
      m.emplace(t, res.new_state());
    m.emplace(b.pre(), res.pre());
    m.emplace(b.post(), res.post());

    for (auto t: b.all_transitions())
      res.add_transition(m[b.src_of(t)], m[b.dst_of(t)],
                         b.label_of(t), b.weight_of(t));
    return res;
  }

  template <class A, class B>
  A
  union_a(const A& laut, const B& raut)
  {
    using automaton_t = A;
    using context_t = typename automaton_t::context_t;

    // Create new automata.
    auto gs = get_union(*laut.context().labelset(),
                        *raut.context().labelset());
    auto ls = std::make_shared<typename automaton_t::labelset_t>(gs);
    auto ctx = context_t{ls, laut.context().weightset()};
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
        return make_automaton(l.context(), union_a(l, r));
      }

      using union_a_t =
        auto (const automaton& lhs, const automaton& rhs) -> automaton;
      bool union_a_register(const std::string& lctx, const std::string& rctx,
                            union_a_t fn);
    }
  }
}

#endif // !VCSN_ALGOS_UNION_HH
