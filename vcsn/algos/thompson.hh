#pragma once

#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/core/rat/visitor.hh>
#include <vcsn/ctx/fwd.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/labelset/labelset.hh> // make_nullableset_context
#include <vcsn/misc/raise.hh>

namespace vcsn
{
  namespace rat
  {
    /// Build a Thompson automaton from an expression.
    ///
    /// \tparam Aut        relative the generated automaton
    /// \tparam ExpSet  relative to the expression.
    template <Automaton Aut,
              typename ExpSet>
    class thompson_visitor
      : public ExpSet::const_visitor
    {
    public:
      using automaton_t = Aut;
      using expressionset_t = ExpSet;
      using expression_t = typename expressionset_t::value_t;
      using context_t = context_t_of<automaton_t>;
      using weightset_t = weightset_t_of<expressionset_t>;
      using weight_t = weight_t_of<expressionset_t>;
      using state_t = state_t_of<automaton_t>;

      using super_t = typename expressionset_t::const_visitor;

      static_assert(labelset_t_of<Aut>::has_one(),
                    "thompson: requires nullable labels");

      /// Name of this algorithm, for error messages.
      constexpr static const char* me() { return "thompson"; }

      /// Build an automaton of context \a ctx.
      thompson_visitor(const context_t& ctx, const expressionset_t& rs)
        : rs_(rs)
        , res_(make_shared_ptr<automaton_t>(ctx))
      {}

      thompson_visitor(const expressionset_t& rs)
        : thompson_visitor(rs.context(), rs)
      {}

      /// The Thompson automaton of v.
      automaton_t operator()(const expression_t& v)
      {
        v->accept(*this);
        res_->set_initial(initial_);
        res_->set_final(final_);
        return std::move(res_);
      }

    private:
      VCSN_RAT_UNSUPPORTED(complement)
      VCSN_RAT_UNSUPPORTED(compose)
      VCSN_RAT_UNSUPPORTED(conjunction)
      VCSN_RAT_UNSUPPORTED(infiltrate)
      VCSN_RAT_UNSUPPORTED(ldiv)
      VCSN_RAT_UNSUPPORTED(shuffle)
      VCSN_RAT_UNSUPPORTED(transposition)
      using tuple_t = typename super_t::tuple_t;
      virtual void visit(const tuple_t&, std::true_type) override
      {
        raise(me(), ": tuple is not supported");
      }

      VCSN_RAT_VISIT(zero,)
      {
        initial_ = res_->new_state();
        final_ = res_->new_state();
      }

      VCSN_RAT_VISIT(one,)
      {
        initial_ = res_->new_state();
        final_ = res_->new_state();
        res_->new_transition(initial_, final_, epsilon_);
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

      VCSN_RAT_VISIT(add, e)
      {
        state_t initial = res_->new_state();
        state_t final = res_->new_state();
        for (const auto& c: e)
          {
            c->accept(*this);
            res_->new_transition(initial, initial_, epsilon_);
            res_->new_transition(final_, final, epsilon_);
          }
        initial_ = initial;
        final_ = final;
      }

      VCSN_RAT_VISIT(mul, e)
      {
        e.head()->accept(*this);
        state_t initial = initial_;

        // Then the remainder.
        for (const auto& c: e.tail())
          {
            state_t final = final_;
            c->accept(*this);
            res_->new_transition(final, initial_, epsilon_);
          }
        initial_ = initial;
      }

      VCSN_RAT_VISIT(star, e)
      {
        e.sub()->accept(*this);
        state_t initial = res_->new_state();
        state_t final = res_->new_state();
        res_->new_transition(initial, initial_, epsilon_);
        res_->new_transition(final_,  final,    epsilon_);
        res_->new_transition(final_,  initial_, epsilon_);
        res_->new_transition(initial, final,    epsilon_);
        initial_ = initial;
        final_ = final;
      }

      VCSN_RAT_VISIT(lweight, e)
      {
        e.sub()->accept(*this);

        const weight_t& w = e.weight();
        for (auto t: out(res_, initial_))
          res_->set_weight(t, ws_.mul(w, res_->weight_of(t)));
      }

      VCSN_RAT_VISIT(rweight, e)
      {
        e.sub()->accept(*this);

        const weight_t& w = e.weight();
        for (auto t: in(res_, final_))
          res_->set_weight(t, ws_.mul(res_->weight_of(t), w));
      }

    private:
      const expressionset_t& rs_;
      const weightset_t& ws_ = *rs_.weightset();
      automaton_t res_;
      using label_t = label_t_of<automaton_t>;
      const label_t epsilon_ = res_->labelset()->one();
      state_t initial_ = automaton_t::element_type::null_state();
      state_t final_ = automaton_t::element_type::null_state();
    };
  } // rat::

  /// Build a Thompson automaton from an expression.
  ///
  /// \tparam Aut     relative to the generated automaton.
  /// \tparam ExpSet  relative to the expression.
  template <Automaton Aut, typename ExpSet>
  Aut
  thompson(const context_t_of<Aut>& ctx,
           const ExpSet& rs, const typename ExpSet::value_t& r)
  {
    auto thm = rat::thompson_visitor<Aut, ExpSet>{ctx, rs};
    return thm(r);
  }

  /// Build a Thompson automaton from an expression.
  ///
  /// \tparam Aut     relative to the generated automaton.
  /// \tparam ExpSet  relative to the expression.
  template <Automaton Aut, typename ExpSet>
  Aut
  thompson(const ExpSet& rs, const typename ExpSet::value_t& r)
  {
    auto thm = rat::thompson_visitor<Aut, ExpSet>{rs};
    return thm(r);
  }

  namespace dyn
  {
    namespace detail
    {
      /*---------------------.
      | dyn::thompson(exp).  |
      `---------------------*/

      /// Bridge.
      template <typename ExpSet>
      automaton
      thompson(const expression& exp)
      {
        // FIXME: So far, there is a single implementation of expressions,
        // but we should actually be parameterized by its type too.
        using expressionset_t = ExpSet;
        const auto& e = exp->as<expressionset_t>();
        auto ctx
          = vcsn::detail::make_nullableset_context(e.valueset().context());
        using ctx_t = decltype(ctx);
        using automaton_t = mutable_automaton<ctx_t>;
        return ::vcsn::thompson<automaton_t>(ctx, e.valueset(), e.value());
      }
    }
  }

} // vcsn::
