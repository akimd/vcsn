#ifndef VCSN_ALGOS_LIFT_HH
# define VCSN_ALGOS_LIFT_HH

# include <vcsn/ctx/ctx.hh>
# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/core/rat/ratexpset.hh>
# include <vcsn/core/rat/ratexp.hh>

namespace vcsn
{

  namespace details
  {
    template <typename Context>
    using lifted_context_t =
      ctx::context<typename Context::labelset_t,
                   ratexpset<Context>,
                   labels_are_unit>;

    template <typename Aut>
    using lifted_automaton_t =
      mutable_automaton<lifted_context_t<typename Aut::context_t>>;
  }

  template <typename Aut>
  inline
  details::lifted_automaton_t<Aut>
  lift(const Aut& a)
  {
    using kre_t = ratexpset<typename Aut::context_t>;
    kre_t kre{a.context()};
    // Not using: http://gcc.gnu.org/bugzilla/show_bug.cgi?id=53540.
    typedef details::lifted_context_t<typename Aut::context_t> ctx_t;
    ctx_t ctx{a.context().labelset(), std::make_shared<const kre_t>(kre)};
    using auto_in_t = Aut;
    // Not using: http://gcc.gnu.org/bugzilla/show_bug.cgi?id=53540.
    typedef details::lifted_automaton_t<auto_in_t> auto_out_t;
    using state_in_t = typename auto_in_t::state_t;
    using state_out_t = typename auto_out_t::state_t;
    auto res = make_mutable_automaton(ctx);
    std::map<state_in_t, state_out_t> map;
    map[a.pre()] = res.pre();
    map[a.post()] = res.post();
    for (auto s: a.states())
      map[s] = res.new_state();

    for (auto t: a.all_transitions())
      if (a.src_of(t) == a.pre())
        res.add_initial
          (map[a.dst_of(t)], kre.unit(a.weight_of(t)));
      else if (a.dst_of(t) == a.post())
        res.add_final
          (map[a.src_of(t)], kre.unit(a.weight_of(t)));
      else
        res.add_transition
          (map[a.src_of(t)], map[a.dst_of(t)],
           {},
           kre.weight(a.weight_of(t), kre.atom(a.label_of(t))));
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
          (lift(dynamic_cast<const Aut&>(*aut)));
      }

      using lift_t = auto (const automaton& aut) -> automaton;

      bool lift_register(const std::string& ctx, const lift_t& fn);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_LIFT_HH
