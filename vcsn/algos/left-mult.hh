#ifndef VCSN_ALGOS_LEFT_MULT_HH
# define VCSN_ALGOS_LEFT_MULT_HH

# include <vcsn/algos/copy.hh>
# include <vcsn/algos/standard.hh>
# include <vcsn/core/rat/ratexpset.hh>
# include <vcsn/ctx/traits.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/dyn/ratexp.hh>
# include <vcsn/dyn/weight.hh>
# include <vcsn/misc/raise.hh>

namespace vcsn
{

  namespace detail
  {
    template <typename Aut>
    struct standard_operations
    {
      using automaton_t = Aut;
      using context_t = context_t_of<automaton_t>;
      using weight_t = weight_t_of<context_t>;
      using weightset_t = weightset_t_of<context_t>;
      using state_t = state_t_of<automaton_t>;

      /// Left-multiplication by a weight.
      static automaton_t&
      left_mult_here(const weight_t& w, automaton_t& res)
      {
        weightset_t ws(*res->context().weightset());
        if (ws.is_zero(w))
          zero_here(res);
        else if (ws.is_one(w))
          {}
        else if (is_standard(res))
          {
            state_t initial = res->dst_of(res->initial_transitions().front());
            for (auto t: res->all_out(initial))
              res->lmul_weight(t, w);
          }
        else
          for (auto t: res->initial_transitions())
            res->lmul_weight(t, w);
        return res;
      }

      /// Right-multiplication by a weight.
      static automaton_t&
      right_mult_here(automaton_t& res, const weight_t& w)
      {
        weightset_t ws(*res->context().weightset());
        if (ws.is_zero(w))
          zero_here(res);
        else if (ws.is_one(w))
          {}
        else
          for (auto t: res->final_transitions())
            res->rmul_weight(t, w);
        return res;
      }

      /// Transform \a res into the (standard) empty automaton.
      static automaton_t&
      zero_here(automaton_t& res)
      {
        automaton_t a = make_shared_ptr<automaton_t>(res->context());
        a->set_initial(a->new_state());
        res = std::move(a);
        return res;
      }
    };
  }

  /*-----------------------.
  | left-mult(automaton).  |
  `-----------------------*/

  template <typename Aut>
  inline
  Aut&
  left_mult_here(const weight_t_of<Aut>& w, Aut& res)
  {
    return detail::standard_operations<Aut>::left_mult_here(w, res);
  }

