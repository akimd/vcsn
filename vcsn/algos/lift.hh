#ifndef VCSN_ALGOS_LIFT_HH
# define VCSN_ALGOS_LIFT_HH

# include <vcsn/ctx/ctx.hh>
# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/core/rat/kratexpset.hh>
# include <vcsn/core/rat/kratexp.hh>

namespace vcsn
{

  namespace details
  {
    template <typename Context>
    using lifted_context_t =
      ctx::context<typename Context::genset_t,
                   kratexpset<Context>,
                   labels_are_words>;

    template <typename Aut>
    using lifted_automaton_t =
      mutable_automaton<lifted_context_t<typename Aut::context_t>>;
  }

  template <typename Aut>
  inline
  details::lifted_automaton_t<Aut>
  lift(const Aut& a)
  {
    using kre_t = kratexpset<typename Aut::context_t>;
    kre_t kre{a.context()};
    // Not using: http://gcc.gnu.org/bugzilla/show_bug.cgi?id=53540.
    typedef details::lifted_context_t<typename Aut::context_t> ctx_t;
    ctx_t ctx{a.context().genset(), std::make_shared<const kre_t>(kre)};
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
           ctx.genset()->to_word(""),
           kre.weight(a.weight_of(t), kre.atom(a.label_of(t))));
    return res;
  }

} // vcsn::

#endif // !VCSN_ALGOS_LIFT_HH
