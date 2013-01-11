#ifndef VCSN_ALGOS_LIFT_HH
# define VCSN_ALGOS_LIFT_HH

# include <vcsn/ctx/ctx.hh>
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
      ctx::context<typename Context::labelset_t,
                   ratexpset<Context>,
                   labels_are_unit>;

    // lift(ctx) -> ctx
    template <typename Context>
    lifted_context_t<Context>
    lift_context(const Context& ctx)
    {
      using rs_in_t = typename Context::ratexpset_t;
      auto rs_in = ctx.make_ratexpset();
      typedef details::lifted_context_t<Context> ctx_out_t;
      return ctx_out_t{ctx.labelset(),
                       std::make_shared<const rs_in_t>(rs_in)};
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

    auto ctx_out = details::lift_context(a.context());
    // Not using: http://gcc.gnu.org/bugzilla/show_bug.cgi?id=53540.
    typedef details::lifted_automaton_t<auto_in_t> auto_out_t;
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
        return std::make_shared<vcsn::details::lifted_automaton_t<Aut>>
          (::vcsn::lift(dynamic_cast<const Aut&>(*aut)));
      }

      using lift_automaton_t = auto (const automaton& aut) -> automaton;

      bool lift_automaton_register(const std::string& ctx,
                                   const lift_automaton_t& fn);

    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_LIFT_HH
