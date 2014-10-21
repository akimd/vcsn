#ifndef VCSN_ALGOS_TO_EXPRESSION_HH
# define VCSN_ALGOS_TO_EXPRESSION_HH

# include <vcsn/algos/copy.hh>
# include <vcsn/algos/lift.hh>
# include <vcsn/core/rat/ratexp.hh>
# include <vcsn/misc/vector.hh>

namespace vcsn
{

  /*----------------.
  | state_chooser.  |
  `----------------*/

  /// A state (inner) from an automaton.
  template <typename Aut,
            typename Lifted = detail::lifted_automaton_t<Aut>>
  using state_chooser_t =
    std::function<state_t_of<Lifted>(const Lifted&)>;


  /*--------------------------.
  | Naive heuristics degree.  |
  `--------------------------*/

  template <typename Aut>
  state_t_of<Aut>
  next_naive(const Aut& a)
  {
    state_t_of<Aut> best = 0;
    bool best_has_loop = false;
    size_t best_degree = std::numeric_limits<size_t>::max();
    for (auto s: a->states())
      {
        size_t out = 0;
        // Since we are in LAO, there can be at most one such loop.
        bool has_loop = false;
        // Don't count the loops as out-degree.
        for (auto t: a->all_out(s))
          if (a->dst_of(t) != s)
            ++out;
          else
            has_loop = true;
        size_t in = a->all_in(s).size();
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
    template <typename Aut, typename Kind = typename context_t_of<Aut>::kind_t>
    struct state_eliminator;

    /// Eliminate states in an automaton whose labelset is oneset.
    template <typename Aut>
    struct state_eliminator<Aut, labels_are_one>
    {
      using automaton_t = typename std::remove_cv<Aut>::type;
      using state_t = state_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;
      /// State selector type.
      using state_chooser_t = std::function<state_t(const automaton_t&)>;

      state_eliminator(automaton_t& aut)
        : debug_(0)
        , aut_(aut)
      {}

      /// Eliminate state s.
      void operator()(state_t s)
      {
        require(aut_->has_state(s), "not a valid state: ", s);

        // The loop's weight.
        auto loop = ws_.zero();
        assert(aut_->outin(s, s).size() <= 1);
        // There is a single possible loop labeled by \e, but it's
        // easier and symmetrical with LAR to use a for-loop.
        for (auto t: to_vector(aut_->outin(s, s)))
          {
            loop = ws_.add(loop, aut_->weight_of(t));
            aut_->del_transition(t);
          }
        loop = ws_.star(loop);

        // Get all the predecessors, and successors, except itself.
        auto outs = aut_->all_out(s);
        for (auto in: aut_->all_in(s))
          for (auto out: outs)
            aut_->add_transition
              (aut_->src_of(in), aut_->dst_of(out),
               aut_->label_of(in),
               ws_.mul(aut_->weight_of(in), loop, aut_->weight_of(out)));
        aut_->del_state(s);
      }

      /// Eliminate all the states, in the order specified by \a next_state.
      void operator()(const state_chooser_t& next_state)
      {
        while (aut_->num_states())
          operator()(next_state(aut_));
      }

    private:
      /// Debug level.  The higher, the more details are reported.
      int debug_;
      /// The automaton we work on.
      automaton_t& aut_;
      /// Shorthand to the weightset.
      const weightset_t& ws_ = *aut_->weightset();
    };


    /// Eliminate states in an automaton whose labelset is a ratexpset.
    template <typename Aut>
    struct state_eliminator<Aut, labels_are_ratexps>
    {
      // FIXME: ratexpset<lal_char(a-c)_z>_q for instance cannot work,
      // because we need to move the q weights inside the
      // lal_char(a-c)_z ratexps, which obviously not possible.  So we
      // need to require that there is a subtype relationship between
      // the weightset and the weightset of the ratexp.
      //
      // Yet as of 2014-07, there is no means to check that subtype
      // relationship in Vaucanson.

      using automaton_t = typename std::remove_cv<Aut>::type;
      using state_t = state_t_of<automaton_t>;
      using ratexpset_t = labelset_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;
      /// State selector type.
      using state_chooser_t = std::function<state_t(const automaton_t&)>;

      state_eliminator(automaton_t& aut)
        : debug_(0)
        , aut_(aut)
      {}

      /// Eliminate state s.
      void operator()(state_t s)
      {
        require(aut_->has_state(s), "not a valid state: ", s);

        // The loops' ratexp.
        auto loop = rs_.zero();
        for (auto t: to_vector(aut_->outin(s, s)))
          {
            loop = rs_.add(loop,
                           rs_.lmul(aut_->weight_of(t), aut_->label_of(t)));
            aut_->del_transition(t);
          }
        loop = rs_.star(loop);

        // Get all the predecessors, and successors, except itself.
        auto outs = aut_->all_out(s);
        for (auto in: aut_->all_in(s))
          for (auto out: outs)
            aut_->add_transition
              (aut_->src_of(in), aut_->dst_of(out),
               rs_.mul(rs_.lmul(aut_->weight_of(in), aut_->label_of(in)),
                       loop,
                       rs_.lmul(aut_->weight_of(out), aut_->label_of(out))));
        aut_->del_state(s);
      }

      /// Eliminate all the states, in the order specified by \a next_state.
      void operator()(const state_chooser_t& next_state)
      {
        while (aut_->num_states())
          operator()(next_state(aut_));
      }

    private:
      /// Debug level.  The higher, the more details are reported.
      int debug_;
      /// The automaton we work on.
      automaton_t& aut_;
      /// Shorthand to the labelset, which is a ratexpset.
      const ratexpset_t& rs_ = *aut_->labelset();
      /// Shorthand to the weightset.
      const weightset_t& ws_ = *aut_->weightset();
    };

    template <typename Aut>
    state_eliminator<Aut>
    make_state_eliminator(Aut& a)
    {
      return a;
    }
  }


  /// Remove state \a s from automaton \a res.
  template <typename Aut>
  Aut&
  eliminate_state_here(Aut& res, state_t_of<Aut> s)
  {
    if (s == res->null_state())
      s = next_naive(res);
    auto eliminate_state = detail::make_state_eliminator(res);
    eliminate_state(s);
    return res;
  }

  /// A copy of automaton \a res without the state \a s.
  template <typename Aut>
  Aut
  eliminate_state(const Aut& aut, state_t_of<Aut> s)
  {
    // FIXME: this is soooo wrong: we eliminate a state in a copy,
    // whose state numbers might be completely different.  We _need_
    // the state names.
    auto res = vcsn::copy(aut);
    return eliminate_state_here(res, s);
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
        const auto& a = aut->as<Aut>();
        auto s = a->null_state();
        // FIXME: weak.
        if (state != -1)
          s = state + 2;
        return make_automaton(vcsn::eliminate_state(a, s));
      }

      REGISTER_DECLARE(eliminate_state,
                       (const automaton& aut, int) -> automaton);
    }
  }


  /*-----------------.
  | to_expression.   |
  `-----------------*/

  template <typename Aut,
            typename RatExpSet = ratexpset<context_t_of<Aut>>>
  typename RatExpSet::value_t
  to_expression(const Aut& a,
                const state_chooser_t<Aut>& next_state)
  {
    // State elimination is performed on the lifted automaton.
    auto aut = lift(a);
    auto eliminate_states = detail::make_state_eliminator(aut);
    eliminate_states(next_state);
    return aut->get_initial_weight(aut->post());
  }


  template <typename Aut,
            typename RatExpSet = ratexpset<context_t_of<Aut>>>
  typename RatExpSet::value_t
  to_expression_naive(const Aut& a)
  {
    state_chooser_t<Aut> next = next_naive<detail::lifted_automaton_t<Aut>>;
    return to_expression(a, next);
  }

  /*----------------------.
  | dyn::to_expression.   |
  `----------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename String>
      ratexp
      to_expression(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        // FIXME: So far, there is a single implementation of ratexps,
        // but we should actually be parameterized by its type too.
        using context_t = context_t_of<Aut>;
        using ratexpset_t = vcsn::ratexpset<context_t>;
        ratexpset_t rs(a->context(), ratexpset_t::identities_t::trivial);
        if (algo == "auto" || algo == "naive")
          return make_ratexp(rs, ::vcsn::to_expression_naive(a));
        else
          raise("to-expression: invalid algorithm: ", str_escape(algo),
                ": expected \"auto\", or \"naive\"");
      }

      REGISTER_DECLARE(to_expression,
                       (const automaton& aut, const std::string& algo)
                       -> ratexp);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_TO_EXPRESSION_HH
