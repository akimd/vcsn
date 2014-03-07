#ifndef VCSN_ALGOS_UNION_HH
# define VCSN_ALGOS_UNION_HH

# include <unordered_map>

# include <vcsn/core/mutable_automaton.hh>
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
    const auto& ls = *res.labelset();
    const auto& bls = *b.labelset();
    const auto& ws = *res.weightset();
    const auto& bws = *b.weightset();
    // State in B -> state in Res.
    std::unordered_map<typename B::state_t, typename A::state_t> m;
    for (auto s: b.states())
      m.emplace(s, res.new_state());
    m.emplace(b.pre(), res.pre());
    m.emplace(b.post(), res.post());

    for (auto t: b.all_transitions())
      res.new_transition(m[b.src_of(t)], m[b.dst_of(t)],
                         ls.conv(bls, b.label_of(t)),
                         ws.conv(bws, b.weight_of(t)));
    return res;
  }

  template <typename A, typename B>
  mutable_automaton<join_t<typename A::context_t, typename B::context_t>>
  union_a(const A& laut, const B& raut)
  {
    using automaton_t
      = mutable_automaton<join_t<typename A::context_t, typename B::context_t>>;

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
