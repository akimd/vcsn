#pragma once

#include <vcsn/algos/constant-term.hh>
#include <vcsn/core/automaton.hh> // all_in
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/core/rat/visitor.hh>
#include <vcsn/ctx/fwd.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/misc/getargs.hh>
#include <vcsn/misc/raise.hh>

namespace vcsn
{
  /*---------------.
  | Function tag.  |
  `---------------*/

  CREATE_FUNCTION_TAG(zpc);

  namespace rat
  {
    /// Build a ZPC automaton from an expression.
    ///
    /// \tparam Aut     relative the generated automaton.
    /// \tparam ExpSet  relative to the expression.
    template <Automaton Aut,
              typename ExpSet>
    class zpc_visitor
      : public ExpSet::const_visitor
    {
      static_assert(labelset_t_of<Aut>::has_one(),
                    "zpc: requires nullable labels");

    public:
      using automaton_t = Aut;
      using expressionset_t = ExpSet;
      using context_t = context_t_of<automaton_t>;
      using weightset_t = weightset_t_of<expressionset_t>;
      using weight_t = weight_t_of<expressionset_t>;
      using state_t = state_t_of<automaton_t>;

      using super_t = typename expressionset_t::const_visitor;

      /// Name of this algorithm, for error messages.
      constexpr static const char* me() { return "zpc"; }

      /// Build an automaton of context \a ctx.
      zpc_visitor(const context_t& ctx,
                  const expressionset_t& rs,
                  bool compact)
        : rs_(rs)
        , res_(make_shared_ptr<automaton_t>(ctx))
        , compact_(compact)
      {}

      zpc_visitor(const expressionset_t& rs, bool compact)
        : zpc_visitor(rs.context(), rs, compact)
      {}

      automaton_t
      operator()(const typename expressionset_t::value_t& v)
      {
        try
          {
            v->accept(*this);
            res_->set_initial(initial_);
            res_->set_final(final_);

            for (auto t: all_in(res_, res_->post()))
              res_->set_weight(t, ws_.mul(res_->weight_of(t), final_weight_));

            res_->properties().update(zpc_ftag{});
            return std::move(res_);
          }
        catch (const std::runtime_error& e)
          {
            raise(e,
                  "  while computing ZPC automaton of: ", to_string(rs_, v));
          }
      }

    private:
      VCSN_RAT_UNSUPPORTED(complement)
      VCSN_RAT_UNSUPPORTED(compose)
      VCSN_RAT_UNSUPPORTED(conjunction)
      VCSN_RAT_UNSUPPORTED(infiltrate)
      VCSN_RAT_UNSUPPORTED(ldivide)
      VCSN_RAT_UNSUPPORTED(shuffle)
      VCSN_RAT_UNSUPPORTED(transposition)
      using tuple_t = typename super_t::tuple_t;
      void visit(const tuple_t&, std::true_type) override
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

        res_->set_final(initial_);
      }

      VCSN_RAT_VISIT(name, e)
      {
        super_t::visit(e);
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
        if (compact_)
          add_compact(e);
        else
          add_regular(e);
      }

      VCSN_RAT_VISIT(mul, e)
      {
        if (compact_)
          prod_compact(e);
        else
          prod_regular(e);
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
        for (auto t: all_out(res_, initial_))
          res_->set_weight(t, ws_.mul(w, res_->weight_of(t)));
      }

      VCSN_RAT_VISIT(rweight, e)
      {
        e.sub()->accept(*this);

        final_weight_ = ws_.mul(e.weight(), final_weight_);
      }

      void add_regular(const add_t& e)
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

      void prod_regular(const mul_t& e)
      {
        std::vector<state_t> state_stack;

        // Number of 'initial' states to create.
        for (auto i = 0; i < e.size() - 1; ++i)
          state_stack.push_back(res_->new_state());

        e.head()->accept(*this);

        // In each methods prod_regular, prod_compact and add_compact,
        // sometimes it's necessary to create simultaneously
        // two sub-automaton, the first one's states are postfixes by _e and
        // the next one (creation order) by _f.
        state_t initial_e = initial_;
        state_t final_e = final_;

        // Default value of initial for the compact form.
        state_t initial = initial_e;
        state_t final = automaton_t::element_type::null_state();

        initial = state_stack.back();
        state_stack.pop_back();
        res_->new_transition(initial, initial_e, epsilon_);

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
      }

      void add_compact(const add_t& e)
      {
        e.head()->accept(*this);

        weight_t cst = res_->get_final_weight(initial_);
        res_->unset_final(initial_);

        state_t initial = initial_;
        state_t initial_e = initial;
        state_t final_e = final_;

        for (auto c: e.tail())
          {
            c->accept(*this);
            cst = ws_.add(cst, res_->get_final_weight(initial_));
            res_->unset_final(initial_);

            res_->new_transition(initial_e, initial_, epsilon_);
            res_->new_transition(final_e, final_, epsilon_);

            initial_e = initial_;
            final_e = final_;
          }

        initial_ = initial;

        res_->set_final(initial_, cst);
      }

      void prod_compact(const mul_t& e)
      {
        e.head()->accept(*this);

        state_t initial_e = initial_;
        state_t final_e = final_;

        // Default value of initial for the compact form.
        state_t initial = initial_e;
        state_t final;

        for (auto t: e.tail())
          {
            t->accept(*this);
            final = final_;

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
          }

        initial_ = initial;
        final_ = final;
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
      /// Whether to build the "compact" version of the ZPC automaton.
      const bool compact_;
    };
  } // rat::

  /// Build a ZPC automaton from an expression.
  ///
  /// \tparam Aut     relative to the generated automaton.
  /// \tparam ExpSet  relative to the expression.
  template <Automaton Aut, typename ExpSet>
  Aut
  zpc(const context_t_of<Aut>& ctx,
      const ExpSet& rs,
      const typename ExpSet::value_t& r,
      const std::string& algo = "auto")
  {
    static const auto map = getarg<bool>
      {
        "zpc version",
        {
          // name,    compact.
          // Beware of the conversion from const char* to bool here.
          // http://stackoverflow.com/questions/13268608/.
          {"auto",    std::string("regular")},
          {"compact", true},
          {"regular", false},
        }
      };
    auto zpc = rat::zpc_visitor<Aut, ExpSet>{ctx, rs, map[algo]};
    return zpc(r);
  }

  namespace dyn
  {
    namespace detail
    {
      /*-----------------.
      | dyn::zpc(exp).   |
      `-----------------*/

      /// Bridge.
      template <typename ExpSet, typename String>
      automaton
      zpc(const expression& exp, const std::string& algo)
      {
        // FIXME: So far, there is a single implementation of expressions,
        // but we should actually be parameterized by its type too.
        using expressionset_t = ExpSet;
        const auto& e = exp->as<expressionset_t>();
        auto ctx = e.valueset().context();
        using ctx_t = decltype(ctx);
        using automaton_t = mutable_automaton<ctx_t>;
        return ::vcsn::zpc<automaton_t>(ctx, e.valueset(), e.value(), algo);
      }
    }
  }
} // vcsn::
