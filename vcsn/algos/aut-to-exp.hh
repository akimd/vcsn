#ifndef VCSN_ALGOS_AUT_TO_EXP_HH
# define VCSN_ALGOS_AUT_TO_EXP_HH

# include <vcsn/algos/copy.hh>
# include <vcsn/algos/lift.hh>
# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/core/rat/ratexp.hh>

namespace vcsn
{

  /*----------------.
  | state_chooser.  |
  `----------------*/

  /// A state (inner) from an automaton.
  template <typename Aut,
            typename Lifted = detail::lifted_automaton_t<Aut>>
  using state_chooser_t =
    std::function<typename Lifted::state_t(const Lifted&)>;


  /*--------------------------.
  | Naive heuristics degree.  |
  `--------------------------*/

  template <typename Aut>
  typename Aut::state_t
  next_naive(const Aut& a)
  {
    typename Aut::state_t best = 0;
    bool best_has_loop = false;
    size_t best_degree = std::numeric_limits<size_t>::max();
    for (auto s: a.states())
      {
        size_t out = 0;
        // Since we are in LAO, there can be at most one such loop.
        bool has_loop = false;
        // Don't count the loops as out-degree.
        for (auto t: a.all_out(s))
          if (a.dst_of(t) != s)
            ++out;
          else
            has_loop = true;
        size_t in = a.all_in(s).size();
        size_t degree = in * out;
        // We prefer to delete a state that has no loop transition.
        if (degree < best_degree
            || (degree == best_degree && has_loop < best_has_loop))
          {
            best = s;
            best_degree = degree;
            best_has_loop = has_loop;
          }
      }
    assert(best);
    return best;
  }

  /*------------------.
  | eliminate_state.  |
  `------------------*/

  namespace detail
  {
    template <typename Aut, typename Kind = typename Aut::context_t::kind_t>
    struct state_eliminator;

    template <typename Aut>
    struct state_eliminator<Aut, labels_are_one>
    {
      static_assert(Aut::context_t::is_lao,
                    "requires labels_are_one");

      using automaton_t = typename std::remove_cv<Aut>::type;
      using state_t = typename automaton_t::state_t;
      using weightset_t = typename automaton_t::weightset_t;
      /// State selector type.
      using state_chooser_t = std::function<state_t(const automaton_t&)>;

      state_eliminator(automaton_t& aut)
        : debug_(0)
        , aut_(aut)
      {}

      /// Eliminate state s.
      void operator()(state_t s)
      {
        require(aut_.has_state(s), "not a valid state: " + std::to_string(s));

        // The loop's weight.
        auto loop = ws_.zero();
        assert(aut_.outin(s, s).size() <= 1);
        // There is a single possible loop labeled by \e, but it's
        // easier and symmetrical with LAR to use a for-loop.
        for (auto t: aut_.outin(s, s))
          {
            loop = ws_.add(loop, aut_.weight_of(t));
            aut_.del_transition(t);
          }
        loop = ws_.star(loop);

        // Get all the predecessors, and successors, except itself.
        auto outs = aut_.all_out(s);
        for (auto in: aut_.all_in(s))
          for (auto out: outs)
            aut_.add_transition
              (aut_.src_of(in), aut_.dst_of(out),
               aut_.label_of(in),
               ws_.mul(ws_.mul(aut_.weight_of(in), loop),
                       aut_.weight_of(out)));
        aut_.del_state(s);
      }

      /// Eliminate all the states, in the order specified by \a next_state.
      void operator()(const state_chooser_t& next_state)
      {
        while (aut_.num_states())
          operator()(next_state(aut_));
      }

    private:
      /// Debug level.  The higher, the more details are reported.
      int debug_;
      /// The automaton we work on.
      automaton_t& aut_;
      /// Shorthand to the weightset.
      const weightset_t& ws_ = *aut_.weightset();
    };

    template <typename Aut>
    struct state_eliminator<Aut, labels_are_ratexps>
    {
      static_assert(Aut::context_t::is_lar,
                    "requires labels_are_ratexps");

