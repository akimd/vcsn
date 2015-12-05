#pragma once

#include <vcsn/algos/fwd.hh>
#include <vcsn/algos/strip.hh>
#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/core/rat/expression.hh>
#include <vcsn/core/rat/expressionset.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/misc/attributes.hh>

namespace vcsn
{

  /*-----------------------.
  | transpose(automaton).  |
  `-----------------------*/
  namespace detail
  {
    /// Read-write on an automaton, that transposes everything.
    template <typename Aut>
    class transpose_automaton_impl
      : public automaton_decorator<Aut>
    {
    public:
      /// The type of automaton to wrap.
      using automaton_t = Aut;

      using super_t = automaton_decorator<automaton_t>;
      using context_t = context_t_of<automaton_t>;

      /// The type to use to build an automaton of the same type:
      /// remove the inner const-volatile qualifiers, but still build
      /// a transpose_automaton.
      template <typename Ctx = context_t>
      using fresh_automaton_t
        = transpose_automaton<fresh_automaton_t_of<automaton_t, Ctx>>;

      using state_t = state_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;
      using label_t = label_t_of<automaton_t>;
      using weight_t = weight_t_of<automaton_t>;

      using labelset_t = labelset_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;
      using kind_t = typename automaton_t::element_type::kind_t;

      using super_t::super_t;
      using super_t::aut_;

      static symbol sname()
      {
        static auto res = symbol{"transpose_automaton<"
                          + automaton_t::element_type::sname() + '>'};
        return res;
      }

      std::ostream& print_set(std::ostream& o, format fmt = {}) const
      {
        o << "transpose_automaton<";
        aut_->print_set(o, fmt);
        return o << '>';
      }

      /// The automaton we are decorating (typically a mutable_automaton).
      automaton_t
      naked_automaton()
      {
        return aut_;
      }

      auto
      strip()
        -> decltype(transpose(vcsn::strip(this->naked_automaton())))
      {
        return transpose(vcsn::strip(this->naked_automaton()));
      }


      /*-------------------------------.
      | const methods that transpose.  |
      `-------------------------------*/
#define DEFINE(Signature, Value)                \
      auto                                      \
      Signature const                           \
      -> decltype(aut_->Value)                  \
      {                                         \
        return aut_->Value;                     \
      }

      DEFINE(all_in(state_t s),              all_out(s));
      template <typename Pred>
      DEFINE(all_in(state_t s, Pred pred),   all_out(s, pred));
      DEFINE(all_out(state_t s),             all_in(s));
      template <typename Pred>
      DEFINE(all_out(state_t s, Pred pred),  all_in(s, pred));

      DEFINE(dst_of(transition_t t),         src_of(t));
      DEFINE(final_transitions(),            initial_transitions());
      DEFINE(in(state_t s),                  out(s));
      DEFINE(initial_transitions(),          final_transitions());
      DEFINE(is_final(state_t s),            is_initial(s));
      DEFINE(num_finals(),                   num_initials());
      DEFINE(num_initials(),                 num_finals());
      DEFINE(out(state_t s),                 in(s));
      DEFINE(outin(state_t s, state_t d),    outin(d, s));
      DEFINE(src_of(transition_t t),         dst_of(t));
      DEFINE(is_initial(state_t s),          is_final(s));

      DEFINE(in(state_t s, label_t l),
             out(s, aut_->labelset()->transpose(l)));
      DEFINE(out(state_t s, label_t l),
             in(s, aut_->labelset()->transpose(l)));
      DEFINE(get_transition(state_t s, state_t d, label_t l),
             get_transition(d, s, aut_->labelset()->transpose(l)));
      DEFINE(has_transition(state_t s, state_t d, label_t l),
             has_transition(d, s, aut_->labelset()->transpose(l)));
      DEFINE(label_of(transition_t t),
             labelset()->transpose(aut_->label_of(t)));

      ATTRIBUTE_PURE
      DEFINE(get_initial_weight(state_t s),
             weightset()->transpose(aut_->get_final_weight(s)));

      ATTRIBUTE_PURE
      DEFINE(get_final_weight(state_t s),
             weightset()->transpose(aut_->get_initial_weight(s)));

      ATTRIBUTE_PURE
      DEFINE(weight_of(transition_t t),
             weightset()->transpose(aut_->weight_of(t)));

#undef DEFINE


      /*-----------------------------------.
      | non-const methods that transpose.  |
      `-----------------------------------*/

#define DEFINE(Signature, Value)                \
      auto                                      \
      Signature                                 \
        -> decltype(aut_->Value)                \
      {                                         \
        return aut_->Value;                     \
      }

      DEFINE(set_initial(state_t s),     set_final(s));
      DEFINE(set_final(state_t s),       set_initial(s));
      DEFINE(unset_initial(state_t s),   unset_final(s));
      DEFINE(unset_final(state_t s),     unset_initial(s));

      DEFINE(set_weight(transition_t t, weight_t k),
             set_weight(t, aut_->weightset()->transpose(k)));
      DEFINE(add_weight(transition_t t, weight_t k),
             add_weight(t, aut_->weightset()->transpose(k)));
      DEFINE(lmul_weight(transition_t t, weight_t k),
             lmul_weight(t, aut_->weightset()->transpose(k)));
      DEFINE(rmul_weight(transition_t t, weight_t k),
             rmul_weight(t, aut_->weightset()->transpose(k)));

      DEFINE(del_transition(transition_t t), del_transition(t));
      DEFINE(del_transition(state_t s, state_t d, label_t l),
             del_transition(d, s, aut_->labelset()->transpose(l)));

      DEFINE(add_transition(state_t s, state_t d, label_t l, weight_t k),
             add_transition(d, s,
                            aut_->labelset()->transpose(l),
                            aut_->weightset()->transpose(k)));
      DEFINE(add_transition(state_t s, state_t d, label_t l),
             add_transition(d, s, aut_->labelset()->transpose(l)));

      DEFINE(new_transition(state_t s, state_t d, label_t l, weight_t k),
             new_transition(d, s,
                            aut_->labelset()->transpose(l),
                            aut_->weightset()->transpose(k)));
      DEFINE(new_transition(state_t s, state_t d, label_t l),
             new_transition(d, s,
                            aut_->labelset()->transpose(l)));

      DEFINE(set_transition(state_t s, state_t d, label_t l, weight_t k),
             set_transition(d, s,
                            aut_->labelset()->transpose(l),
                            aut_->weightset()->transpose(k)));
      DEFINE(set_initial(state_t s, weight_t k),
             set_final(s, aut_->weightset()->transpose(k)));
      DEFINE(set_final(state_t s, weight_t k),
             set_initial(s, aut_->weightset()->transpose(k)));
      DEFINE(add_initial(state_t s, weight_t k),
             add_final(s, aut_->weightset()->transpose(k)));
      DEFINE(add_final(state_t s, weight_t k),
             add_initial(s, aut_->weightset()->transpose(k)));

      template <typename A>
      DEFINE(add_transition_copy(state_t src, state_t dst,
                                 const A& aut,
                                 typename A::element_type::transition_t t,
                                 bool transpose = false),
             add_transition_copy(dst, src, aut, t, !transpose));

      template <typename A>
      DEFINE(new_transition_copy(state_t src, state_t dst,
                                 const A& aut,
                                 typename A::element_type::transition_t t,
                                 bool transpose = false),
             new_transition_copy(dst, src, aut, t, !transpose));

#undef DEFINE



      /*-----------------------------------.
      | constexpr methods that transpose.  |
      `-----------------------------------*/

#define DEFINE(Signature, Value)                        \
      static constexpr                                  \
      auto                                              \
      Signature                                         \
        -> decltype(automaton_t::element_type::Value)   \
      {                                                 \
        return automaton_t::element_type::Value;        \
      }

      DEFINE(post(), pre());
      DEFINE(pre(), post());

#undef DEFINE
    };
  }

