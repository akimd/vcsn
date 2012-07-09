#ifndef VCSN_ALGOS_TRANSPOSE_HH
# define VCSN_ALGOS_TRANSPOSE_HH

# include <vcsn/ctx/ctx.hh>
# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/core/rat/kratexpset.hh>
# include <vcsn/core/rat/kratexp.hh>

namespace vcsn
{

  namespace details
  {
    template <typename Aut>
    class transpose_automaton
    {
    public:
      /// The type of automaton to wrap.
      using automaton_t = Aut;
      using context_t = typename automaton_t::context_t;
      using state_t = typename automaton_t::state_t;
      using transition_t = typename automaton_t::transition_t;
      using label_t = typename automaton_t::label_t;
      using entry_t = typename automaton_t::entry_t;
      using weight_t = typename automaton_t::weight_t;

    private:
      automaton_t& aut_;

    public:
      transpose_automaton(automaton_t& aut)
        : aut_(aut)
      {}

      /*-------------------------------.
      | const methods that transpose.  |
      `-------------------------------*/
# define DEFINE(Signature, Value)               \
      auto                                      \
      Signature const                           \
        -> decltype(aut_.Value)                 \
      {                                         \
        return aut_.Value;                      \
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
             out(s, aut_.genset()->transpose(l)));
      DEFINE(out(state_t s, label_t l),
             in(s, aut_.genset()->transpose(l)));
      DEFINE(get_transition(state_t s, state_t d, label_t l),
             get_transition(d, s, aut_.genset()->transpose(l)));
      DEFINE(has_transition(state_t s, state_t d, label_t l),
             has_transition(d, s, aut_.genset()->transpose(l)));
      DEFINE(label_of(transition_t t),
             genset()->transpose(aut_.label_of(t)));
      DEFINE(word_label_of(transition_t t),
             genset()->transpose(aut_.word_label_of(t)));

      DEFINE(get_initial_weight(state_t s),
             weightset()->transpose(aut_.get_final_weight(s)));
      DEFINE(get_final_weight(state_t s),
             weightset()->transpose(aut_.get_initial_weight(s)));
      DEFINE(weight_of(transition_t t),
             weightset()->transpose(aut_.weight_of(t)));

      DEFINE(entry_at(state_t s, state_t d),
             entryset().transpose(aut_.entry_at(d, s)));

# undef DEFINE

      // Same text, but not the same src_of and dst_of.
      entry_t
      entry_at(transition_t t) const
      {
        return entry_at(src_of(t), dst_of(t));
      }


      /*-----------------------------------.
      | non-const methods that transpose.  |
      `-----------------------------------*/

#if 0
# define DEFINE(Signature, Value)               \
      auto                                      \
      Signature                                 \
        -> decltype(aut_.Value)                 \
      {                                         \
        return aut_.Value;                      \
      }

      DEFINE(del_transition(state_t s, state_t d, label_t l),
             del_transition(d, s, l));
      DEFINE(add_transition(state_t s, state_t d, label_t l, weight_t k),
             add_transition(d, s, l, k));
      DEFINE(add_transition(state_t s, state_t d, label_t l),
             add_transition(d, s, l));
      DEFINE(set_transition(state_t s, state_t d, label_t l, weight_t k),
             set_transition(d, s, l, k));
      DEFINE(set_initial(state_t s),         set_final(s));
      DEFINE(set_final(state_t s),           set_initial(s));
      DEFINE(set_initial(state_t s, weight_t k), set_final(s, k));
      DEFINE(set_final(state_t s, weight_t k),   set_initial(s, k));
      DEFINE(add_initial(state_t s, weight_t k), add_final(s, k));
      DEFINE(add_final(state_t s, weight_t k),   add_initial(s, k));
      DEFINE(unset_initial(state_t s),       unset_final(s));
      DEFINE(unset_final(state_t s),         unset_initial(s));

# undef DEFINE
#endif

      /*-----------------------------------.
      | constexpr methods that transpose.  |
      `-----------------------------------*/

      static constexpr
      auto
      post()
        -> decltype(aut_.pre())
      {
        return automaton_t::pre();
      }

      static constexpr
      auto
      pre()
        -> decltype(aut_.post())
      {
        return automaton_t::post();
      }

      /*--------------------------.
      | forwarded const methods.  |
      `--------------------------*/

# define DEFINE(Name)                           \
      auto                                      \
      Name() const                              \
        -> decltype(aut_.Name())                \
      {                                         \
        return aut_.Name();                     \
      }

      DEFINE(all_entries);
      DEFINE(all_states);
      DEFINE(all_transitions);
      DEFINE(context);
      DEFINE(entryset);
      DEFINE(genset);
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

} // vcsn::

#endif // !VCSN_ALGOS_TRANSPOSE_HH
