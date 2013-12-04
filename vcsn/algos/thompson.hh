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
    /// \tparam Aut      relative the generated automaton
    /// \tparam Context  relative to the RatExp.
    template <typename Aut,
              typename Context = typename Aut::context_t>
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
      using shuffle_t = typename super_type::shuffle_t;
      using intersection_t = typename super_type::intersection_t;
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
        res_.new_transition(initial_, final_,
                            epsilon_,
                            e.left_weight());
      }

      virtual void
      visit(const atom_t& e)
      {
        initial_ = res_.new_state();
        final_ = res_.new_state();
        res_.new_transition(initial_, final_,
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
            res_.new_transition(initial, initial_,
                                epsilon_,
                                e.left_weight());
            res_.new_transition(final_, final,
                                epsilon_,
                                e.right_weight());
          }
        initial_ = initial;
        final_ = final;
      }

      virtual void
      visit(const intersection_t&)
      {
        throw std::domain_error("thompson: intersection is not supported");
      }

      virtual void
      visit(const shuffle_t&)
      {
        throw std::domain_error("shuffle: intersection is not supported");
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
            res_.new_transition(final, initial_, epsilon_);
          }
        initial_ = initial;

        // Apply weights.
        if (!ws_.is_one(e.left_weight()))
          for (auto t: res_.out(initial_))
            res_.set_weight(t, ws_.mul(e.left_weight(), res_.weight_of(t)));
        if (!ws_.is_one(e.right_weight()))
          for (auto t: res_.in(final_))
            res_.set_weight(t, ws_.mul(res_.weight_of(t), e.right_weight()));
      }

      virtual void
      visit(const star_t& e)
      {
        e.sub()->accept(*this);
        state_t initial = res_.new_state();
        state_t final = res_.new_state();
        res_.new_transition(initial, initial_, epsilon_, e.left_weight());
        res_.new_transition(final_,  final,    epsilon_, e.right_weight());
        res_.new_transition(final_,  initial_, epsilon_);
        res_.new_transition(initial, final, epsilon_,
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

  /// \tparam Aut      relative to the generated automaton.
  /// \tparam Context  relative to the RatExp.
  template <typename Aut,
            typename Context = typename Aut::context_t>
  Aut
  thompson(const Context& ctx, const typename Context::ratexp_t& e)
  {
    rat::thompson_visitor<Aut, Context> thompson{ctx};
    return thompson(e);
  }

  namespace dyn
  {
    namespace detail
    {
      /*---------------------.
      | dyn::thompson(exp).  |
      `---------------------*/

      /// Bridge.
      template <typename RatExpSet>
      automaton
      thompson(const ratexp& exp)
      {
        // FIXME: So far, there is a single implementation of ratexps,
        // but we should actually be parameterized by its type too.
        using context_t = typename RatExpSet::context_t;
        using ratexpset_t = RatExpSet;
        using automaton_t = mutable_automaton<context_t>;
        const auto& e = exp->as<ratexpset_t>();
        return make_automaton(thompson<automaton_t>(e.get_ratexpset().context(),
                                                    e.ratexp()));
      }

      REGISTER_DECLARE(thompson,
                       (const ratexp& e) -> automaton);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_THOMPSON_HH
