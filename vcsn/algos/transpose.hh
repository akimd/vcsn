#ifndef VCSN_ALGOS_TRANSPOSE_HH
# define VCSN_ALGOS_TRANSPOSE_HH

# include <vcsn/algos/copy.hh>
# include <vcsn/core/rat/ratexp.hh>
# include <vcsn/core/rat/ratexpset.hh>
# include <vcsn/ctx/context.hh>
# include <vcsn/misc/attributes.hh>

namespace vcsn
{

  namespace detail
  {
    /// Read-write on an automaton, that transposes everything.
    template <typename Aut>
    class transpose_automaton
    {
    public:
      /// The type of automaton to wrap.
      using automaton_t = Aut;

      /// The type of the automata to produce from this kind o
      /// automata.  For instance, determinizing a
      /// transpose_automaton<const mutable_automaton<Ctx>> should
      /// yield a transpose_automaton<mutable_automaton<Ctx>>, without
      /// the "inner" const.
      using self_nocv_t
        = transpose_automaton<typename automaton_t::self_nocv_t>;
      using context_t = typename automaton_t::context_t;
      using state_t = typename automaton_t::state_t;
      using transition_t = typename automaton_t::transition_t;
      using label_t = typename automaton_t::label_t;
      using weight_t = typename automaton_t::weight_t;

      using labelset_t = typename automaton_t::labelset_t;
      using weightset_t = typename automaton_t::weightset_t;
      using kind_t = typename automaton_t::kind_t;

      using labelset_ptr = typename automaton_t::labelset_ptr;
      using weightset_ptr = typename automaton_t::weightset_ptr;

    public:
      transpose_automaton(automaton_t& aut)
        : aut_{&aut}
      {}

      transpose_automaton(transpose_automaton&& aut)
      {
        std::swap(aut_, aut.aut_);
      }

      /// Forward constructor.
      template <typename... Args>
      transpose_automaton(Args&&... args)
        : aut_{new automaton_t{std::forward<Args>(args)...}}
      {}

      transpose_automaton& operator=(transpose_automaton&& that)
      {
        if (this != &that)
          *aut_ = std::move(*that.aut_);
        return *this;
      }

      automaton_t*
      original_automaton()
      {
        return aut_;
      }

      static std::string sname()
      {
        return "transpose_automaton<" + automaton_t::sname() + ">";
      }

      std::string vname(bool full = true) const
      {
        return "transpose_automaton<" + aut_->vname(full) + ">";
      }

    private:
      /// Used to workaround issues with decltype, see the const_casts
      /// above, and http://stackoverflow.com/questions/17111406.
      using automaton_nocv_t = typename std::remove_cv<automaton_t>::type;

      /// The wrapped automaton, possibly const.
      // Must be defined early to please decltype.
      automaton_t* aut_;

    public:
      /*-------------------------------.
      | const methods that transpose.  |
      `-------------------------------*/
# define DEFINE(Signature, Value)               \
      auto                                      \
      Signature const                           \
      -> decltype(aut_->Value)                  \
      {                                         \
        return aut_->Value;                     \
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

# undef DEFINE


      /*-----------------------------------.
      | non-const methods that transpose.  |
      `-----------------------------------*/

# define DEFINE(Signature, Value)                               \
      auto                                                      \
      Signature                                                 \
        -> decltype(const_cast<automaton_nocv_t*>(aut_)->Value) \
      {                                                         \
        return aut_->Value;                                     \
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


      // Forwarded, does not transpose.
      DEFINE(del_state(state_t s),       del_state(s));
      DEFINE(new_state(),                new_state());

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
      DEFINE(null_state(), null_state());
      DEFINE(null_transition(), null_transition());

#undef DEFINE

      /*--------------------------.
      | forwarded const methods.  |
      `--------------------------*/

# define DEFINE(Name)                                           \
      template <typename... Args>                               \
      auto                                                      \
      Name(Args&&... args) const                                \
        -> decltype(aut_->Name(std::forward<Args>(args)...))    \
      {                                                         \
        return aut_->Name(std::forward<Args>(args)...);         \
      }

      DEFINE(all_states);
      DEFINE(all_transitions);
      DEFINE(context);
      DEFINE(labelset);
      DEFINE(num_all_states);
      DEFINE(num_finals);
      DEFINE(num_initials);
      DEFINE(num_states);
      DEFINE(num_transitions);
      DEFINE(states);
      DEFINE(transitions);
      DEFINE(weightset);
# undef DEFINE
    };
  }

  template <class Aut>
  typename detail::transpose_automaton<Aut>
  transpose(Aut& aut)
  {
    return detail::transpose_automaton<Aut>{aut};
  }



  namespace dyn
  {
    namespace detail
    {
      /*----------------------------.
      | dyn::transpose(automaton).  |
      `----------------------------*/
      template <typename Aut>
      automaton
      transpose(automaton& aut)
      {
        auto& a = aut->as<Aut>();
        return make_automaton<Aut,
                              vcsn::detail::transpose_automaton<Aut>>(std::move(vcsn::copy(a)));
      }

      REGISTER_DECLARE(transpose,
                       (automaton& aut) -> automaton);


      /*-------------------------.
      | dyn::transpose(ratexp).  |
      `-------------------------*/
      /// Bridge.
      template <typename RatExpSet>
      ratexp
      transpose_ratexp(const ratexp& exp)
      {
        const auto& e = exp->as<RatExpSet>();

        return make_ratexp(e.get_ratexpset(),
                           transpose(e.get_ratexpset(),
                                     e.ratexp()));
      }

      REGISTER_DECLARE(transpose_ratexp,
                       (const ratexp& e) -> ratexp);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_TRANSPOSE_HH
