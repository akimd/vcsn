#ifndef VCSN_ALGOS_SUM_HH
# define VCSN_ALGOS_SUM_HH

# include <map>

# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/dyn/ratexp.hh> // dyn::make_ratexp
# include <vcsn/misc/raise.hh> // require

namespace vcsn
{

  /*----------------------------.
  | sum(automaton, automaton).  |
  `----------------------------*/

  /// Merge transitions of \a b into those of \a res.
  ///
  /// \precondition The context of \a res must include that of \a b.
  /// \precondition res and b must be standard.
  template <typename A, typename B>
  A&
  sum_here(A& res, const B& b)
  {
    require(is_standard(res), __func__, ": lhs must be standard");
    require(is_standard(b), __func__, ": rhs must be standard");

    // State in B -> state in Res.
    std::map<typename B::state_t, typename A::state_t> m;
    typename A::state_t initial = res.dst_of(res.initial_transitions().front());
    for (auto s: b.states())
      m.emplace(s, b.is_initial(s) ? initial : res.new_state());
    m.emplace(b.pre(), res.pre());
    m.emplace(b.post(), res.post());

    // Add b.
    for (auto t: b.all_transitions())
      // Do not add initial transitions, the unique initial state is
      // already declared as such, and its weight must remain 1.
      if (b.src_of(t) != b.pre())
        {
          if (b.dst_of(t) == b.post())
            res.add_transition(m[b.src_of(t)], m[b.dst_of(t)],
                               b.label_of(t), b.weight_of(t));
          else
            res.new_transition(m[b.src_of(t)], m[b.dst_of(t)],
                               b.label_of(t), b.weight_of(t));
        }
    return res;
  }


  template <class A, class B>
  A
  sum(const A& laut, const B& raut)
  {
    // Sum only works on standard automata.
    using automaton_t = A;

    // Create new automata.
    auto ctx = join(laut.context(), raut.context());
    automaton_t res(ctx);
    // A standard automaton has a single initial state.
    res.set_initial(res.new_state());

    sum_here(res, laut);
    sum_here(res, raut);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Lhs, typename Rhs>
      automaton
      sum(const automaton& lhs, const automaton& rhs)
      {
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return make_automaton(sum(l, r));
      }

      REGISTER_DECLARE(sum,
                       (const automaton&, const automaton&) -> automaton);
    }
  }

  /*----------------------.
  | sum(ratexp, ratexp).  |
  `----------------------*/

  /// Sum of ratexps.
  template <typename RatExpSet>
  inline
  typename RatExpSet::ratexp_t
  sum(const RatExpSet& rs,
      const typename RatExpSet::ratexp_t& lhs,
      const typename RatExpSet::ratexp_t& rhs)
  {
    return rs.add(lhs, rhs);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSetLhs, typename RatExpSetRhs>
      ratexp
      sum_ratexp(const ratexp& lhs, const ratexp& rhs)
      {
        const auto& l = lhs->as<RatExpSetLhs>();
        const auto& r = rhs->as<RatExpSetRhs>();
        auto rs = join(l.get_ratexpset(), r.get_ratexpset());
        auto lr = rs.conv(l.get_ratexpset(), l.ratexp());
        auto rr = rs.conv(r.get_ratexpset(), r.ratexp());
        return make_ratexp(rs, sum(rs, lr, rr));
      }

      REGISTER_DECLARE(sum_ratexp,
                       (const ratexp&, const ratexp&) -> ratexp);
    }
  }
}

#endif // !VCSN_ALGOS_SUM_HH
