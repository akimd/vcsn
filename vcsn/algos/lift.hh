#ifndef VCSN_ALGOS_LIFT_HH
# define VCSN_ALGOS_LIFT_HH

# include <map>

# include <vcsn/ctx/context.hh>
# include <vcsn/labelset/oneset.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/core/mutable-automaton.hh>
# include <vcsn/core/rat/ratexpset.hh>
# include <vcsn/core/rat/ratexp.hh>

namespace vcsn
{

  namespace detail
  {
    /*--------------.
    | lift(types).  |
    `--------------*/

    template <typename ValueSet>
    struct context_of
    {
      using type = typename ValueSet::context_t;
    };

    template <typename Context>
    struct context_of<vcsn::mutable_automaton<Context>>
    {
      using type = Context;
    };

    template <typename ValueSet>
    using context_of_t = typename context_of<ValueSet>::type;

    template <typename Context>
    using lifted_context_t =
      context<oneset, ratexpset<Context>>;

    template <typename Aut>
    using lifted_automaton_t =
      mutable_automaton<lifted_context_t<context_t_of<Aut>>>;

    template <typename RatExpSet>
    using lifted_ratexpset_t =
      ratexpset<lifted_context_t<context_t_of<RatExpSet>>>;

    // lift(ctx) -> ctx
    template <typename LabelSet, typename WeightSet>
    lifted_context_t<context<LabelSet, WeightSet>>
    lift_context(const context<LabelSet, WeightSet>& ctx)
    {
      auto rs_in = ratexpset<context<LabelSet, WeightSet>>(ctx);
      return {oneset{}, rs_in};
    }

    // lift(ratexpset) -> ratexpset
    template <typename Context>
    lifted_ratexpset_t<ratexpset<Context>>
    lift_ratexpset(const ratexpset<Context>& rs)
    {
      return {lift_context(rs.context())};
    }

  }

  /*------------------.
  | lift(automaton).  |
  `------------------*/

  template <typename AutPtr>
  inline
  detail::lifted_automaton_t<AutPtr>
  lift(const AutPtr& a)
  {
    using auto_in_t = typename AutPtr::element_type;
    using ctx_in_t = context_t_of<auto_in_t>;
    using state_in_t = state_t_of<auto_in_t>;

    // Produce RatExps of the same context as the original automaton.
    using rs_in_t = ratexpset<ctx_in_t>;
    rs_in_t rs_in{a->context()};

    auto ctx_out = detail::lift_context(a->context());
    using auto_out_t = detail::lifted_automaton_t<auto_in_t>;
    using state_out_t = state_t_of<auto_out_t>;
    auto_out_t res = std::make_shared<typename auto_out_t::element_type>(ctx_out);
    std::map<state_in_t, state_out_t> map;
    map[a->pre()] = res->pre();
    map[a->post()] = res->post();
    for (auto s: a->states())
      map[s] = res->new_state();

    for (auto t: a->all_transitions())
      if (a->src_of(t) == a->pre())
        res->add_initial(map[a->dst_of(t)],
                        rs_in.lmul(a->weight_of(t), rs_in.one()));
      else if (a->dst_of(t) == a->post())
        res->add_final(map[a->src_of(t)],
                      rs_in.lmul(a->weight_of(t), rs_in.one()));
      else
        res->add_transition
          (map[a->src_of(t)], map[a->dst_of(t)],
           {},
           rs_in.lmul(a->weight_of(t), rs_in.atom(a->label_of(t))));
    return res;
  }


  namespace dyn
  {
    namespace detail
    {
      template <typename Aut>
      automaton
      lift_automaton(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(lift(a));
      }

      REGISTER_DECLARE(lift_automaton,
                       (const automaton& aut) -> automaton);
    }
  }


  /*---------------.
  | lift(ratexp).  |
  `---------------*/

  namespace detail
  {
    template <typename Exp>
    using lifted_ratexp_t =
      typename lifted_context_t<context_t_of<Exp>>::ratexp_t;
  }

  template <typename RatExpSet>
  inline
  typename detail::lifted_ratexpset_t<RatExpSet>::ratexp_t
  lift(const RatExpSet& rs, const typename RatExpSet::ratexp_t& e)
  {
    auto lrs = detail::lift_ratexpset(rs);
    return lrs.lmul(e, lrs.one());
  }


  namespace dyn
  {
    namespace detail
    {
      /*--------------------.
      | dyn::lift(ratexp).  |
      `--------------------*/

      /// Bridge.
      template <typename RatExpSet>
      ratexp
      lift_ratexp(const ratexp& exp)
      {
        const auto& e = exp->as<RatExpSet>();
        return make_ratexp(::vcsn::detail::lift_ratexpset(e.ratexpset()),
                           ::vcsn::lift(e.ratexpset(), e.ratexp()));
      }

      REGISTER_DECLARE(lift_ratexp,
                       (const ratexp& aut) -> ratexp);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_LIFT_HH
