#pragma once

#include <vcsn/algos/accessible.hh>
#include <vcsn/algos/add.hh>
#include <vcsn/algos/are-equivalent.hh> // difference
#include <vcsn/algos/are-isomorphic.hh>
#include <vcsn/algos/cerny.hh>
#include <vcsn/algos/compare-automaton.hh>
#include <vcsn/algos/compare.hh>
#include <vcsn/algos/complete.hh>
#include <vcsn/algos/conjunction-expression.hh>
#include <vcsn/algos/conjunction.hh>
#include <vcsn/algos/constant-term.hh>
#include <vcsn/algos/constant.hh>
#include <vcsn/algos/copy.hh>
#include <vcsn/algos/de-bruijn.hh>
#include <vcsn/algos/derivation.hh>
#include <vcsn/algos/derived-term.hh>
#include <vcsn/algos/determinize.hh>
#include <vcsn/algos/divide.hh>
#include <vcsn/algos/divkbaseb.hh>
#include <vcsn/algos/double-ring.hh>
#include <vcsn/algos/edit-automaton.hh>
#include <vcsn/algos/evaluate.hh>
#include <vcsn/algos/expand.hh>
#include <vcsn/algos/identities-of.hh>
#include <vcsn/algos/inductive.hh>
#include <vcsn/algos/infiltrate-expression.hh>
#include <vcsn/algos/info.hh>
#include <vcsn/algos/is-acyclic.hh>
#include <vcsn/algos/is-ambiguous.hh>
#include <vcsn/algos/is-complete.hh>
#include <vcsn/algos/is-deterministic.hh>
#include <vcsn/algos/is-proper.hh>
#include <vcsn/algos/is-valid-expression.hh>
#include <vcsn/algos/is-valid.hh>
#include <vcsn/algos/ladybird.hh>
#include <vcsn/algos/make-context.hh>
#include <vcsn/algos/minimize.hh>
#include <vcsn/algos/multiply.hh>
#include <vcsn/algos/normalize.hh>
#include <vcsn/algos/num-tapes.hh>
#include <vcsn/algos/print.hh>
#include <vcsn/algos/proper.hh>
#include <vcsn/algos/push-weights.hh>
#include <vcsn/algos/read.hh>
#include <vcsn/algos/shortest.hh>
#include <vcsn/algos/shuffle-expression.hh>
#include <vcsn/algos/sort.hh>
#include <vcsn/algos/split.hh>
#include <vcsn/algos/standard.hh>
#include <vcsn/algos/star-height.hh>
#include <vcsn/algos/star.hh>
#include <vcsn/algos/synchronizing-word.hh>
#include <vcsn/algos/thompson.hh>
#include <vcsn/algos/to-expansion.hh>
#include <vcsn/algos/to-expression.hh>
#include <vcsn/algos/transpose.hh>
#include <vcsn/algos/u.hh>
#include <vcsn/algos/weight.hh>

#include <vcsn/core/rat/identities.hh>

#include <vcsn/dyn/registries.hh>

#include <vcsn/weightset/b.hh>
#include <vcsn/weightset/polynomialset.hh>

#include <vcsn/misc/attributes.hh>
#include <vcsn/dyn/name.hh>

/* The purpose of this file is manyfold:

   - *prevent* the instantiation of the algorithms that will be
      provided by the context library.  This is what the INSTANTIATE
      macros do.  In this case MAYBE_EXTERN is "extern".

   - instantiate them when building the context libraries.  In this
     context, MAYBE_EXTERN is "".

   - register the dyn functions.  This is what the register function
     templates do.

*/

namespace vcsn
{
#define VCSN_CTX_INSTANTIATE_PRINT(Format, Aut)                         \
  MAYBE_EXTERN template                                                 \
  LIBVCSN_API                                                           \
  std::ostream& Format<Aut>(const Aut& aut, std::ostream& out)

