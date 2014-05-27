#ifndef VCSN_ALGOS_TRANSPOSE_HH
# define VCSN_ALGOS_TRANSPOSE_HH

# include <vcsn/core/automaton-decorator.hh>
# include <vcsn/core/rat/ratexp.hh>
# include <vcsn/core/rat/ratexpset.hh>
# include <vcsn/ctx/context.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/misc/attributes.hh>

namespace vcsn
{

  /*-----------------------.
  | transpose(automaton).  |
  `-----------------------*/
  namespace detail
  {
    /// Read-write on an automaton, that transposes everything.
    template <typename AutPtr>
    class transpose_automaton_impl : public automaton_decorator<AutPtr>
    {
    public:
      /// The type of automaton to wrap.
      using automaton_ptr = AutPtr;
      using automaton_t = typename automaton_ptr::element_type;
      using super = automaton_decorator<AutPtr>;
      using automaton_nocv_t = typename super::automaton_nocv_t;

      using context_t = context_t_of<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;
      using label_t = label_t_of<automaton_t>;
      using weight_t = weight_t_of<automaton_t>;

      using labelset_t = labelset_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;
      using kind_t = typename automaton_t::kind_t;

      using labelset_ptr = typename automaton_t::labelset_ptr;
      using weightset_ptr = typename automaton_t::weightset_ptr;

    public:
      using super::super;

      static std::string sname()
      {
        return "transpose_automaton<" + automaton_t::sname() + ">";
      }

      std::string vname(bool full = true) const
      {
        return "transpose_automaton<" + this->aut_->vname(full) + ">";
      }

      /*-------------------------------.
      | const methods that transpose.  |
      `-------------------------------*/
# define DEFINE(Signature, Value)               \
      auto                                      \
      Signature const                           \
      -> decltype(this->aut_->Value)            \
      {                                         \
        return this->aut_->Value;               \
      }

      DEFINE(is_initial(state_t s),          is_final(s));
      DEFINE(is_final(state_t s),            is_initial(s));
      DEFINE(all_in(state_t s),              all_out(s));
      DEFINE(all_out(state_t s),             all_in(s));
      DEFINE(in(state_t s),                  out(s));
      DEFINE(out(state_t s),                 in(s));
      DEFINE(outin(state_t s, state_t d),    outin(d, s));
      DEFINE(src_of(transition_t t),         dst_of(t));
      DEFINE(dst_of(transition_t t),         src_of(t));
      DEFINE(initial_transitions(),          final_transitions());
      DEFINE(final_transitions(),            initial_transitions());

      DEFINE(in(state_t s, label_t l),
             out(s, this->aut_->labelset()->transpose(l)));
      DEFINE(out(state_t s, label_t l),
             in(s, this->aut_->labelset()->transpose(l)));
      DEFINE(get_transition(state_t s, state_t d, label_t l),
             get_transition(d, s, this->aut_->labelset()->transpose(l)));
      DEFINE(has_transition(state_t s, state_t d, label_t l),
             has_transition(d, s, this->aut_->labelset()->transpose(l)));
      DEFINE(label_of(transition_t t),
             labelset()->transpose(this->aut_->label_of(t)));

      ATTRIBUTE_PURE
      DEFINE(get_initial_weight(state_t s),
             weightset()->transpose(this->aut_->get_final_weight(s)));

      ATTRIBUTE_PURE
      DEFINE(get_final_weight(state_t s),
             weightset()->transpose(this->aut_->get_initial_weight(s)));

      ATTRIBUTE_PURE
      DEFINE(weight_of(transition_t t),
             weightset()->transpose(this->aut_->weight_of(t)));

# undef DEFINE


      /*-----------------------------------.
      | non-const methods that transpose.  |
      `-----------------------------------*/

# define DEFINE(Signature, Value)               \
      auto                                      \
      Signature                                 \
        -> decltype(this->aut_->Value)          \
      {                                         \
        return this->aut_->Value;               \
      }

      DEFINE(set_initial(state_t s),     set_final(s));
      DEFINE(set_final(state_t s),       set_initial(s));
      DEFINE(unset_initial(state_t s),   unset_final(s));
      DEFINE(unset_final(state_t s),     unset_initial(s));

      DEFINE(set_weight(transition_t t, weight_t k),
             set_weight(t, this->aut_->weightset()->transpose(k)));
      DEFINE(add_weight(transition_t t, weight_t k),
             add_weight(t, this->aut_->weightset()->transpose(k)));
      DEFINE(lmul_weight(transition_t t, weight_t k),
             lmul_weight(t, this->aut_->weightset()->transpose(k)));
      DEFINE(rmul_weight(transition_t t, weight_t k),
             rmul_weight(t, this->aut_->weightset()->transpose(k)));

