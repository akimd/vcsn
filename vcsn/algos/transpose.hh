#ifndef VCSN_ALGOS_TRANSPOSE_HH
# define VCSN_ALGOS_TRANSPOSE_HH

# include <vcsn/ctx/ctx.hh>
# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/core/rat/ratexpset.hh>
# include <vcsn/core/rat/ratexp.hh>

namespace vcsn
{

  namespace details
  {
    template <typename Aut>
    class transpose_automaton: public dyn::abstract_automaton
    {
    public:
      /// The type of automaton to wrap.
      using super_t = dyn::abstract_automaton;
      using automaton_t = Aut;
      using context_t = typename automaton_t::context_t;
      using state_t = typename automaton_t::state_t;
      using transition_t = typename automaton_t::transition_t;
      using label_t = typename automaton_t::label_t;
      using entry_t = typename automaton_t::entry_t;
      using weight_t = typename automaton_t::weight_t;

    private:
      automaton_t* aut_;

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

      automaton_t*
      original_automaton()
      {
        return aut_;
      }

      static std::string sname()
      {
        return "transpose_automaton<" + automaton_t::sname() + ">";
      }

      virtual std::string vname(bool full = true) const override
      {
        return "transpose_automaton<" + aut_->vname(full) + ">";
      }

      /*-------------------------------.
      | const methods that transpose.  |
      `-------------------------------*/
# define DEFINE(Signature, Value)               \
      auto                                      \
      Signature const                           \
        -> decltype(aut_->Value)                \
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
      DEFINE(word_label_of(transition_t t),
             labelset()->transpose(aut_->word_label_of(t)));

      DEFINE(get_initial_weight(state_t s),
             weightset()->transpose(aut_->get_final_weight(s)));
      DEFINE(get_final_weight(state_t s),
             weightset()->transpose(aut_->get_initial_weight(s)));
      DEFINE(weight_of(transition_t t),
             weightset()->transpose(aut_->weight_of(t)));

      DEFINE(entry_at(state_t s, state_t d),
             entryset().transpose(aut_->entry_at(d, s)));

# undef DEFINE

      // Same text, but not the same src_of and dst_of.
      entry_t
      entry_at(transition_t t) const
      {
        return entry_at(src_of(t), dst_of(t));
      }


      /*--------------------.
      | non-const methods.  |
      `--------------------*/

# define DEFINE(Signature, Value)               \
      auto                                      \
      Signature                                 \
        -> decltype(aut_->Value)                \
      {                                         \
        return aut_->Value;                     \
      }

      /*-----------------------------------.
      | non-const methods that transpose.  |
      `-----------------------------------*/

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
        -> decltype(aut_->Value)                \
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

# define DEFINE(Name)                           \
      auto                                      \
      Name() const                              \
        -> decltype(aut_->Name())               \
      {                                         \
        return aut_->Name();                    \
      }

      DEFINE(all_entries);
      DEFINE(all_states);
      DEFINE(all_transitions);
      DEFINE(context);
      DEFINE(entryset);
      DEFINE(labelset);
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
  typename details::transpose_automaton<Aut>
  transpose(Aut& aut)
  {
    return details::transpose_automaton<Aut>{aut};
  }


  /*-----------------.
  | dyn::transpose.  |
  `-----------------*/

  namespace dyn
  {
    namespace details
    {
      /* automaton. */
      template <typename Aut>
      automaton
      transpose(automaton& aut)
      {
        return std::make_shared<vcsn::details::transpose_automaton<Aut>>
          (dynamic_cast<Aut&>(*aut));
      }

      using transpose_t = auto (automaton& aut) -> automaton;

      bool transpose_register(const std::string& ctx, const transpose_t& fn);


      /* ratexp. */
      template <typename Context>
      ratexp
      abstract_transpose_exp(const ratexp& e)
      {
        const auto& exp =
          std::dynamic_pointer_cast<const typename Context::node_t>(e->ratexp());
        auto res = transpose(dynamic_cast<const Context&>(e->ctx()), exp);
        return make_ratexp(e->ctx(), res);
      }

      using transpose_exp_t = auto (const ratexp& e) -> ratexp;

      bool transpose_exp_register(const std::string& ctx,
                                  const transpose_exp_t& fn);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_TRANSPOSE_HH