  /*-------------------------------------------------------.
  | Instantiate the function that work for every context.  |
  `-------------------------------------------------------*/

#define VCSN_CTX_INSTANTIATE_1(Ctx)                                     \
  MAYBE_EXTERN template                                                 \
  class LIBVCSN_API mutable_automaton<Ctx>;                             \
                                                                        \
  /* to_expression. */                                                  \
  MAYBE_EXTERN template                                                 \
  Ctx::expression_t                                                     \
  to_expression<mutable_automaton<Ctx>>                                 \
  (const mutable_automaton<Ctx>& aut,                                   \
    const state_chooser_t<mutable_automaton<Ctx>>& next_state);         \
                                                                        \
  /* dot. */                                                            \
  VCSN_CTX_INSTANTIATE_PRINT(dot, mutable_automaton<Ctx>);              \
                                                                        \
  /* efsm. */                                                           \
  VCSN_CTX_INSTANTIATE_PRINT(efsm, mutable_automaton<Ctx>);             \
                                                                        \
  /* print. */                                                          \
  MAYBE_EXTERN template                                                 \
  std::ostream&                                                         \
  print<expressionset<Ctx>>(const expressionset<Ctx>& rs,               \
                            const Ctx::expression_t& e, std::ostream& o, \
                            format fmt);                                \
                                                                        \
  /* standard. */                                                       \
  MAYBE_EXTERN template                                                 \
  class rat::standard_visitor<mutable_automaton<Ctx>, Ctx>;             \
                                                                        \
  /* tikz. */                                                           \
  VCSN_CTX_INSTANTIATE_PRINT(tikz, mutable_automaton<Ctx>);             \
                                                                        \
  /* transpose. */                                                      \
  MAYBE_EXTERN template                                                 \
  class LIBVCSN_API detail::transpose_automaton<mutable_automaton<Ctx>>; \
  MAYBE_EXTERN template                                                 \
  class LIBVCSN_API detail::transposer<expressionset<Ctx>>