      DEFINE(del_transition(transition_t t), del_transition(t));
      DEFINE(del_transition(state_t s, state_t d, label_t l),
             del_transition(d, s, this->aut_->labelset()->transpose(l)));

      DEFINE(add_transition(state_t s, state_t d, label_t l, weight_t k),
             add_transition(d, s,
                            this->aut_->labelset()->transpose(l),
                            this->aut_->weightset()->transpose(k)));
      DEFINE(add_transition(state_t s, state_t d, label_t l),
             add_transition(d, s, this->aut_->labelset()->transpose(l)));

      DEFINE(new_transition(state_t s, state_t d, label_t l, weight_t k),
             new_transition(d, s,
                            this->aut_->labelset()->transpose(l),
                            this->aut_->weightset()->transpose(k)));
      DEFINE(new_transition(state_t s, state_t d, label_t l),
             new_transition(d, s,
                            this->aut_->labelset()->transpose(l)));

      DEFINE(set_transition(state_t s, state_t d, label_t l, weight_t k),
             set_transition(d, s,
                            this->aut_->labelset()->transpose(l),
                            this->aut_->weightset()->transpose(k)));
      DEFINE(set_initial(state_t s, weight_t k),
             set_final(s, this->aut_->weightset()->transpose(k)));
      DEFINE(set_final(state_t s, weight_t k),
             set_initial(s, this->aut_->weightset()->transpose(k)));
      DEFINE(add_initial(state_t s, weight_t k),
             add_final(s, this->aut_->weightset()->transpose(k)));
      DEFINE(add_final(state_t s, weight_t k),
             add_initial(s, this->aut_->weightset()->transpose(k)));

      template <typename A>
      DEFINE(add_transition_copy(const A& aut, state_t src,
                                 state_t dst, transition_t t, weight_t k,
                                 bool transpose = false),
             add_transition_copy(aut,
                                 dst, src, t,
                                 this->aut_->weightset()->transpose(k),
                                 !transpose));

      template <typename A>
      DEFINE(new_transition_copy(const A& aut, state_t src,
                                 state_t dst, transition_t t, weight_t k,
                                 bool transpose = false),
             new_transition_copy(aut,
                                 dst, src, t,
                                 this->aut_->weightset()->transpose(k),
                                 !transpose));

# undef DEFINE



      /*-----------------------------------.
      | constexpr methods that transpose.  |
      `-----------------------------------*/

# define DEFINE(Signature, Value)               \
      static constexpr                          \
      auto                                      \
      Signature                                 \
        -> decltype(automaton_t::Value)         \
      {                                         \
        return automaton_t::Value;              \
      }

      DEFINE(post(), pre());
      DEFINE(pre(), post());

#undef DEFINE
    };
  }

  template <typename AutPtr>
  using transpose_automaton
    = std::shared_ptr<detail::transpose_automaton_impl<AutPtr>>;

  template <typename AutPtr>
  transpose_automaton<AutPtr>
  transpose(AutPtr& aut)
  {
    return std::make_shared<detail::transpose_automaton_impl<AutPtr>>(aut);
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      automaton
      transpose(automaton& aut)
      {
        auto& a = aut->as<Aut>();
        return make_automaton(transpose(a));
      }

      REGISTER_DECLARE(transpose,
                       (automaton& aut) -> automaton);
    }
  }


  /*-------------------------.
  | dyn::transpose(ratexp).  |
  `-------------------------*/
  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSet>
      ratexp
      transpose_ratexp(const ratexp& exp)
      {
        const auto& e = exp->as<RatExpSet>();

        return make_ratexp(e.ratexpset(),
                           transpose(e.ratexpset(),
                                     e.ratexp()));
      }

      REGISTER_DECLARE(transpose_ratexp,
                       (const ratexp& e) -> ratexp);
    }
  }

  /*-----------------------------.
  | dyn::transposition(ratexp).  |
  `-----------------------------*/
  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSet>
      ratexp
      transposition_ratexp(const ratexp& exp)
      {
        const auto& e = exp->as<RatExpSet>();

        return make_ratexp(e.ratexpset(),
                           e.ratexpset().transposition(e.ratexp()));
      }

      REGISTER_DECLARE(transposition_ratexp,
                       (const ratexp& e) -> ratexp);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_TRANSPOSE_HH