      using automaton_t = typename std::remove_cv<Aut>::type;
      using state_t = typename automaton_t::state_t;
      using ratexpset_t = typename automaton_t::labelset_t;
      using weightset_t = typename automaton_t::weightset_t;
      /// State selector type.
      using state_chooser_t = std::function<state_t(const automaton_t&)>;

      state_eliminator(automaton_t& aut)
        : debug_(0)
        , aut_(aut)
      {}

      /// Eliminate state s.
      void operator()(state_t s)
      {
        require(aut_.has_state(s), "not a valid state: " + std::to_string(s));

        // The loops' ratexp.
        auto loop = rs_.zero();
        for (auto t: aut_.outin(s, s))
          {
            loop = rs_.add(loop,
                           rs_.lmul(aut_.weight_of(t), aut_.label_of(t)));
            aut_.del_transition(t);
          }
        loop = rs_.star(loop);

        // Get all the predecessors, and successors, except itself.
        auto outs = aut_.all_out(s);
        for (auto in: aut_.all_in(s))
          for (auto out: outs)
            aut_.add_transition
              (aut_.src_of(in), aut_.dst_of(out),
               rs_.mul(rs_.mul(rs_.lmul(aut_.weight_of(in), aut_.label_of(in)),
                               loop),
                       rs_.lmul(aut_.weight_of(out), aut_.label_of(out))));
        aut_.del_state(s);
      }

      /// Eliminate all the states, in the order specified by \a next_state.
      void operator()(const state_chooser_t& next_state)
      {
        while (aut_.num_states())
          operator()(next_state(aut_));
      }

    private:
      /// Debug level.  The higher, the more details are reported.
      int debug_;
      /// The automaton we work on.
      automaton_t& aut_;
      /// Shorthand to the labelset, which is a ratexpset.
      const ratexpset_t& rs_ = *aut_.labelset();
      /// Shorthand to the weightset.
      const weightset_t& ws_ = *aut_.weightset();
    };
  }

  /*-----------------------.
  | dyn::eliminate_state.  |
  `-----------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Int>
      automaton
      eliminate_state(const automaton& aut, int state)
      {
        using state_t = typename Aut::state_t;
        const auto& a = aut->as<Aut>();
        auto res = vcsn::copy(a);
        state_t s = state == -1 ? next_naive(res) : state + 2;
        vcsn::detail::state_eliminator<decltype(res)> eliminate_state(res);
        eliminate_state(s);
        return make_automaton(std::move(res));
      }

      REGISTER_DECLARE(eliminate_state,
                        (const automaton& aut, int) -> automaton);
    }
  }


  /*-------------.
  | aut_to_exp.  |
  `-------------*/

  template <typename Aut,
            typename Context = typename Aut::context_t>
  typename Context::ratexp_t
  aut_to_exp(const Aut& a,
             const state_chooser_t<Aut>& next_state)
  {
    // State elimination is performed on the lifted automaton.
    auto aut = lift(a);
    detail::state_eliminator<decltype(aut)> eliminate_states(aut);
    eliminate_states(next_state);
    return aut.get_initial_weight(aut.post());
  }


  template <typename Aut,
            typename Context = typename Aut::context_t>
  typename Context::ratexp_t
  aut_to_exp_naive(const Aut& a)
  {
    state_chooser_t<Aut> next = next_naive<detail::lifted_automaton_t<Aut>>;
    return aut_to_exp(a, next);
  }

  /*------------------.
  | dyn::aut_to_exp.  |
  `------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      ratexp
      aut_to_exp(const automaton& aut)
      {
        // FIXME: So far, there is a single implementation of ratexps,
        // but we should actually be parameterized by its type too.
        using context_t = typename Aut::context_t;
        using ratexpset_t = vcsn::ratexpset<context_t>;
        const auto& a = aut->as<Aut>();
        return make_ratexp(ratexpset_t(a.context()),
                           aut_to_exp_naive(a));
      }

      REGISTER_DECLARE(aut_to_exp,
                       (const automaton& aut) -> ratexp);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_AUT_TO_EXP_HH
