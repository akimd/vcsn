#ifndef VCSN_ALGOS_THOMPSON_HH
# define VCSN_ALGOS_THOMPSON_HH

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
    class thompson_visitor
      : public Context::const_visitor
    {
    public:
      using automaton_t = Aut;
      using context_t = Context;
      using weightset_t = typename context_t::weightset_t;
      using weight_t = typename context_t::weight_t;
      using state_t = typename automaton_t::state_t;

      using super_type = typename Context::const_visitor;
      using prod_t = typename super_type::prod_t;
      using sum_t = typename super_type::sum_t;
      using star_t = typename super_type::star_t;
      using zero_t = typename super_type::zero_t;
      using one_t = typename super_type::one_t;
      using atom_t = typename super_type::atom_t;

      thompson_visitor(const context_t& ctx)
        : res_(ctx)
      {}

      automaton_t
      operator()(const typename context_t::ratexp_t& v)
      {
        v->accept(*this);
        res_.set_initial(initial_);
        res_.set_final(final_);
        return std::move(res_);
      }

      virtual void
      visit(const zero_t&)
      {
        initial_ = res_.new_state();
        final_ = res_.new_state();
      }

      virtual void
      visit(const one_t& e)
      {
        initial_ = res_.new_state();
        final_ = res_.new_state();
        res_.add_transition(initial_, final_,
                            epsilon_,
                            e.left_weight());
      }

      virtual void
      visit(const atom_t& e)
      {
        initial_ = res_.new_state();
        final_ = res_.new_state();
        res_.add_transition(initial_, final_,
                            e.value(),
                            e.left_weight());
      }

      virtual void
      visit(const sum_t& e)
      {
        state_t initial = res_.new_state();
        state_t final = res_.new_state();
        for (auto c: e)
          {
            c->accept(*this);
            res_.add_transition(initial, initial_,
                                epsilon_,
                                e.left_weight());
            res_.add_transition(final_, final,
                                epsilon_,
                                e.right_weight());
          }
        initial_ = initial;
        final_ = final;
      }

      virtual void
      visit(const prod_t& e)
      {
        e.head()->accept(*this);
        state_t initial = initial_;

        // Then the remainder.
        for (auto c: e.tail())
          {
            state_t final = final_;
            c->accept(*this);
            res_.add_transition(final, initial_, epsilon_);
          }
        initial_ = initial;

        if (!ws_.is_one(e.left_weight()))
          {
            state_t initial = res_.new_state();
            res_.add_transition(initial, initial_,
                                epsilon_,
                                e.left_weight());
            initial_ = initial;
          }
        if (!ws_.is_one(e.right_weight()))
          {
            state_t final = res_.new_state();
            res_.add_transition(final_, final,
                                epsilon_,
                                e.right_weight());
            final_ = final;
          }
      }

      virtual void
      visit(const star_t& e)
      {
        e.sub()->accept(*this);
        state_t initial = res_.new_state();
        state_t final = res_.new_state();
        res_.add_transition(initial, initial_, epsilon_, e.left_weight());
        res_.add_transition(final_,  final,    epsilon_, e.right_weight());
        res_.add_transition(final_,  initial_, epsilon_);
        res_.add_transition(initial, final, epsilon_,
                            ws_.mul(e.left_weight(), e.right_weight()));
        initial_ = initial;
        final_ = final;
      }


    private:
      automaton_t res_;
      const weightset_t& ws_ = *res_.weightset();
      using label_t = typename automaton_t::context_t::label_t;
      const label_t epsilon_ = res_.labelset()->one();
      state_t initial_ = automaton_t::null_state();
      state_t final_ = automaton_t::null_state();
    };

  } // rat::

  /// \param Aut      relative to the generated automaton.
  /// \param Context  relative to the RatExp.
  template <class Aut,
            class Context = typename Aut::context_t>
  Aut
  thompson(const Context& ctx, const typename Context::ratexp_t& e)
  {
    rat::thompson_visitor<Aut, Context> standard{ctx};
    return standard(e);
  }

  /// \param Aut      relative to the generated automaton.
  /// \param Context  relative to the RatExp.
  template <class Aut,
            class Context = typename Aut::context_t>
  Aut
  thompson(const Context& ctx, const rat::exp_t e)
  {
    return thompson<Aut, Context>(ctx, ctx.downcast(e));
  }

  /*--------------------.
  | abstract thompson.  |
  `--------------------*/
  namespace dyn
  {
    namespace detail
    {
      template <typename Aut>
      automaton
      thompson(const ratexp& e)
      {
        const auto& ctx =
          dynamic_cast<const typename Aut::context_t&>(e->ctx());
        return make_automaton(ctx, thompson<Aut>(ctx, e->ratexp()));
      }

      REGISTER_DECLARE(thompson,
                       (const ratexp& e) -> automaton);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_THOMPSON_HH
