#ifndef VCSN_CTX_INSTANTIATE_HH
# define VCSN_CTX_INSTANTIATE_HH

# include <vcsn/algos/accessible.hh>
# include <vcsn/algos/aut_to_exp.hh>
# include <vcsn/algos/complete.hh>
# include <vcsn/algos/copy.hh>
# include <vcsn/algos/dot.hh>
# include <vcsn/algos/edit-automaton.hh>
# include <vcsn/algos/efsm.hh>
# include <vcsn/algos/enumerate.hh>
# include <vcsn/algos/eval.hh>
# include <vcsn/algos/info.hh>
# include <vcsn/algos/is-deterministic.hh>
# include <vcsn/algos/is-eps-acyclic.hh>
# include <vcsn/algos/is-normalized.hh>
# include <vcsn/algos/is-proper.hh>
# include <vcsn/algos/is_complete.hh>
# include <vcsn/algos/lift.hh>
# include <vcsn/algos/make-context.hh>
# include <vcsn/algos/print.hh>
# include <vcsn/algos/product.hh>
# include <vcsn/algos/proper.hh>
# include <vcsn/algos/standard.hh>
# include <vcsn/algos/thompson.hh>
# include <vcsn/algos/tikz.hh>
# include <vcsn/algos/transpose.hh>

# include <vcsn/factory/de_bruijn.hh>
# include <vcsn/factory/divkbaseb.hh>
# include <vcsn/factory/ladybird.hh>
# include <vcsn/factory/u.hh>

/* The purpose of this file is manyfold:

   - *prevent* the instantiation of the algorithms that we will
      provided by the context library.  This is what the INSTANTIATE
      macros do.  In this case MAYBE_EXTERN is "extern".

   - instantiate them when building the context libraries.  In this
     context, MAYBE_EXTERN is "".

   - register the dyn functions.  This is what the register function
     templates do.

*/

namespace vcsn
{
# define VCSN_CTX_INSTANTIATE_PRINT(Format, Aut)                        \
  MAYBE_EXTERN template                                                 \
  std::ostream& Format<Aut>(const Aut& aut, std::ostream& out);         \
                                                                        \
  MAYBE_EXTERN template                                                 \
  std::string Format<Aut>(const Aut& aut);

  /*-------------------------------------------------------.
  | Instantiate the function that work for every context.  |
  `-------------------------------------------------------*/

# define VCSN_CTX_INSTANTIATE_1(Ctx)                                    \
  MAYBE_EXTERN template                                                 \
  class mutable_automaton<Ctx>;                                         \
                                                                        \
  /* aut_to_exp. */                                                     \
  MAYBE_EXTERN template                                                 \
  Ctx::ratexp_t                                                         \
  aut_to_exp<mutable_automaton<Ctx>>                                    \
  (const mutable_automaton<Ctx>& aut,                                   \
    const state_chooser_t<mutable_automaton<Ctx>>& next_state);         \
                                                                        \
  /* dot. */                                                            \
  VCSN_CTX_INSTANTIATE_PRINT(dot, mutable_automaton<Ctx>);              \
  VCSN_CTX_INSTANTIATE_PRINT                                            \
  (dot, vcsn::detail::transpose_automaton<mutable_automaton<Ctx>>);     \
                                                                        \
  /* efsm. */                                                           \
  MAYBE_EXTERN template                                                 \
  std::ostream& efsm<mutable_automaton<Ctx>>                            \
  (const mutable_automaton<Ctx>& aut, std::ostream& out);               \
  MAYBE_EXTERN template                                                 \
  std::ostream&                                                         \
  efsm<vcsn::detail::transpose_automaton<mutable_automaton<Ctx>>>       \
  (const vcsn::detail::transpose_automaton<mutable_automaton<Ctx>>& aut,\
   std::ostream& out);                                                  \
                                                                        \
  /* lift. */                                                           \
  MAYBE_EXTERN template                                                 \
  detail::lifted_automaton_t<mutable_automaton<Ctx>>                    \
  lift<mutable_automaton<Ctx>>(const mutable_automaton<Ctx>& aut);      \
                                                                        \
  /* print. */                                                          \
  MAYBE_EXTERN template                                                 \
  std::ostream&                                                         \
  print<Ctx>(const Ctx& ctx, const rat::exp_t& e, std::ostream& o);     \
  MAYBE_EXTERN template                                                 \
  std::ostream&                                                         \
  print<typename Ctx::weightset_t>                                      \
  (const typename Ctx::weightset_t& ws,                                 \
   const typename Ctx::weight_t& e,                                     \
   std::ostream& o);                                                    \
                                                                        \
  /* standard. */                                                       \
  MAYBE_EXTERN template                                                 \
  class rat::standard_visitor<mutable_automaton<Ctx>, Ctx>;             \
                                                                        \
  /* tikz. */                                                           \
  MAYBE_EXTERN template                                                 \
  std::ostream&                                                         \
  tikz<mutable_automaton<Ctx>>(const mutable_automaton<Ctx>& aut,       \
                               std::ostream& out);                      \
                                                                        \
  /* transpose. */                                                      \
  MAYBE_EXTERN template                                                 \
  class detail::transpose_automaton<mutable_automaton<Ctx>>;            \
  MAYBE_EXTERN template                                                 \
  class detail::transposer<Ctx>


