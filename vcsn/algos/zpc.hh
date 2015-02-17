#pragma once

#include <vcsn/algos/constant-term.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/core/rat/visitor.hh>
#include <vcsn/ctx/fwd.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/expression.hh>
#include <vcsn/labelset/labelset.hh> // make_nullableset_context
#include <vcsn/misc/raise.hh>

namespace vcsn
{
  namespace rat
  {
    /// Build a ZPC automaton from an expression.
    ///
    /// \tparam Aut     relative the generated automaton
    /// \tparam ExpSet  relative to the expression.
    template <typename Aut,
              typename ExpSet>
    class zpc_visitor
      : public ExpSet::const_visitor
    {
    public:
      using automaton_t = Aut;
      using expressionset_t = ExpSet;
      using context_t = context_t_of<automaton_t>;
      using weightset_t = weightset_t_of<expressionset_t>;
      using weight_t = weight_t_of<expressionset_t>;
      using state_t = state_t_of<automaton_t>;

      using super_t = typename expressionset_t::const_visitor;

      static_assert(labelset_t_of<Aut>::has_one(),
                    "zpc: requires nullable labels");

      constexpr static const char* me() { return "zpc"; }

      /// Build an automaton of context \a ctx.
      zpc_visitor(const context_t& ctx, const expressionset_t& rs)
        : rs_(rs)
        , res_(make_shared_ptr<automaton_t>(ctx))
      {}

      zpc_visitor(const expressionset_t& rs)
        : zpc_visitor(rs.context(), rs)
      {}

      automaton_t
      operator()(const typename expressionset_t::value_t& v)
      {
        v->accept(*this);
        res_->set_initial(initial_);
        res_->set_final(final_);

        for (auto t: res_->all_in(res_->post()))
          res_->set_weight(t, ws_.mul(res_->weight_of(t), final_weight_));

        return std::move(res_);
      }

    private:
      VCSN_RAT_UNSUPPORTED(complement)
      VCSN_RAT_UNSUPPORTED(conjunction)
      VCSN_RAT_UNSUPPORTED(ldiv)
      VCSN_RAT_UNSUPPORTED(shuffle)
      VCSN_RAT_UNSUPPORTED(transposition)

      VCSN_RAT_VISIT(zero,)
      {
        initial_ = res_->new_state();
        final_ = res_->new_state();
      }

      VCSN_RAT_VISIT(one,)
      {
        initial_ = res_->new_state();
        final_ = res_->new_state();

        res_->set_final(initial_);
      }

      VCSN_RAT_VISIT(atom, e)
      {
        initial_ = res_->new_state();
        final_ = res_->new_state();
        // The automaton and the expression might have different
        // labelsets.
        res_->new_transition(initial_, final_,
                             res_->labelset()->conv(*rs_.labelset(),
                                                    e.value()));
      }

      VCSN_RAT_VISIT(sum, e)
      {
        state_t initial = res_->new_state();

        weight_t cst = ws_.zero();
        std::vector<state_t> finals_;

        for (auto c: e)
        {
          c->accept(*this);
          res_->new_transition(initial, initial_, epsilon_);
          finals_.push_back(final_);

          cst = ws_.add(cst, res_->get_final_weight(initial_));
          res_->unset_final(initial_);
        }

        state_t final = res_->new_state();

        for (auto s: finals_)
          res_->new_transition(s, final, epsilon_);

        initial_ = initial;
        final_ = final;

        // Constant term weight.
        res_->set_final(initial_, cst);
      }

      VCSN_RAT_VISIT(prod, e)
      {
        // Number of 'initial' states to create.
        auto size = e.size() - 1;

        std::vector<state_t> state_stack;

        for (auto i = 0; i < size; ++i)
          state_stack.push_back(res_->new_state());

        e.head()->accept(*this);

        state_t initial_e = initial_;
        state_t final_e = final_;

        state_t initial = state_stack.back();
        state_stack.pop_back();

        res_->new_transition(initial, initial_e, epsilon_);

        state_t final;

        for (auto t: e.tail())
          {
            t->accept(*this);
            final = res_->new_state();

            res_->new_transition(final_, final, epsilon_);

            auto cst_e = res_->get_final_weight(initial_e);
            auto cst_f = res_->get_final_weight(initial_);

            res_->new_transition(initial, initial_, epsilon_, cst_e);
            res_->unset_final(initial_e);
            res_->new_transition(final_e, final, epsilon_, cst_f);
            res_->unset_final(initial_);

            res_->set_final(initial, ws_.mul(cst_e, cst_f));

            res_->new_transition(final_e, initial_, epsilon_);

            initial_e = initial;
            final_e = final;

            if (!state_stack.empty())
              {
                initial = state_stack.back();
                state_stack.pop_back();

                res_->new_transition(initial, initial_e, epsilon_);
              }
          }

        initial_ = initial;
        final_ = final;

        res_->set_initial(initial);
      }

      VCSN_RAT_VISIT(star, e)
      {
        state_t initial = res_->new_state();

        e.sub()->accept(*this);

        state_t final = res_->new_state();

        auto cst = ws_.star(res_->get_final_weight(initial_));
        res_->unset_final(initial_);

        res_->new_transition(initial, initial_, epsilon_, cst);
        res_->new_transition(final_, final, epsilon_, cst);
        res_->new_transition(final_, initial_, epsilon_, cst);

        initial_ = initial;
        final_ = final;

        res_->set_final(initial_, cst);
      }

      VCSN_RAT_VISIT(lweight, e)
      {
        e.sub()->accept(*this);

        const weight_t& w = e.weight();
        for (auto t: res_->all_out(initial_))
          res_->set_weight(t, ws_.mul(w, res_->weight_of(t)));
      }

      VCSN_RAT_VISIT(rweight, e)
      {
        e.sub()->accept(*this);

        final_weight_ = ws_.mul(e.weight(), final_weight_);
      }

   private:
      const expressionset_t& rs_;
      const weightset_t& ws_ = *rs_.weightset();
      automaton_t res_;
      using label_t = label_t_of<automaton_t>;
      const label_t epsilon_ = res_->labelset()->one();
      state_t initial_ = automaton_t::element_type::null_state();
      state_t final_ = automaton_t::element_type::null_state();
      weight_t final_weight_ = ws_.one();
    };

  } // rat::

