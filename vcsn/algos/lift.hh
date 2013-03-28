#ifndef VCSN_ALGOS_LIFT_HH
# define VCSN_ALGOS_LIFT_HH

# include <vcsn/ctx/ctx.hh>
# include <vcsn/ctx/unitset.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/core/rat/ratexpset.hh>
# include <vcsn/core/rat/ratexp.hh>

namespace vcsn
{

  /*------------------.
  | lift(automaton).  |
  `------------------*/

  namespace details
  {
    template <typename Context>
    using lifted_context_t =
      ctx::context<ctx::unitset, ratexpset<Context>>;

    // lift(ctx) -> ctx
    template <typename Context>
    lifted_context_t<Context>
    lift(const Context& ctx)
    {
      auto rs_in = ctx.make_ratexpset();
      using ctx_out_t = details::lifted_context_t<Context>;
      return ctx_out_t(ctx::unitset{}, rs_in);
    }

    template <typename Aut>
    using lifted_automaton_t =
      mutable_automaton<lifted_context_t<typename Aut::context_t>>;
  }

  template <typename Aut>
  inline
  details::lifted_automaton_t<Aut>
  lift(const Aut& a)
  {
    using auto_in_t = Aut;
    using ctx_in_t = typename auto_in_t::context_t;
    using state_in_t = typename auto_in_t::state_t;

    // Produce RatExps of the same context as the original automaton.
    using rs_in_t = ratexpset<ctx_in_t>;
    rs_in_t rs_in{a.context()};

    auto ctx_out = details::lift(a.context());
    using auto_out_t = details::lifted_automaton_t<auto_in_t>;
    using state_out_t = typename auto_out_t::state_t;
    auto_out_t res{ctx_out};
    std::map<state_in_t, state_out_t> map;
    map[a.pre()] = res.pre();
    map[a.post()] = res.post();
    for (auto s: a.states())
      map[s] = res.new_state();

    for (auto t: a.all_transitions())
      if (a.src_of(t) == a.pre())
        res.add_initial
          (map[a.dst_of(t)], rs_in.unit(a.weight_of(t)));
      else if (a.dst_of(t) == a.post())
        res.add_final
          (map[a.src_of(t)], rs_in.unit(a.weight_of(t)));
      else
        res.add_transition
          (map[a.src_of(t)], map[a.dst_of(t)],
           {},
           rs_in.weight(a.weight_of(t), rs_in.atom(a.label_of(t))));
    return res;
  }


  namespace dyn
  {
    namespace details
    {
      template <typename Aut>
      automaton
      lift(const automaton& aut)
      {
        const auto& a = dynamic_cast<const Aut&>(*aut);
        return make_automaton(::vcsn::details::lift(a.context()), lift(a));
      }

      using lift_automaton_t = auto (const automaton& aut) -> automaton;

      bool lift_automaton_register(const std::string& ctx,
                                   lift_automaton_t fn);
    }
  }


  /*---------------.
  | lift(ratexp).  |
  `---------------*/

  namespace details
  {
    template <typename Exp>
    using lifted_ratexp_t =
      typename lifted_context_t<typename Exp::context_t>::ratexp_t;

  }

  /// \param Exp  Ctx::ratexp_t = vcsn::rat::node<Label, Weight>.
  template <typename Context>
  inline
  typename details::lifted_context_t<Context>::ratexp_t
  lift(const Context& ctx, const typename Context::ratexp_t& e)
  {
    return details::lift(ctx).make_ratexpset().unit(e);
  }


  namespace dyn
  {
    namespace details
    {
      template <typename Context>
      ratexp
      lift(const ratexp& e)
      {
        const auto& ctx =
          dynamic_cast<const Context&>(e->ctx());
        const auto& exp =
          std::dynamic_pointer_cast<const typename Context::node_t>(e->ratexp());
        return make_ratexp(::vcsn::details::lift(ctx), ::vcsn::lift(ctx, exp));
      }

      using lift_exp_t = auto (const ratexp& aut) -> ratexp;
      bool lift_exp_register(const std::string& ctx, lift_exp_t fn);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_LIFT_HH
