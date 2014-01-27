#ifndef VCSN_ALGOS_CONCATENATE_HH
# define VCSN_ALGOS_CONCATENATE_HH

# include <map>
# include <vector>

# include <vcsn/algos/copy.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/misc/raise.hh> // require

namespace vcsn
{
  /*------------------------------------.
  | concatenate(automaton, automaton).  |
  `------------------------------------*/

  template <typename A, typename B>
  A&
  concatenate_here(A& res, const B& b)
  {
    require(is_standard(res), __func__, ": lhs must be standard");
    require(is_standard(b), __func__, ": rhs must be standard");

    using automaton_t = A;
    auto ws = *res.context().weightset();

    // The set of the current (left-hand side) final transitions.
    auto ftr_ = res.final_transitions();
    // Store these transitions by copy.
    using transs_t = std::vector<typename automaton_t::transition_t>;
    transs_t ftr{ begin(ftr_), end(ftr_) };

    typename B::state_t b_initial = b.dst_of(b.initial_transitions().front());
    // State in B -> state in Res.
    // The initial state of b is not copied.
    std::map<typename B::state_t, typename A::state_t> m;
    m.emplace(b.post(), res.post());
    for (auto s: b.states())
      if (!b.is_initial(s))
        m.emplace(s, res.new_state());

    // Import all the B transitions, except the initial ones
    // and those from its (genuine) initial state.
    //
    // FIXME: provide generalized copy() that returns the map of
    // states orig -> copy.
    for (auto t: b.all_transitions())
      if (b.src_of(t) != b.pre() && b.src_of(t) != b_initial)
        res.new_transition(m[b.src_of(t)], m[b.dst_of(t)],
                           b.label_of(t), b.weight_of(t));

    // Branch all the final transitions of res to the successors of
    // b's initial state.
    for (auto t1: ftr)
      {
        // Remove the previous final transition first, as we might add
        // a final transition for the same state later.
        //
        // For instance on "{2}a+({3}\e+{5}a)", the final state s1 of
        // {2}a will be made final thanks to {3}\e.  So if we compute
        // the new transitions from s1 and then remove t1, we will
        // have removed the fact that s1 is final thanks to {3}\e.
        //
        // Besides, s1 will become final with weight {3}, which might
        // interfere with {5}a too.
        auto s1 = res.src_of(t1);
        auto w1 = res.weight_of(t1);
        res.del_transition(t1);
        for (auto t2: b.all_out(b_initial))
          res.set_transition(s1,
                             m[b.dst_of(t2)],
                             b.label_of(t2),
                             ws.mul(w1, b.weight_of(t2)));
      }
    return res;
  }

  /// Concatenate two standard automata.
  template <class A, class B>
  A
  concatenate(const A& lhs, const B& rhs)
  {
    require(is_standard(lhs), __func__, ": lhs must be standard");
    require(is_standard(rhs), __func__, ": rhs must be standard");

    using automaton_t = A;

    // Create new automata.
    auto ctx = join(lhs.context(), rhs.context());
    automaton_t res(ctx);
    ::vcsn::copy(lhs, res, {keep_all_states<typename automaton_t::state_t>});
    concatenate_here(res, rhs);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Lhs, typename Rhs>
      automaton
      concatenate(const automaton& lhs, const automaton& rhs)
      {
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return make_automaton(concatenate(l, r));
      }

      REGISTER_DECLARE(concatenate,
                        (const automaton&, const automaton&) -> automaton);
    }
  }

  /*--------.
  | chain.  |
  `--------*/

  /// Concatenate n-times the standard automaton A.
  template <typename Aut>
  Aut
  chain(const Aut& aut, size_t n)
  {
    Aut res(aut.context());
    auto s = res.new_state();
    res.set_initial(s);
    res.set_final(s);

    for (size_t i = 0; i < n; ++i)
      concatenate_here(res, aut);
    return res;
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Unsigned>
      automaton
      chain(const automaton& a, unsigned n)
      {
        const auto& aut = a->as<Aut>();
        return make_automaton(chain(aut, n));
      }

      REGISTER_DECLARE(chain,
                       (const automaton& aut, unsigned n) -> automaton);
    }
  }


  /*------------------------------.
  | concatenate(ratexp, ratexp).  |
  `------------------------------*/

  /// Concatenation/product of ratexps.
  template <typename RatExpSet>
  inline
  typename RatExpSet::ratexp_t
  concatenate(const RatExpSet& rs,
              const typename RatExpSet::ratexp_t& lhs,
              const typename RatExpSet::ratexp_t& rhs)
  {
    return rs.mul(lhs, rhs);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSetLhs, typename RatExpSetRhs>
      ratexp
      concatenate_ratexp(const ratexp& lhs, const ratexp& rhs)
      {
        const auto& l = lhs->as<RatExpSetLhs>();
        const auto& r = rhs->as<RatExpSetLhs>();
        return make_ratexp(l.get_ratexpset(),
                           concatenate<RatExpSetLhs>(l.get_ratexpset(),
                                                     l.ratexp(),
                                                     r.ratexp()));
      }

      REGISTER_DECLARE(concatenate_ratexp,
                       (const ratexp&, const ratexp&) -> ratexp);
    }
  }
}

#endif // !VCSN_ALGOS_CONCATENATE_HH