  /// Build a ZPC automaton from an expression.
  ///
  /// \tparam Aut     relative to the generated automaton.
  /// \tparam ExpSet  relative to the expression.
  template <typename Aut, typename ExpSet>
  inline
  Aut
  zpc(const context_t_of<Aut>& ctx,
      const ExpSet& rs, const typename ExpSet::value_t& r)
  {
    rat::zpc_visitor<Aut, ExpSet> zpc{ctx, rs};
    return zpc(r);
  }

  /// Build a ZPC automaton from an expression.
  ///
  /// \tparam Aut     relative to the generated automaton.
  /// \tparam ExpSet  relative to the expression.
  template <typename Aut, typename ExpSet>
  inline
  Aut
  zpc(const ExpSet& rs, const typename ExpSet::value_t& r)
  {
    rat::zpc_visitor<Aut, ExpSet> zpc{rs};
    return zpc(r);
  }

  namespace dyn
  {
    namespace detail
    {
      /*---------------------.
      | dyn::zpc(exp).       |
      `---------------------*/

      /// Bridge.
      template <typename ExpSet>
      inline
      automaton
      zpc(const expression& exp)
      {
        // FIXME: So far, there is a single implementation of expressions,
        // but we should actually be parameterized by its type too.
        using expressionset_t = ExpSet;
        const auto& e = exp->as<expressionset_t>();
        auto ctx
          = vcsn::detail::make_nullableset_context(e.expressionset().context());
        using ctx_t = decltype(ctx);
        using automaton_t = mutable_automaton<ctx_t>;
        return make_automaton(::vcsn::zpc<automaton_t>(ctx,
                                                       e.expressionset(),
                                                       e.expression()));
      }
    }
  }

} // vcsn::