  /*----------------------------------.
  | Register the abstract functions.  |
  `----------------------------------*/

#define REGISTER(Type, Algo)                    \
  Algo ## _register(Type::sname(), Algo<Type>)

  namespace ctx
  {
    namespace detail
    {
      template <typename Ctx>
      bool
      register_kind_functions(labels_are_letters)
      {
        using aut_t = mutable_automaton<Ctx>;
        using namespace dyn::detail;

        REGISTER(aut_t, complete);
        REGISTER(Ctx, de_bruijn);
        REGISTER(Ctx, divkbaseb);
        REGISTER(aut_t, enumerate);
        REGISTER(aut_t, eval);

        REGISTER(aut_t, is_complete);
        REGISTER(aut_t, is_deterministic);

        REGISTER(Ctx, ladybird);
        REGISTER(aut_t, power);
        product_register(aut_t::sname(), aut_t::sname(), product<aut_t, aut_t>);
        REGISTER(aut_t, shortest);
        REGISTER(Ctx, u);

        return true;
      }

      template <typename Ctx>
      bool
      register_kind_functions(labels_are_nullable)
      {
        using aut_t = mutable_automaton<Ctx>;
        using namespace dyn::detail;

        REGISTER(Ctx, de_bruijn);
        REGISTER(Ctx, divkbaseb);

        REGISTER(aut_t, proper);

        REGISTER(Ctx, ladybird);

        thompson_register(Ctx::sname(), thompson<aut_t>);
        REGISTER(Ctx, u);

        return true;
      }

      template <typename Ctx>
      bool
      register_kind_functions(labels_are_one)
      {
        return true;
      }

      template <typename Ctx>
      bool
      register_kind_functions(labels_are_words)
      {
        using aut_t = mutable_automaton<Ctx>;

        using namespace dyn::detail;

        // proper.
        REGISTER(aut_t, proper);

        // thompson.
        thompson_register(Ctx::sname(), thompson<aut_t>);

        return true;
      }

      template <typename Ctx>
      bool
      register_functions()
      {
        using aut_t = mutable_automaton<Ctx>;
        using taut_t = vcsn::detail::transpose_automaton<aut_t>;

        using namespace dyn::detail;

        // accessible.
        REGISTER(aut_t, accessible);

        // aut_to_exp.
        REGISTER(aut_t, aut_to_exp);

        // coaccessible.
        REGISTER(aut_t, coaccessible);

        // copy.
        REGISTER(aut_t, copy);

        // dot.
        dot_stream_register(aut_t::sname(),
                            static_cast<dot_stream_t&>(dot<aut_t>));
        dot_string_register(aut_t::sname(),
                            static_cast<dot_string_t&>(dot<aut_t>));
        dot_stream_register(taut_t::sname(),
                            static_cast<dot_stream_t&>(dot<taut_t>));
        dot_string_register(taut_t::sname(),
                            static_cast<dot_string_t&>(dot<taut_t>));

        // edit-automaton.
        make_automaton_editor_register(Ctx::sname(),
                                       make_automaton_editor<aut_t>);

        // efsm.
        REGISTER(aut_t, efsm);
        REGISTER(taut_t, efsm);

        // info.
        REGISTER(aut_t, info);
        REGISTER(taut_t, info);
        REGISTER(Ctx, info_exp);

        REGISTER(aut_t, is_empty);

        // is-eps-acyclic.
        REGISTER(aut_t, is_eps_acyclic);

        REGISTER(aut_t, is_normalized);

        // is-proper.
        REGISTER(aut_t, is_proper);

        REGISTER(aut_t, is_trim);
        REGISTER(aut_t, is_useless);

        // lift.
        lift_automaton_register(aut_t::sname(), lift<aut_t>);
        lift_exp_register(Ctx::sname(), lift<Ctx>);

        // make-context.
        REGISTER(Ctx, make_context);
        REGISTER(Ctx, make_ratexpset);

        // print
        print_exp_register(Ctx::sname(), print<Ctx>);
        print_weight_register(Ctx::weightset_t::sname(),
                              print<typename Ctx::weightset_t>);

        // standard.
        standard_register(Ctx::sname(), standard<aut_t>);

        // tikz.
        REGISTER(aut_t, tikz);
        REGISTER(taut_t, tikz);

        // transpose.
        REGISTER(aut_t, transpose);
        transpose_exp_register(Ctx::sname(), abstract_transpose_exp<Ctx>);

        // trim.
        REGISTER(aut_t, trim);

        register_kind_functions<Ctx>(typename Ctx::kind_t());
        return true;
      }
    }
  }

# undef REGISTER

# if VCSN_INSTANTIATION
#  define VCSN_CTX_INSTANTIATE_2(Ctx)                           \
  namespace ctx                                                 \
  {                                                             \
    namespace detail                                            \
    {                                                           \
      static bool registered = register_functions<Ctx>();       \
    }                                                           \
  }
# else
#  define VCSN_CTX_INSTANTIATE_2(Ctx)
# endif

# define VCSN_CTX_INSTANTIATE(Ctx)              \
  VCSN_CTX_INSTANTIATE_1(ctx::Ctx);             \
  VCSN_CTX_INSTANTIATE_2(Ctx)

# ifndef MAYBE_EXTERN
#  define MAYBE_EXTERN extern
# endif

}

#endif // !VCSN_CTX_INSTANTIATE_HH
