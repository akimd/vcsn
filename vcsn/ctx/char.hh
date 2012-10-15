#ifndef VCSN_CTX_CHAR_HH
# define VCSN_CTX_CHAR_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/ctx/ctx.hh>

namespace vcsn
{
  namespace ctx
  {
    template <typename WeightSet, typename Kind = labels_are_letters>
    using char_ = context<vcsn::set_alphabet<vcsn::char_letters>,
                          WeightSet,
                          Kind>;
  }


# define VCSN_CTX_INSTANTIATE_DOTTY(Aut)                                \
  MAYBE_EXTERN template                                                 \
  void dotty<Aut>(const Aut& aut, std::ostream& out);                   \
                                                                        \
  MAYBE_EXTERN template                                                 \
  std::string dotty<Aut>(const Aut& aut);                               \
                                                                        \
  MAYBE_EXTERN template                                                 \
  void abstract_dotty<Aut>(const dyn::abstract_automaton& aut,       \
                           std::ostream& out);                          \
                                                                        \
  MAYBE_EXTERN template                                                 \
  std::string abstract_dotty<Aut>(const dyn::abstract_automaton& aut);

  /*-------------------------------------------------------.
  | Instantiate the function that work for every context.  |
  `-------------------------------------------------------*/

# define VCSN_CTX_INSTANTIATE_1(Ctx)                                    \
  MAYBE_EXTERN template                                                 \
  class polynomialset<Ctx>;                                             \
                                                                        \
  MAYBE_EXTERN template                                                 \
  class mutable_automaton<Ctx>;                                         \
                                                                        \
  /* aut_to_exp. */                                                     \
  MAYBE_EXTERN template                                                 \
  Ctx::kratexp_t                                                        \
  aut_to_exp<mutable_automaton<Ctx>>                                    \
  (const mutable_automaton<Ctx>& aut,                                   \
    const state_chooser_t<mutable_automaton<Ctx>>& next_state);         \
                                                                        \
  /* dotty. */                                                          \
  VCSN_CTX_INSTANTIATE_DOTTY(mutable_automaton<Ctx>);                   \
  VCSN_CTX_INSTANTIATE_DOTTY                                            \
  (details::transpose_automaton<mutable_automaton<Ctx>>);               \
                                                                        \
  /* lift. */                                                           \
  MAYBE_EXTERN template                                                 \
  details::lifted_automaton_t<mutable_automaton<Ctx>>                   \
  lift<mutable_automaton<Ctx>>(const mutable_automaton<Ctx>& aut);      \
                                                                        \
  /* standard_of. */                                                    \
  MAYBE_EXTERN template                                                 \
  class rat::standard_of_visitor<mutable_automaton<Ctx>>;               \
                                                                        \
  /* transpose. */                                                      \
  MAYBE_EXTERN template                                                 \
  class details::transpose_automaton<mutable_automaton<Ctx>>;           \
  MAYBE_EXTERN template                                                 \
  class details::transposer<Ctx>;



  /*----------------------------------.
  | Register the abstract functions.  |
  `----------------------------------*/

# if VCSN_INSTANTIATION
#  define VCSN_CTX_INSTANTIATE_2(Ctx)                                   \
  namespace ctx                                                         \
  {                                                                     \
    namespace                                                           \
    {                                                                   \
      bool                                                              \
      register_functions()                                              \
      {                                                                 \
        using aut_t = mutable_automaton<Ctx>;                           \
        using taut_t = details::transpose_automaton<aut_t>;             \
                                                                        \
        /* aut_to_exp. */                                               \
        dyn::details::aut_to_exp_register                               \
          (aut_t::sname(), dyn::details::aut_to_exp<aut_t>);            \
        dyn::details::aut_to_exp_in_degree_register                     \
          (aut_t::sname(), dyn::details::aut_to_exp_in_degree<aut_t>);  \
                                                                        \
        /* dotty. */                                                    \
        dotty_register                                                  \
          (aut_t::sname(),                                              \
           static_cast<const dotty_stream_t&>(abstract_dotty<aut_t>));  \
        dotty_register                                                  \
          (aut_t::sname(),                                              \
           static_cast<const dotty_string_t&>(abstract_dotty<aut_t>));  \
        dotty_register                                                  \
          (taut_t::sname(),                                             \
           static_cast<const dotty_stream_t&>(abstract_dotty<taut_t>)); \
        dotty_register                                                  \
          (taut_t::sname(),                                             \
           static_cast<const dotty_string_t&>(abstract_dotty<taut_t>)); \
                                                                        \
        /* edit-automaton. */                                           \
        make_automaton_editor_register                                  \
          (Ctx::sname(), abstract_make_automaton_editor<aut_t>);        \
                                                                        \
        /* dotty. */                                                    \
        lift_register                                                   \
          (aut_t::sname(), abstract_lift<aut_t>);                       \
                                                                        \
        /* make-context. */                                             \
        make_context_register                                           \
          (Ctx::sname(), abstract_make_context<Ctx>);                   \
        make_kratexpset_register                                        \
          (Ctx::sname(), abstract_make_kratexpset<Ctx>);                \
                                                                        \
        /* standard_of. */                                              \
        standard_of_register                                            \
          (Ctx::sname(), abstract_standard_of<aut_t>);                  \
                                                                        \
        /* transpose. */                                                \
        transpose_register                                              \
          (aut_t::sname(), abstract_transpose<aut_t>);                  \
        transpose_exp_register                                          \
          (Ctx::sname(), abstract_transpose_exp<Ctx>);                  \
                                                                        \
        return true;                                                    \
      }                                                                 \
                                                                        \
      static bool registered = register_functions();                    \
    }                                                                   \
  }
# else
#  define VCSN_CTX_INSTANTIATE_2(Ctx)
# endif


# define VCSN_CTX_INSTANTIATE(Ctx)              \
  VCSN_CTX_INSTANTIATE_1(ctx::Ctx)              \
  VCSN_CTX_INSTANTIATE_2(Ctx)

}

#endif // !VCSN_CTX_CHAR_HH
