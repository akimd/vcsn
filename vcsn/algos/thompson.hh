#ifndef VCSN_ALGOS_THOMPSON_HH
# define VCSN_ALGOS_THOMPSON_HH

# include <vcsn/ctx/fwd.hh>
# include <vcsn/ctx/traits.hh>
# include <vcsn/core/mutable-automaton.hh>
# include <vcsn/core/rat/visitor.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/ratexp.hh>
# include <vcsn/misc/raise.hh>

namespace vcsn
{
  namespace rat
  {
    /// Build a Thompson automaton from a ratexp.
    ///
    /// \tparam Aut        relative the generated automaton
    /// \tparam RatExpSet  relative to the RatExp.
    template <typename Aut,
              typename RatExpSet>
    class thompson_visitor
      : public RatExpSet::const_visitor
    {
    public:
      using automaton_t = Aut;
      using ratexpset_t = RatExpSet;
      using weightset_t = weightset_t_of<ratexpset_t>;
      using weight_t = weight_t_of<ratexpset_t>;
      using state_t = state_t_of<automaton_t>;

      using super_type = typename ratexpset_t::const_visitor;

      static_assert(labelset_t_of<Aut>::has_one(),
                    "thompson: requires nullable labels");

      constexpr static const char* me() { return "thompson"; }

      thompson_visitor(const ratexpset_t& rs)
        : rs_(rs)
        , res_(make_shared_ptr<automaton_t>(rs_.context()))
      {}

      automaton_t
      operator()(const typename ratexpset_t::value_t& v)
      {
        v->accept(*this);
        res_->set_initial(initial_);
        res_->set_final(final_);
        return std::move(res_);
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
        res_->new_transition(initial_, final_, e.value());
      }

      VCSN_RAT_VISIT(sum, e)
      {
        state_t initial = res_->new_state();
        state_t final = res_->new_state();
        for (auto c: e)
          {
            c->accept(*this);
            res_->new_transition(initial, initial_, epsilon_);
            res_->new_transition(final_, final, epsilon_);
          }
        initial_ = initial;
        final_ = final;
      }

      VCSN_RAT_UNSUPPORTED(complement)
      VCSN_RAT_UNSUPPORTED(conjunction)
      VCSN_RAT_UNSUPPORTED(ldiv)
      VCSN_RAT_UNSUPPORTED(shuffle)
      VCSN_RAT_UNSUPPORTED(transposition)

      VCSN_RAT_VISIT(prod, e)
      {
        e.head()->accept(*this);
        state_t initial = initial_;

        // Then the remainder.
        for (auto c: e.tail())
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
        for (auto t: res_->out(initial_))
          res_->set_weight(t, ws_.mul(w, res_->weight_of(t)));
      }

      VCSN_RAT_VISIT(rweight, e)
      {
        e.sub()->accept(*this);

        const weight_t& w = e.weight();
        for (auto t: res_->in(final_))
          res_->set_weight(t, ws_.mul(res_->weight_of(t), w));
      }

    private:
      const ratexpset_t& rs_;
      const weightset_t& ws_ = *rs_.weightset();
      automaton_t res_;
      using label_t = label_t_of<automaton_t>;
      const label_t epsilon_ = res_->labelset()->one();
      state_t initial_ = automaton_t::element_type::null_state();
      state_t final_ = automaton_t::element_type::null_state();
    };

  } // rat::

  /// Build a Thompson automaton from a ratexp.
  ///
  /// \tparam Aut        relative to the generated automaton.
  /// \tparam RatExpSet  relative to the RatExp.
  template <typename Aut,
            typename RatExpSet>
  Aut
  thompson(const RatExpSet& rs, const typename RatExpSet::value_t& r)
  {
    rat::thompson_visitor<Aut, RatExpSet> thompson{rs};
    return thompson(r);
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
        using ratexpset_t = RatExpSet;
        using automaton_t = mutable_automaton<context_t_of<ratexpset_t>>;
        const auto& e = exp->as<ratexpset_t>();
        return make_automaton(::vcsn::thompson<automaton_t>(e.ratexpset(),
                                                            e.ratexp()));
      }

      REGISTER_DECLARE(thompson,
                       (const ratexp& e) -> automaton);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_THOMPSON_HH
