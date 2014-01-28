#ifndef VCSN_ALGOS_STANDARD_HH
# define VCSN_ALGOS_STANDARD_HH

# include <set>

# include <vcsn/ctx/fwd.hh>
# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/core/rat/visitor.hh>
# include <vcsn/dyn/ratexp.hh>
# include <vcsn/misc/raise.hh>

namespace vcsn
{

  /*-------------------------.
  | is_standard(automaton).  |
  `-------------------------*/

  template <typename Aut>
  bool
  is_standard(const Aut& a)
  {
    return
      a.num_initials() == 1
      && a.weightset()->is_one(a.weight_of(a.initial_transitions().front()))
      // No arrival on the initial state.
      && a.in(a.dst_of(a.initial_transitions().front())).empty();
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      bool
      is_standard(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return is_standard(a);
      }

      REGISTER_DECLARE(is_standard,
                       (const automaton& e) -> bool);
    }
  }

  /*----------------------.
  | standard(automaton).  |
  `----------------------*/

  /// Turn \a aut into a standard automaton.
  template <typename Aut>
  void
  standard(Aut& aut)
  {
    if (is_standard(aut))
      return;

    auto ws = *aut.weightset();

    const auto& inits = aut.initial_transitions();
    std::vector<typename Aut::state_t> initials{begin(inits), end(inits)};

    auto ini = aut.new_state();
    for (auto ti: initials)
      {
        auto wi = aut.weight_of(ti);
        for (auto t: aut.all_out(aut.dst_of(ti)))
          aut.new_transition(ini, aut.dst_of(t), aut.label_of(t),
                             ws.mul(wi, aut.weight_of(t)));
        aut.del_transition(ti);
      }
    aut.set_initial(ini);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      automaton
      standard(const automaton& aut)
      {
        auto res = copy(aut->as<Aut>());
        standard(res);
        return make_automaton(std::move(res));
      }

      REGISTER_DECLARE(standard,
                       (const automaton& e) -> automaton);
    }
  }


  /*-------------------.
  | standard(ratexp).  |
  `-------------------*/

  namespace rat
  {

    /// \tparam Aut      relative the generated automaton
    /// \tparam Context  relative to the RatExp.
    template <typename Aut,
              typename Context = typename Aut::context_t>
    class standard_visitor
      : public Context::const_visitor
    {
    public:
      using automaton_t = Aut;
      using context_t = Context;
      using weightset_t = typename context_t::weightset_t;
      using weight_t = typename context_t::weight_t;
      using state_t = typename automaton_t::state_t;

      using super_type = typename Context::const_visitor;

      standard_visitor(const context_t& ctx)
        : ws_(*ctx.weightset())
        , res_(ctx)
      {}

      automaton_t
      operator()(const typename context_t::ratexp_t& v)
      {
        v->accept(*this);
        res_.set_initial(initial_);
        return std::move(res_);
      }

      VCSN_RAT_VISIT(intersection,)
      {
        raise("standard: intersection is not supported");
      }

      VCSN_RAT_VISIT(complement,)
      {
        raise("standard: complement is not supported");
      }

      VCSN_RAT_VISIT(shuffle,)
      {
        raise("standard: shuffle is not supported");
      }

      VCSN_RAT_VISIT(zero,)
      {
        initial_ = res_.new_state();
      }

      VCSN_RAT_VISIT(one,)
      {
        auto i = res_.new_state();
        initial_ = i;
        res_.set_final(i);
      }

      VCSN_RAT_VISIT(atom, e)
      {
        auto i = res_.new_state();
        auto f = res_.new_state();
        initial_ = i;
        res_.new_transition(i, f, e.value());
        res_.set_final(f);
      }

      /// The current set of final states.
      using states_t = std::set<state_t>;
      states_t
      finals()
      {
        states_t res;
        for (auto t: res_.final_transitions())
          res.insert(res_.src_of(t));
        return res;
      }

      VCSN_RAT_VISIT(sum, e)
      {
        states_t other_finals = finals();
        e.head()->accept(*this);
        state_t initial = initial_;
        for (auto c: e.tail())
          {
            c->accept(*this);
            for (auto t: res_.all_out(initial_))
              // Not set_transition: for instance 'a*+a*' will make
              // "initial" go twice to post().
              res_.add_transition(initial,
                                  res_.dst_of(t),
                                  res_.label_of(t),
                                  res_.weight_of(t));
            res_.del_state(initial_);
          }
        initial_ = initial;
      }

      VCSN_RAT_VISIT(prod, e)
      {
        // The set of the final states that were introduced in pending
        // parts of the automaton (for instance if we are in the rhs
        // of "a+bc", recording the final state for "a").
        states_t other_finals = finals();

        // Traverse the first part of the product, handle left_weight.
        e.head()->accept(*this);
        state_t initial = initial_;

        // Then the remainder.
        for (auto c: e.tail())
          {
            // The set of the current (left-hand side) final transitions.
            auto ftr_ = res_.final_transitions();
            // Store transitions by copy.
            using transs_t = std::vector<typename automaton_t::transition_t>;
            transs_t ftr{ begin(ftr_), end(ftr_) };

            // Visit the next member of the product.
            c->accept(*this);

            // Branch all the previously added final transitions to
            // the successors of the new initial state.
            for (auto t1: ftr)
              if (!has(other_finals, res_.src_of(t1)))
                {
                  // Remove the previous final transition first, as we
                  // might add a final transition for the same state
                  // later.
                  //
                  // For instance on "{2}a+({3}\e+{5}a)", the final
                  // state s1 of {2}a will be made final thanks to
                  // {3}\e.  So if we compute the new transitions from
                  // s1 and then remove t1, we will have removed the
                  // fact that s1 is final thanks to {3}\e.
                  //
                  // Besides, s1 will become final with weight {3}, which
                  // might interfere with {5}a too.
                  auto s1 = res_.src_of(t1);
                  auto w1 = res_.weight_of(t1);
                  res_.del_transition(t1);
                  for (auto t2: res_.all_out(initial_))
                    res_.set_transition(s1,
                                        res_.dst_of(t2),
                                        res_.label_of(t2),
                                        ws_.mul(w1, res_.weight_of(t2)));
                }
            res_.del_state(initial_);
          }
        initial_ = initial;
      }

      // See star_here().
      VCSN_RAT_VISIT(star, e)
      {
        states_t other_finals = finals();
        e.sub()->accept(*this);
        // The "final weight of the initial state", starred.
        weight_t w = ws_.star(res_.get_final_weight(initial_));
        // Branch all the final states (but initial) to the successors
        // of initial.
        for (auto ti: res_.out(initial_))
          {
            res_.lmul_weight(ti, w);
            for (auto tf: res_.final_transitions())
              if (res_.src_of(tf) != initial_
                  && !has(other_finals, res_.src_of(tf)))
                // Note that the weight of ti has already been
                // multiplied, on the left, by w.
                //
                // Not set_transition, as for instance with "a**", the
                // second star modifies many existing transitions.
                res_.add_transition
                  (res_.src_of(tf),
                   res_.dst_of(ti),
                   res_.label_of(ti),
                   ws_.mul(res_.weight_of(tf), res_.weight_of(ti)));
          }
        for (auto tf: res_.final_transitions())
          res_.rmul_weight(tf, w);
        res_.set_final(initial_, w);
      }

      VCSN_RAT_VISIT(lweight, e)
      {
        e.sub()->accept(*this);
        for (auto t: res_.all_out(initial_))
          res_.lmul_weight(t, e.weight());
      }

      VCSN_RAT_VISIT(rweight, e)
      {
        states_t other_finals = finals();
        e.sub()->accept(*this);
        for (auto t: res_.final_transitions())
          if (! has(other_finals, res_.src_of(t)))
            res_.rmul_weight(t, e.weight());
      }

    private:
      const weightset_t& ws_;
      automaton_t res_;
      state_t initial_ = automaton_t::null_state();
    };

  } // rat::

  /// \tparam Aut      relative to the generated automaton.
  /// \tparam Context  relative to the RatExp.
  template <typename Aut,
            typename Context = typename Aut::context_t>
  Aut
  standard(const Context& ctx, const typename Context::ratexp_t& e)
  {
    rat::standard_visitor<Aut, Context> standard{ctx};
    return standard(e);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSet>
      automaton
      standard_exp(const ratexp& exp)
      {
        // FIXME: So far, there is a single implementation of ratexps,
        // but we should actually be parameterized by its type too.
        using context_t = typename RatExpSet::context_t;
        using ratexpset_t = RatExpSet;
        using automaton_t = mutable_automaton<context_t>;
        const auto& e = exp->as<ratexpset_t>();
        return make_automaton(standard<automaton_t>(e.get_ratexpset().context(),
                                                    e.ratexp()));
      }

      REGISTER_DECLARE(standard_exp,
                       (const ratexp& e) -> automaton);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_STANDARD_HH
