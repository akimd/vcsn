#ifndef VCSN_CTX_INSTANTIATE_HH
# define VCSN_CTX_INSTANTIATE_HH

# include <vcsn/algos/aut_to_exp.hh>
# include <vcsn/algos/determinize.hh>
# include <vcsn/algos/dotty.hh>
# include <vcsn/algos/edit-automaton.hh>
# include <vcsn/algos/eval.hh>
# include <vcsn/algos/lift.hh>
# include <vcsn/algos/make-context.hh>
# include <vcsn/algos/print.hh>
# include <vcsn/algos/standard_of.hh>
# include <vcsn/algos/transpose.hh>
# include <vcsn/algos/xml.hh>

# include <vcsn/factory/de_bruijn.hh>

namespace vcsn
{
# define VCSN_CTX_INSTANTIATE_DOTTY(Aut)                                \
  MAYBE_EXTERN template                                                 \
  void dotty<Aut>(const Aut& aut, std::ostream& out);                   \
                                                                        \
  MAYBE_EXTERN template                                                 \
  std::string dotty<Aut>(const Aut& aut);                               \
                                                                        \
  namespace dyn                                                         \
  {                                                                     \
    namespace details                                                   \
    {                                                                   \
      MAYBE_EXTERN template                                             \
        void dotty<Aut>(const dyn::automaton& aut, std::ostream& out);  \
                                                                        \
      MAYBE_EXTERN template                                             \
        std::string dotty<Aut>(const dyn::automaton& aut);              \
    }                                                                   \
  }

# define VCSN_CTX_INSTANTIATE_XML(Ctx)                                  \
  MAYBE_EXTERN template                                                 \
  void xml<Ctx>(std::ostream& out,                                      \
                const Ctx& ctx,                                         \
                const rat::exp& exp);                                   \
                                                                        \
  MAYBE_EXTERN template                                                 \
  std::string xml<Ctx>(const Ctx& cxt,                                  \
                       const rat::exp& exp);                            \
                                                                        \
  namespace dyn                                                         \
  {                                                                     \
    namespace details                                                   \
    {                                                                   \
      MAYBE_EXTERN template                                             \
      void xml<Ctx>(std::ostream& out, const dyn::ratexp& exp);         \
                                                                        \
      MAYBE_EXTERN template                                             \
        std::string xml<Ctx>(const dyn::ratexp&);                       \
    }                                                                   \
  }

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
  /* dotty. */                                                          \
  VCSN_CTX_INSTANTIATE_DOTTY(mutable_automaton<Ctx>);                   \
  VCSN_CTX_INSTANTIATE_DOTTY                                            \
  (vcsn::details::transpose_automaton<mutable_automaton<Ctx>>);         \
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
  VCSN_CTX_INSTANTIATE_XML(Ctx);



  /*----------------------------------.
  | Register the abstract functions.  |
  `----------------------------------*/

  namespace ctx
  {
    namespace details
    {
      template <typename Ctx>
      bool
      register_kind_functions(labels_are_unit)
      {
        return true;
      }

      template <typename Ctx>
      bool
      register_kind_functions(labels_are_letters)
      {
        using aut_t = mutable_automaton<Ctx>;
        using namespace dyn::details;

        // de_bruijn.
        de_bruijn_register(Ctx::sname(), de_bruijn<Ctx>);
        // eval
        eval_register(aut_t::sname(), eval<aut_t>);
        return true;
      }

      template <typename Ctx>
      bool
      register_kind_functions(labels_are_words)
      {
        return true;
      }

      template <typename Ctx>
      bool
      register_functions()
      {
        using aut_t = mutable_automaton<Ctx>;
        using taut_t = vcsn::details::transpose_automaton<aut_t>;

        using namespace dyn::details;

        // aut_to_exp.
        aut_to_exp_register(aut_t::sname(), aut_to_exp<aut_t>);
        aut_to_exp_in_degree_register(aut_t::sname(),
                                      aut_to_exp_in_degree<aut_t>);

        // dotty.
        dotty_register(aut_t::sname(),
                       static_cast<const dotty_stream_t&>(dotty<aut_t>));
        dotty_register(aut_t::sname(),
                       static_cast<const dotty_string_t&>(dotty<aut_t>));
        dotty_register(taut_t::sname(),
                       static_cast<const dotty_stream_t&>(dotty<taut_t>));
        dotty_register(taut_t::sname(),
                       static_cast<const dotty_string_t&>(dotty<taut_t>));

        // edit-automaton.
        make_automaton_editor_register(Ctx::sname(),
                                       abstract_make_automaton_editor<aut_t>);

        // lift.
        lift_register(aut_t::sname(), lift<aut_t>);

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
        xml_register(Ctx::sname(),
                     static_cast<const xml_stream_t&>(xml<Ctx>));
        xml_register(Ctx::sname(),
                     static_cast<const xml_string_t&>(xml<Ctx>));

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