  template <typename Aut>
  inline
  Aut
  transpose(const transpose_automaton<Aut>& aut)
  {
    return aut->naked_automaton();
  }

  template <typename Aut>
  inline
  transpose_automaton<Aut>
  transpose(Aut aut)
  {
    return make_shared_ptr<transpose_automaton<Aut>>(aut);
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
        return make_automaton(vcsn::transpose(a));
      }
    }
  }


  /*------------------------------.
  | dyn::transpose(expression).   |
  `------------------------------*/
  namespace dyn
  {
    namespace detail
    {
      /// Bridge (transpose).
      template <typename ExpSet>
      expression
      transpose_expression(const expression& exp)
      {
        const auto& e = exp->as<ExpSet>();

        return make_expression(e.expressionset(),
                               ::vcsn::transpose(e.expressionset(),
                                                 e.expression()));
      }
    }
  }

  /*----------------------------------.
  | dyn::transposition(expression).   |
  `----------------------------------*/
  namespace dyn
  {
    namespace detail
    {
      /// Bridge (transposition).
      template <typename ExpSet>
      expression
      transposition_expression(const expression& exp)
      {
        const auto& e = exp->as<ExpSet>();

        return make_expression(e.expressionset(),
                               e.expressionset().transposition(e.expression()));
      }
    }
  }
} // vcsn::
