#ifndef VCSN_ALGOS_STANDARD_HH
# define VCSN_ALGOS_STANDARD_HH

# include <vcsn/ctx/fwd.hh>
# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/core/rat/visitor.hh>
# include <vcsn/dyn/ratexp.hh>

namespace vcsn
{
  namespace rat
  {
    /// \param Aut      relative the generated automaton
    /// \param Context  relative to the RatExp.
    template <class Aut,
              class Context = typename Aut::context_t>
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
      using node_t = typename super_type::node_t;
      using inner_t = typename super_type::inner_t;
      using nary_t = typename super_type::nary_t;
      using prod_t = typename super_type::prod_t;
      using sum_t = typename super_type::sum_t;
      using leaf_t = typename super_type::leaf_t;
      using star_t = typename super_type::star_t;
      using zero_t = typename super_type::zero_t;
      using one_t = typename super_type::one_t;
      using atom_t = typename super_type::atom_t;

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

      virtual void
      visit(const zero_t&)
      {
        initial_ = res_.new_state();
      }

      virtual void
      visit(const one_t& v)
      {
        auto i = res_.new_state();
        initial_ = i;
        res_.set_final(i, v.left_weight());
      }

      virtual void
      visit(const atom_t& e)
      {
        auto i = res_.new_state();
        auto f = res_.new_state();
        initial_ = i;
        res_.add_transition(i, f, e.value(), e.left_weight());
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

      /// Apply the left weight to initial state, and the right weight
      /// to all the "fresh" final states, i.e., those that are not
      /// part of "other_finals".
      void
      apply_weights(const inner_t& e, const states_t& other_finals)
      {
        {
          weight_t w = e.left_weight();
          if (w != ws_.unit())
            for (auto t: res_.all_out(initial_))
              res_.lmul_weight(t, w);
        }
        {
          weight_t w = e.right_weight();
          if (w != ws_.unit())
            for (auto t: res_.final_transitions())
              if (other_finals.find(res_.src_of(t)) == other_finals.end())
                res_.rmul_weight(t, w);
        }
      }

      virtual void
      visit(const sum_t& e)
      {
        states_t other_finals = finals();
        e.head()->accept(*this);
        state_t initial = initial_;
        for (auto c: e.tail())
          {
            c->accept(*this);
            for (auto t: res_.all_out(initial_))
              res_.add_transition(initial,
                                  res_.dst_of(t),
                                  res_.label_of(t),
                                  res_.weight_of(t));
            res_.del_state(initial_);
          }
        initial_ = initial;
        apply_weights(e, other_finals);
      }

      virtual void
      visit(const prod_t& e)
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
              if (other_finals.find(res_.src_of(t1)) == other_finals.end())
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
        apply_weights(e, other_finals);
      }

      virtual void
      visit(const star_t& e)
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
                  && other_finals.find(res_.src_of(tf)) == other_finals.end())
                // Note that the weight of ti has already been
                // multiplied, on the left, by w.
                res_.add_transition
                  (res_.src_of(tf),
                   res_.dst_of(ti),
                   res_.label_of(ti),
                   ws_.mul(res_.weight_of(tf), res_.weight_of(ti)));
          }
        for (auto tf: res_.final_transitions())
          res_.rmul_weight(tf, w);
        res_.set_final(initial_, w);
        apply_weights(e, other_finals);
      }


    private:
      const weightset_t& ws_;
      automaton_t res_;
      state_t initial_ = automaton_t::null_state();
    };

  } // rat::

  /// \param Aut      relative to the generated automaton.
  /// \param Context  relative to the RatExp.
  template <class Aut,
            class Context = typename Aut::context_t>
  Aut
  standard(const Context& ctx, const typename Context::ratexp_t& e)
  {
    rat::standard_visitor<Aut, Context> standard{ctx};
    return standard(e);
  }

  /// \param Aut      relative to the generated automaton.
  /// \param Context  relative to the RatExp.
  template <class Aut,
            class Context = typename Aut::context_t>
  Aut
  standard(const Context& ctx, const rat::exp_t e)
  {
    return standard<Aut, Context>(ctx, ctx.downcast(e));
  }

  /*--------------------.
  | abstract standard.  |
  `--------------------*/
  namespace dyn
  {
    namespace detail
    {
      template <typename Aut>
      automaton
      standard(const dyn::ratexp& e)
      {
        const auto& ctx =
          dynamic_cast<const typename Aut::context_t&>(e->ctx());
        return make_automaton(ctx, standard<Aut>(ctx, e->ratexp()));
      }

      REGISTER_DECLARE(standard,
                       (const dyn::ratexp& e) -> automaton);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_STANDARD_HH
