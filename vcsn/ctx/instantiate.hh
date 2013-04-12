#ifndef VCSN_CTX_INSTANTIATE_HH
# define VCSN_CTX_INSTANTIATE_HH

# include <vcsn/algos/accessible.hh>
# include <vcsn/algos/aut_to_exp.hh>
# include <vcsn/algos/complete.hh>
# include <vcsn/algos/copy.hh>
# include <vcsn/algos/dot.hh>
# include <vcsn/algos/edit-automaton.hh>
# include <vcsn/algos/eps-removal.hh>
# include <vcsn/algos/eval.hh>
# include <vcsn/algos/fsm.hh>
# include <vcsn/algos/info.hh>
# include <vcsn/algos/is_complete.hh>
# include <vcsn/algos/is-deterministic.hh>
# include <vcsn/algos/is-eps-acyclic.hh>
# include <vcsn/algos/lift.hh>
# include <vcsn/algos/make-context.hh>
# include <vcsn/algos/print.hh>
# include <vcsn/algos/product.hh>
# include <vcsn/algos/standard_of.hh>
# include <vcsn/algos/transpose.hh>
# include <vcsn/algos/xml.hh>

# include <vcsn/factory/de_bruijn.hh>
# include <vcsn/factory/ladybird.hh>

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
  void Format<Aut>(const Aut& aut, std::ostream& out);                  \
                                                                        \
  MAYBE_EXTERN template                                                 \
  std::string Format<Aut>(const Aut& aut);

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
  Ctx::ratexp_t                                                         \
  aut_to_exp<mutable_automaton<Ctx>>                                    \
  (const mutable_automaton<Ctx>& aut,                                   \
    const state_chooser_t<mutable_automaton<Ctx>>& next_state);         \
                                                                        \
  /* dot. */                                                            \
  VCSN_CTX_INSTANTIATE_PRINT(dot, mutable_automaton<Ctx>);              \
  VCSN_CTX_INSTANTIATE_PRINT                                            \
  (dot, vcsn::details::transpose_automaton<mutable_automaton<Ctx>>);    \
                                                                        \
  /* fsm. */                                                            \
  VCSN_CTX_INSTANTIATE_PRINT(fsm, mutable_automaton<Ctx>);              \
  VCSN_CTX_INSTANTIATE_PRINT                                            \
  (fsm, vcsn::details::transpose_automaton<mutable_automaton<Ctx>>);    \
                                                                        \
  /* lift. */                                                           \
  MAYBE_EXTERN template                                                 \
  details::lifted_automaton_t<mutable_automaton<Ctx>>                   \
  lift<mutable_automaton<Ctx>>(const mutable_automaton<Ctx>& aut);      \
                                                                        \
  /* print. */                                                          \
  MAYBE_EXTERN template                                                 \
  std::ostream&                                                         \
  print<Ctx>(const Ctx& ctx, const rat::exp_t& e, std::ostream& o);     \
                                                                        \
  /* standard_of. */                                                    \
  MAYBE_EXTERN template                                                 \
  class rat::standard_of_visitor<mutable_automaton<Ctx>, Ctx>;          \
                                                                        \
  /* transpose. */                                                      \
  MAYBE_EXTERN template                                                 \
  class details::transpose_automaton<mutable_automaton<Ctx>>;           \
  MAYBE_EXTERN template                                                 \
  class details::transposer<Ctx>;                                       \
                                                                        \
  /* xml. */                                                            \
  MAYBE_EXTERN template                                                 \
  void xml<Ctx>(const Ctx& ctx, const rat::exp_t exp, std::ostream& out); \
                                                                        \
  MAYBE_EXTERN template                                                 \
  std::string xml<Ctx>(const Ctx& cxt, const rat::exp_t exp);




  /*----------------------------------.
  | Register the abstract functions.  |
  `----------------------------------*/

  namespace ctx
  {
    namespace details
    {
      template <typename Ctx>
      bool
      register_kind_functions(labels_are_letters)
      {
        using aut_t = mutable_automaton<Ctx>;
        using namespace dyn::details;

        complete_register(aut_t::sname(), complete<aut_t>);
        de_bruijn_register(Ctx::sname(), de_bruijn<Ctx>);
        eval_register(aut_t::sname(), eval<aut_t>);

        is_complete_register(aut_t::sname(), is_complete<aut_t>);
        is_deterministic_register(aut_t::sname(), is_deterministic<aut_t>);

        ladybird_register(Ctx::sname(), ladybird<Ctx>);
        product_register(aut_t::sname(), aut_t::sname(), product<aut_t, aut_t>);

        return true;
      }

      template <typename Ctx>
      bool
      register_kind_functions(labels_are_nullable)
      {
        using aut_t = mutable_automaton<Ctx>;
        using namespace dyn::details;

        de_bruijn_register(Ctx::sname(), de_bruijn<Ctx>);

        is_deterministic_register(aut_t::sname(), is_deterministic<aut_t>);

        is_eps_acyclic_register(aut_t::sname(), is_eps_acyclic<aut_t>);

        eps_removal_register(aut_t::sname(), eps_removal<aut_t>);

        ladybird_register(Ctx::sname(), ladybird<Ctx>);

        return true;
      }

      template <typename Ctx>
      bool
      register_kind_functions(labels_are_unit)
      {
        return true;
      }

      template <typename Ctx>
      bool
      register_kind_functions(labels_are_words)
      {
        using aut_t = mutable_automaton<Ctx>;

        using namespace dyn::details;

        // is-eps-acyclic.
        is_eps_acyclic_register(aut_t::sname(), is_eps_acyclic<aut_t>);

        // eps-removal.
        eps_removal_register(aut_t::sname(), eps_removal<aut_t>);

        return true;
      }

      template <typename Ctx>
      bool
      register_functions()
      {
        using aut_t = mutable_automaton<Ctx>;
        using taut_t = vcsn::details::transpose_automaton<aut_t>;

        using namespace dyn::details;

        // accessible.
        accessible_register(aut_t::sname(), accessible<aut_t>);

        // aut_to_exp.
        aut_to_exp_register(aut_t::sname(), aut_to_exp<aut_t>);
        aut_to_exp_in_degree_register(aut_t::sname(),
                                      aut_to_exp_in_degree<aut_t>);

        // copy.
        copy_register(aut_t::sname(), copy<aut_t>);

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

        // fsm.
        fsm_stream_register(aut_t::sname(),
                            static_cast<fsm_stream_t&>(fsm<aut_t>));
        fsm_string_register(aut_t::sname(),
                            static_cast<fsm_string_t&>(fsm<aut_t>));
        fsm_stream_register(taut_t::sname(),
                            static_cast<fsm_stream_t&>(fsm<taut_t>));
        fsm_string_register(taut_t::sname(),
                            static_cast<fsm_string_t&>(fsm<taut_t>));

        // info.
        info_stream_register(aut_t::sname(),
                             static_cast<info_stream_t&>(info<aut_t>));
        info_string_register(aut_t::sname(),
                             static_cast<info_string_t&>(info<aut_t>));
        info_stream_register(taut_t::sname(),
                             static_cast<info_stream_t&>(info<taut_t>));
        info_string_register(taut_t::sname(),
                             static_cast<info_string_t&>(info<taut_t>));

        // is-deterministic.
        is_deterministic_register(aut_t::sname(), is_deterministic<aut_t>);

        // is-proper.
        is_proper_register(aut_t::sname(), is_proper<aut_t>);

        // lift.
        lift_automaton_register(aut_t::sname(), lift<aut_t>);
        lift_exp_register(Ctx::sname(), lift<Ctx>);

        // make-context.
        make_context_register(Ctx::sname(), make_context<Ctx>);
        make_ratexpset_register(Ctx::sname(), make_ratexpset<Ctx>);

        // print
        print_exp_register(Ctx::sname(), print<Ctx>);

        // standard_of.
        standard_of_register(Ctx::sname(), standard_of<aut_t>);

        // transpose.
        transpose_register(aut_t::sname(), transpose<aut_t>);
        transpose_exp_register(Ctx::sname(), abstract_transpose_exp<Ctx>);

        // xml.
        xml_stream_register(Ctx::sname(),
                            static_cast<xml_stream_t&>(xml<Ctx>));
        xml_string_register(Ctx::sname(),
                            static_cast<xml_string_t&>(xml<Ctx>));

        register_kind_functions<Ctx>(typename Ctx::kind_t());
        return true;
      }
    }
  }

# if VCSN_INSTANTIATION
#  define VCSN_CTX_INSTANTIATE_2(Ctx)                           \
  namespace ctx                                                 \
  {                                                             \
    namespace details                                           \
    {                                                           \
      static bool registered = register_functions<Ctx>();       \
    }                                                           \
  }
# else
#  define VCSN_CTX_INSTANTIATE_2(Ctx)
# endif

# define VCSN_CTX_INSTANTIATE(Ctx)              \
  VCSN_CTX_INSTANTIATE_1(ctx::Ctx)              \
  VCSN_CTX_INSTANTIATE_2(Ctx)

# ifndef MAYBE_EXTERN
#  define MAYBE_EXTERN extern
# endif

}

#endif // !VCSN_CTX_INSTANTIATE_HH