  template <typename AutIn,
            typename AutOut = typename AutIn::element_type::automaton_nocv_t>
  inline
  AutOut
  left_mult(const weight_t_of<AutOut>& w, const AutIn& aut)
  {
    auto res = copy<AutIn, AutOut>(aut);
    left_mult_here(w, res);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename WeightSet, typename Aut>
      automaton
      left_mult(const weight& weight, const automaton& aut)
      {
        const auto& a1 = aut->as<Aut>();
        const auto& w1 = weight->as<WeightSet>();
        // FIXME: this is hairy because there is no elegant means (so
        // far) to copy an automaton to a supertype, because the
        // incoming context is not automatically converted to the
        // supertype by vcsn::copy.
        auto ctx = make_context(*a1->labelset(),
                                join(w1.weightset(), *a1->weightset()));
        auto a2 = make_mutable_automaton(ctx);
        copy_into(a1, a2);
        auto w2 = ctx.weightset()->conv(w1.weightset(), w1.weight());
        return make_automaton(::vcsn::left_mult_here(w2, a2));
      }

      REGISTER_DECLARE(left_mult,
                       (const weight&, const automaton&) -> automaton);

    }
  }


  /*--------------------.
  | left-mult(ratexp).  |
  `--------------------*/

  template <typename RatExpSet>
  inline
  typename RatExpSet::value_t
  left_mult(const RatExpSet& rs,
            const weight_t_of<RatExpSet>& w,
            const typename RatExpSet::value_t& r)
  {
    return rs.lmul(w, r);
  }

  /// Join between a ratexpset and a weightset.
  ///
  /// We must not perform a plain
  ///
  /// join(w1.weightset(), r1.ratexpset())
  ///
  /// here.  Consider for instance
  ///
  /// ratexpset<lal(abc), ratexpset<law(xyz), b>>
  ///
  /// we would perform
  ///
  /// join(ratexpset<law(xyz), b>,
  ///      ratexpset<lal(abc), ratexpset<law(xyz), b>>)
  ///
  /// i.e., a join of contexts which applies to both labelsets
  /// (here, join(lal(abc), "law(xyz)) = law(abcxyz)") and
  /// weightsets.  Here, the "ratexpset<law(xyz), b>" must really
  /// be considered as a weightset, so compute the join of
  /// weightsets by hand, and leave the labelset alone.
  template <typename WeightSet, typename RatExpSet>
  ratexpset<context<labelset_t_of<RatExpSet>,
                    join_t<WeightSet, weightset_t_of<RatExpSet>>>>
  join_weightset_ratexpset(const WeightSet& ws,
                           const RatExpSet& rs)
  {
    auto ctx = make_context(*rs.labelset(), join(ws, *rs.weightset()));
    return make_ratexpset(ctx, rs.identities());
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename WeightSet, typename RatExpSet>
      ratexp
      left_mult_ratexp(const weight& weight, const ratexp& exp)
      {
        const auto& w1 = weight->as<WeightSet>();
        const auto& r1 = exp->as<RatExpSet>();
        auto rs = join_weightset_ratexpset(w1.weightset(), r1.ratexpset());
        auto w2 = rs.weightset()->conv(w1.weightset(), w1.weight());
        auto r2 = rs.conv(r1.ratexpset(), r1.ratexp());
        return make_ratexp(rs,
                           ::vcsn::left_mult(rs, w2, r2));
      }

      REGISTER_DECLARE(left_mult_ratexp,
                       (const weight&, const ratexp&) -> ratexp);

    }
  }

  /*------------------------.
  | right-mult(automaton).  |
  `------------------------*/

  template <typename Aut>
  inline
  Aut&
  right_mult_here(Aut& res, const weight_t_of<Aut>& w)
  {
    return detail::standard_operations<Aut>::right_mult_here(res, w);
  }

  template <typename Aut>
  inline
  typename Aut::element_type::automaton_nocv_t
  right_mult(const Aut& aut, const weight_t_of<Aut>& w)
  {
    auto res = copy(aut);
    right_mult_here(res, w);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename WeightSet>
      automaton
      right_mult(const automaton& aut, const weight& weight)
      {
        const auto& a1 = aut->as<Aut>();
        const auto& w1 = weight->as<WeightSet>();
        // FIXME: see comment for left_mult.
        auto ctx = make_context(*a1->labelset(),
                                join(*a1->weightset(), w1.weightset()));
        auto a2 = make_mutable_automaton(ctx);
        copy_into(a1, a2);
        auto w2 = ctx.weightset()->conv(w1.weightset(), w1.weight());
        return make_automaton(::vcsn::right_mult_here(a2, w2));
      }

      REGISTER_DECLARE(right_mult,
                       (const automaton&, const weight&) -> automaton);
    }
  }

  /*---------------------.
  | right-mult(ratexp).  |
  `---------------------*/

  template <typename RatExpSet>
  inline
  typename RatExpSet::value_t
  right_mult(const RatExpSet& rs,
             const typename RatExpSet::value_t& r,
             const weight_t_of<RatExpSet>& w)
  {
    return rs.rmul(r, w);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSet, typename WeightSet>
      ratexp
      right_mult_ratexp(const ratexp& exp, const weight& weight)
      {
        const auto& w1 = weight->as<WeightSet>();
        const auto& r1 = exp->as<RatExpSet>();
        auto rs = join_weightset_ratexpset(w1.weightset(), r1.ratexpset());
        auto w2 = rs.weightset()->conv(w1.weightset(), w1.weight());
        auto r2 = rs.conv(r1.ratexpset(), r1.ratexp());
        return make_ratexp(rs,
                           ::vcsn::right_mult(rs, r2, w2));
      }

      REGISTER_DECLARE(right_mult_ratexp,
                       (const ratexp&, const weight&) -> ratexp);

    }
  }
}

#endif // !VCSN_ALGOS_LEFT_MULT_HH
