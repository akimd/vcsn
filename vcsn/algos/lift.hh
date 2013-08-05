#ifndef VCSN_ALGOS_LIFT_HH
# define VCSN_ALGOS_LIFT_HH

# include <map>

# include <vcsn/ctx/ctx.hh>
# include <vcsn/labelset/oneset.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/core/rat/ratexpset.hh>
# include <vcsn/core/rat/ratexp.hh>

namespace vcsn
{

  namespace detail
  {
    /*--------------.
    | lift(types).  |
    `--------------*/

    template <typename Context>
    using lifted_context_t =
      ctx::context<ctx::oneset, ratexpset<Context>>;

    template <typename Aut>
    using lifted_automaton_t =
      mutable_automaton<lifted_context_t<typename Aut::context_t>>;

    template <typename RatExpSet>
    using lifted_ratexpset_t =
      ratexpset<lifted_context_t<typename RatExpSet::context_t>>;

    // lift(ctx) -> ctx
    template <typename Context>
    lifted_context_t<Context>
    lift(const Context& ctx)
    {
      auto rs_in = ctx.make_ratexpset();
      return {ctx::oneset{}, rs_in};
    }

    // lift(ratexpset) -> ratexpset
    template <typename Context>
    lifted_ratexpset_t<ratexpset<Context>>
    lift(const ratexpset<Context>& rs)
    {
      return {lift(rs.context())};
    }

  }

  /*------------------.
  | lift(automaton).  |
  `------------------*/

  template <typename Aut>
  inline
  detail::lifted_automaton_t<Aut>
  lift(const Aut& a)
  {
    using auto_in_t = Aut;
    using ctx_in_t = typename auto_in_t::context_t;
    using state_in_t = typename auto_in_t::state_t;

    // Produce RatExps of the same context as the original automaton.
    using rs_in_t = ratexpset<ctx_in_t>;
    rs_in_t rs_in{a.context()};

    auto ctx_out = detail::lift(a.context());
    using auto_out_t = detail::lifted_automaton_t<auto_in_t>;
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
          (map[a.dst_of(t)], rs_in.one(a.weight_of(t)));
      else if (a.dst_of(t) == a.post())
        res.add_final
          (map[a.src_of(t)], rs_in.one(a.weight_of(t)));
      else
        res.add_transition
          (map[a.src_of(t)], map[a.dst_of(t)],
           {},
           rs_in.weight(a.weight_of(t), rs_in.atom(a.label_of(t))));
    return res;
  }


  namespace dyn
  {
    namespace detail
    {
      template <typename Aut>
      automaton
      lift(const automaton& aut)
      {
        const auto& a = dynamic_cast<const Aut&>(*aut);
        return make_automaton(::vcsn::detail::lift(a.context()), lift(a));
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
      typename lifted_context_t<typename Exp::context_t>::ratexp_t;

  }

  template <typename RatExpSet>
  inline
  typename detail::lifted_ratexpset_t<RatExpSet>::ratexp_t
  lift(const RatExpSet& rs, const typename RatExpSet::ratexp_t& e)
  {
    return detail::lift(rs).one(e);
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
      lift(const ratexp& exp)
      {
        const auto& e = exp->as<RatExpSet>();
        return make_ratexp(::vcsn::detail::lift(e.get_ratexpset()),
                           ::vcsn::lift(e.get_ratexpset(), e.ratexp()));
      }

      REGISTER_DECLARE(lift_exp,
                       (const ratexp& aut) -> ratexp);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_LIFT_HH
