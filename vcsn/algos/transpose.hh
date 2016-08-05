#pragma once

#include <vcsn/algos/copy.hh>
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
    /// Read-write view on an automaton, that transposes everything.
    template <Automaton Aut>
    class transpose_automaton_impl
      : public automaton_decorator<Aut>
    {
    public:
      /// The type of automaton to wrap.
      using automaton_t = Aut;
      using self_t = transpose_automaton<automaton_t>;

      using super_t = automaton_decorator<automaton_t>;
      using context_t = context_t_of<automaton_t>;

      /// The transpose of a fresh non-transposed automaton.
      template <Automaton Fresh>
      struct fresh_impl_
      {
        using type = transpose_automaton<Fresh>;
      };

      /// The transpose of a fresh transposed automaton: the fresh type.
      template <Automaton Fresh>
      struct fresh_impl_<transpose_automaton<Fresh>>
      {
        using type = Fresh;
      };

      /// The type to use to build an automaton of the same type:
      /// remove the inner const-volatile qualifiers, and
      /// transpose_automaton if needed.
      template <typename Ctx = context_t>
      using fresh_automaton_t =
        typename fresh_impl_<fresh_automaton_t_of<automaton_t, Ctx>>::type;

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

      /// Strip the decoration of the wrapped automaton, but keep the
      /// transposition decoration.
      auto
      strip()
        -> decltype(transpose(vcsn::strip(this->naked_automaton())))
      {
        return transpose(vcsn::strip(this->naked_automaton()));
      }

      /// The transposed automaton without any decorator, not even the
      /// transposition decorator.
      auto automaton() const
      {
        return copy<const transpose_automaton_impl*, automaton_t>(this);
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
      DEFINE(all_out(state_t s),             all_in(s));
      DEFINE(dst_of(transition_t t),         src_of(t));
      DEFINE(is_final(state_t s),            is_initial(s));
      DEFINE(num_finals(),                   num_initials());
      DEFINE(num_initials(),                 num_finals());
      DEFINE(src_of(transition_t t),         dst_of(t));
      DEFINE(is_initial(state_t s),          is_final(s));

      DEFINE(is_lazy(state_t s),     is_lazy_in(s));
      DEFINE(is_lazy_in(state_t s),  is_lazy(s));

      DEFINE(get_transition(state_t s, state_t d, label_t l),
             get_transition(d, s, aut_->labelset()->transpose(l)));
      DEFINE(has_transition(transition_t t),
             has_transition(t));
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

      DEFINE(set_lazy(state_t s, bool l = true),     set_lazy_in(s, l));
      DEFINE(set_lazy_in(state_t s, bool l = true),  set_lazy(s, l));

      DEFINE(set_initial(state_t s),     set_final(s));
      DEFINE(set_final(state_t s),       set_initial(s));
      DEFINE(unset_initial(state_t s),   unset_final(s));
      DEFINE(unset_final(state_t s),     unset_initial(s));

      DEFINE(set_weight(transition_t t, weight_t k),
             set_weight(t, aut_->weightset()->transpose(k)));
      DEFINE(add_weight(transition_t t, weight_t k),
             add_weight(t, aut_->weightset()->transpose(k)));
      DEFINE(lweight(transition_t t, weight_t k),
             lweight(t, aut_->weightset()->transpose(k)));
      DEFINE(rweight(transition_t t, weight_t k),
             rweight(t, aut_->weightset()->transpose(k)));

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

      template <Automaton A>
      DEFINE(add_transition_copy(state_t src, state_t dst,
                                 const A& aut,
                                 transition_t_of<A> t,
                                 bool transpose = false),
             add_transition_copy(dst, src, aut, t, !transpose));

      template <Automaton A>
      DEFINE(new_transition_copy(state_t src, state_t dst,
                                 const A& aut,
                                 transition_t_of<A> t,
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

  /// The transpose of a transpose automaton is the original automaton.
  template <Automaton Aut>
  Aut
  transpose(const transpose_automaton<Aut>& aut)
  {
    return aut->naked_automaton();
  }

  /// Transpose of an automaton.
  template <Automaton Aut>
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
      template <Automaton Aut>
      automaton
      transpose(automaton& aut)
      {
        auto& a = aut->as<Aut>();
        return vcsn::transpose(a);
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
        return {e.valueset(), ::vcsn::transpose(e.valueset(), e.value())};
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
        return {e.valueset(), e.valueset().transposition(e.value())};
      }
    }
  }
} // vcsn::