  /*----------------------------------.
  | Register the abstract functions.  |
  `----------------------------------*/

#define REGISTER(Algo, ...)                                     \
  dyn::detail::Algo ## _register(ssignature<__VA_ARGS__>(),     \
                                 dyn::detail::Algo<__VA_ARGS__>)

  namespace ctx
  {
    namespace detail
    {

      /// Instantiate the functions for automata of type \a Aut.
      template <Automaton Aut>
      bool
      register_automaton_functions()
      {
        using aut_t = Aut;
        using ctx_t = context_t_of<Aut>;
        // Wordset.
        using wls_t = vcsn::detail::law_t<labelset_t_of<ctx_t>>;

        REGISTER(compare, aut_t, aut_t);
        REGISTER(context_of, aut_t);
        REGISTER(copy, aut_t);
        REGISTER(determinize, aut_t, const std::string);
        REGISTER(evaluate, aut_t, wls_t);
        REGISTER(info, aut_t, std::ostream, unsigned, bool);
        REGISTER(is_free, aut_t);
        REGISTER(minimize, aut_t, const std::string);
        REGISTER(print, aut_t, std::ostream, const std::string);
        REGISTER(proper, aut_t, direction, bool, const std::string);
        REGISTER(to_expression, aut_t, rat::identities, const std::string);

#if 2 <= VCSN_INSTANTIATION
        using ws_t = weightset_t_of<ctx_t>;
        REGISTER(accessible, aut_t);
        REGISTER(add, aut_t, aut_t, const std::string);
        REGISTER(are_isomorphic, aut_t, aut_t);
        REGISTER(coaccessible, aut_t);
        REGISTER(complete, aut_t);
        REGISTER(conjunction, std::tuple<aut_t, aut_t>, bool);
        REGISTER(conjunction_repeated, aut_t, unsigned);
        REGISTER(infiltrate, std::tuple<aut_t, aut_t>);
        REGISTER(is_ambiguous, aut_t);
        REGISTER(is_complete, aut_t);
        REGISTER(is_deterministic, aut_t);
        REGISTER(is_empty, aut_t);
        REGISTER(is_eps_acyclic, aut_t);
        REGISTER(is_normalized, aut_t);
        REGISTER(is_out_sorted, aut_t);
        REGISTER(is_proper, aut_t);
        REGISTER(is_standard, aut_t);
        REGISTER(is_synchronized_by, aut_t, wls_t);
        REGISTER(is_synchronizing, aut_t);
        REGISTER(is_trim, aut_t);
        REGISTER(is_useless, aut_t);
        REGISTER(is_valid, aut_t);
        REGISTER(lweight, ws_t, aut_t, const std::string);
        REGISTER(multiply, aut_t, aut_t, const std::string);
        REGISTER(multiply_repeated, aut_t, int, int, const std::string);
        REGISTER(pair, aut_t, bool);
        REGISTER(push_weights, aut_t);
        REGISTER(rweight, aut_t, ws_t, const std::string);
        REGISTER(shortest, aut_t, std::optional<unsigned>, std::optional<unsigned>);
        REGISTER(shuffle, std::tuple<aut_t, aut_t>);
        REGISTER(sort, aut_t);
        REGISTER(standard, aut_t);
        REGISTER(star, aut_t, const std::string);
        REGISTER(synchronizing_word, aut_t, const std::string);
        REGISTER(transpose, aut_t);
        REGISTER(trim, aut_t);
#endif

        return true;
      }

      /// Instantiate the core functions for expressions of type \a ExpSet.
      template <typename Ctx>
      bool
      register_context_functions()
      {
        using ctx_t = Ctx;

        REGISTER(make_automaton_editor, ctx_t);
        REGISTER(make_context, ctx_t);
        REGISTER(make_word_context, ctx_t);

#if 2 <= VCSN_INSTANTIATION
        REGISTER(cerny, ctx_t, unsigned);
        REGISTER(de_bruijn, ctx_t, unsigned);
        REGISTER(divkbaseb, ctx_t, unsigned, unsigned);
        REGISTER(double_ring, ctx_t, unsigned, const std::vector<unsigned>);
        REGISTER(ladybird, ctx_t, unsigned);
        REGISTER(u, ctx_t, unsigned);
#endif
        return true;
      }

        /// Instantiate the core functions for expressions of type \a ExpSet.
      template <typename ExpressionSet>
      bool
      register_expression_functions()
      {
        using rs_t = ExpressionSet;
        using ctx_t = context_t_of<rs_t>;
        using ws_t = weightset_t_of<ctx_t>;

        REGISTER(add_expression, rs_t, rs_t);
        REGISTER(compare_expression, rs_t, rs_t);
        REGISTER(concatenate_expression, rs_t, rs_t);
        REGISTER(conjunction_expression, rs_t, rs_t);
        REGISTER(context_of_expression, rs_t);
        REGISTER(copy_expression, rs_t, rs_t, rat::identities);
        REGISTER(expression_one, ctx_t, rat::identities);
        REGISTER(expression_zero, ctx_t, rat::identities);
        REGISTER(identities_of, rs_t);
        REGISTER(infiltrate_expression, rs_t, rs_t);
        REGISTER(info_expression, rs_t, std::ostream);
        REGISTER(ldivide_expression, rs_t, rs_t);
        REGISTER(lweight_expression, ws_t, rs_t);
        REGISTER(multiply_expression, rs_t, rs_t);
        REGISTER(multiply_expression_repeated, rs_t, int, int);
        REGISTER(print_expression, rs_t, std::ostream, const std::string);
        REGISTER(rdivide_expression, rs_t, rs_t);
        REGISTER(rweight_expression, rs_t, ws_t);
        REGISTER(shuffle_expression, rs_t, rs_t);
        REGISTER(standard_expression, rs_t);
        REGISTER(thompson, rs_t);

#if 2 <= VCSN_INSTANTIATION
        REGISTER(is_valid_expression, rs_t);
        REGISTER(complement_expression, rs_t);
        REGISTER(constant_term, rs_t);
        REGISTER(derived_term, rs_t, const std::string);
        REGISTER(expand, rs_t);
        REGISTER(inductive, rs_t, const std::string);
        REGISTER(split, rs_t);
        REGISTER(star_height, rs_t);
        REGISTER(to_expansion, rs_t);
        REGISTER(transpose_expression, rs_t);
        REGISTER(transposition_expression, rs_t);
#endif
        return true;
      }

      template <typename Ctx>
      bool
      register_kind_functions(labels_are_letters)
      {
        return true;
      }

      template <typename Ctx>
      bool
      register_functions_is_letterized(std::true_type)
      {
#if 2 <= VCSN_INSTANTIATION
        using ctx_t = Ctx;
        using rs_t = expressionset<ctx_t>;

        // Wordset.
        using wls_t = vcsn::detail::law_t<labelset_t_of<Ctx>>;

        // Word polynomialset.
        using wps_t = vcsn::detail::word_polynomialset_t<ctx_t>;

        // Same labelset, but over Booleans.
        using b_ctx_t = context<labelset_t_of<Ctx>, b>;
        using b_rs_t = expressionset<b_ctx_t>;

        REGISTER(derivation, rs_t, wls_t, bool);
        REGISTER(difference_expression, rs_t, b_rs_t);
        REGISTER(list_polynomial, wps_t, std::ostream);
        REGISTER(multiply_polynomial, wps_t, wps_t);
        REGISTER(print_polynomial, wps_t, std::ostream, const std::string);
#endif

        return true;
      }

      template <typename Ctx>
      bool
      register_functions_is_letterized(std::false_type)
      {
        return true;
      }

      template <typename Ctx>
      bool
      register_kind_functions(labels_are_one)
      {
#if 2 <= VCSN_INSTANTIATION
        using ctx_t = Ctx;
        using aut_t = mutable_automaton<ctx_t>;

        REGISTER(eliminate_state, aut_t, int);
#endif

        return true;
      }

      template <typename Ctx>
      ATTRIBUTE_CONST
      bool
      register_kind_functions(labels_are_expressions)
      {
        return true;
      }

      template <typename Ctx>
      ATTRIBUTE_CONST
      bool
      register_kind_functions(labels_are_tuples)
      {
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
        using ctx_t = Ctx;
        using aut_t = mutable_automaton<ctx_t>;
        using rs_t = expressionset<ctx_t>;
        using ls_t = labelset_t_of<ctx_t>;
        using ws_t = weightset_t_of<ctx_t>;

        // label polynomialset.
        using lps_t = polynomialset<ctx_t>;

        register_automaton_functions<aut_t>();
        register_context_functions<ctx_t>();
        register_expression_functions<rs_t>();

        REGISTER(num_tapes, ctx_t);
        REGISTER(print_context, ctx_t, std::ostream, const std::string);
        REGISTER(print_polynomial, lps_t, std::ostream, const std::string);
        REGISTER(print_weight, ws_t, std::ostream, const std::string);
        REGISTER(read_label, ctx_t, std::istream, bool);
        REGISTER(read_polynomial, ctx_t, std::istream);
        REGISTER(read_weight, ctx_t, std::istream);
        REGISTER(to_expression_class, ctx_t, rat::identities, const letter_class_t, bool);
        REGISTER(to_expression_label, ctx_t, rat::identities, ls_t);

#if 2 <= VCSN_INSTANTIATION
        // expression polynomialset.
        using rps_t = rat::expression_polynomialset_t<rs_t>;
        // expansionset.
        using xs_t = rat::expansionset<rs_t>;

        REGISTER(add_weight, ws_t, ws_t);
        REGISTER(list_polynomial, rps_t, std::ostream);
        REGISTER(multiply_weight, ws_t, ws_t);
        REGISTER(print_expansion, xs_t, std::ostream, const std::string);
        REGISTER(print_label, ls_t, std::ostream, const std::string);
        REGISTER(print_polynomial, rps_t, std::ostream, const std::string);
#endif

        using is_letterized_t = bool_constant<ctx_t::labelset_t::is_letterized()>;
        register_functions_is_letterized<ctx_t>(is_letterized_t());

        return register_kind_functions<ctx_t>(typename ctx_t::kind_t());
      }
    }
  }

#undef REGISTER

#if VCSN_INSTANTIATION
# define VCSN_CTX_INSTANTIATE_2(Ctx)            \
  namespace ctx                                 \
  {                                             \
    namespace detail                            \
    {                                           \
      static bool registered ATTRIBUTE_USED     \
        = register_functions<Ctx>();            \
    }                                           \
  }
#else
# define VCSN_CTX_INSTANTIATE_2(Ctx)
#endif

#define VCSN_CTX_INSTANTIATE(Ctx)               \
  /* VCSN_CTX_INSTANTIATE_1(Ctx);*/             \
  VCSN_CTX_INSTANTIATE_2(Ctx)

#ifndef MAYBE_EXTERN
# define MAYBE_EXTERN extern
#endif
}
