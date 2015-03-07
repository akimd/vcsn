#pragma once

#include <vcsn/algos/accessible.hh>
#include <vcsn/algos/are-equivalent.hh> // difference
#include <vcsn/algos/are-isomorphic.hh>
#include <vcsn/algos/cerny.hh>
#include <vcsn/algos/complete.hh>
#include <vcsn/algos/concatenate.hh>
#include <vcsn/algos/constant-term.hh>
#include <vcsn/algos/copy.hh>
#include <vcsn/algos/de-bruijn.hh>
#include <vcsn/algos/derivation.hh>
#include <vcsn/algos/derived-term.hh>
#include <vcsn/algos/divkbaseb.hh>
#include <vcsn/algos/dot.hh>
#include <vcsn/algos/double-ring.hh>
#include <vcsn/algos/edit-automaton.hh>
#include <vcsn/algos/efsm.hh>
#include <vcsn/algos/enumerate.hh>
#include <vcsn/algos/eval.hh>
#include <vcsn/algos/expand.hh>
#include <vcsn/algos/identities.hh>
#include <vcsn/algos/info.hh>
#include <vcsn/algos/is-ambiguous.hh>
#include <vcsn/algos/is-complete.hh>
#include <vcsn/algos/is-deterministic.hh>
#include <vcsn/algos/is-eps-acyclic.hh>
#include <vcsn/algos/normalize.hh>
#include <vcsn/algos/is-proper.hh>
#include <vcsn/algos/is-valid-expression.hh>
#include <vcsn/algos/is-valid.hh>
#include <vcsn/algos/ladybird.hh>
#include <vcsn/algos/left-mult.hh>
#include <vcsn/algos/lift.hh>
#include <vcsn/algos/make-context.hh>
#include <vcsn/algos/minimize.hh>
#include <vcsn/algos/print.hh>
#include <vcsn/algos/product.hh>
#include <vcsn/algos/proper.hh>
#include <vcsn/algos/push-weights.hh>
#include <vcsn/algos/random.hh>
#include <vcsn/algos/read.hh>
#include <vcsn/algos/sort.hh>
#include <vcsn/algos/split.hh>
#include <vcsn/algos/standard.hh>
#include <vcsn/algos/star-height.hh>
#include <vcsn/algos/star.hh>
#include <vcsn/algos/sum.hh>
#include <vcsn/algos/synchronizing-word.hh>
#include <vcsn/algos/thompson.hh>
#include <vcsn/algos/tikz.hh>
#include <vcsn/algos/to-expansion.hh>
#include <vcsn/algos/to-expression.hh>
#include <vcsn/algos/transpose.hh>
#include <vcsn/algos/u.hh>
#include <vcsn/algos/union.hh>

#include <vcsn/core/rat/identities.hh>

#include <vcsn/dyn/registers.hh>

#include <vcsn/weightset/b.hh>
#include <vcsn/weightset/polynomialset.hh>

#include <vcsn/misc/name.hh>

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
  /* lift. */                                                           \
  MAYBE_EXTERN template                                                 \
  detail::lifted_automaton_t<mutable_automaton<Ctx>>                    \
  lift<mutable_automaton<Ctx>>(const mutable_automaton<Ctx>& aut);      \
                                                                        \
  /* print. */                                                          \
  MAYBE_EXTERN template                                                 \
  std::ostream&                                                         \
  print<expressionset<Ctx>>(const expressionset<Ctx>& rs,               \
                            const Ctx::expression_t& e, std::ostream& o, \
                            const std::string& format);                 \
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
  Algo ## _register(ssignature<__VA_ARGS__>(), Algo<__VA_ARGS__>)

  namespace ctx
  {
    namespace detail
    {

      /// Instantiate the core functions for automata of type \a Aut.
      template <typename Aut>
      bool
      register_automaton_functions()
      {
        using aut_t = Aut;
        using namespace dyn::detail;
        REGISTER(copy, aut_t);
        REGISTER(dot, aut_t, std::ostream, bool);
        REGISTER(efsm, aut_t, std::ostream);
        REGISTER(info, aut_t, std::ostream, bool);
        REGISTER(proper, aut_t, direction, bool);
        REGISTER(tikz, aut_t, std::ostream);
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
      register_functions_is_free(std::true_type)
      {
        using ctx_t = Ctx;
        using aut_t = mutable_automaton<ctx_t>;
        using rs_t = expressionset<ctx_t>;

        // Wordset.
        using wls_t = vcsn::detail::law_t<labelset_t_of<Ctx>>;

        // Word polynomialset.
        using wps_t = vcsn::detail::word_polynomialset_t<ctx_t>;

        // Same labelset, but over Booleans.
        using b_ctx_t = context<labelset_t_of<Ctx>, b>;
        using b_rs_t = expressionset<b_ctx_t>;

        using namespace dyn::detail;
        REGISTER(are_isomorphic, aut_t, aut_t);
        REGISTER(cerny, ctx_t, unsigned);
        REGISTER(complete, aut_t);
        REGISTER(concatenate_polynomial, wps_t, wps_t);
        REGISTER(de_bruijn, ctx_t, unsigned);
        REGISTER(derivation, rs_t, wls_t, bool);
        REGISTER(difference_expression, rs_t, b_rs_t);
        REGISTER(divkbaseb, ctx_t, unsigned, unsigned);
        REGISTER(double_ring, ctx_t, unsigned, const std::vector<unsigned>);
        REGISTER(enumerate, aut_t, unsigned);
        REGISTER(eval, aut_t, wls_t);
        REGISTER(infiltration, aut_t, aut_t);
        REGISTER(is_ambiguous, aut_t);
        REGISTER(is_complete, aut_t);
        REGISTER(is_deterministic, aut_t);
        REGISTER(is_synchronized_by, aut_t, wls_t);
        REGISTER(is_synchronizing, aut_t);
        REGISTER(ladybird, ctx_t, unsigned);
        REGISTER(list_polynomial, wps_t, std::ostream);
        REGISTER(pair, aut_t, bool);
        REGISTER(power, aut_t, unsigned);
        REGISTER(print_polynomial, wps_t, std::ostream, const std::string);
        REGISTER(product, aut_t, aut_t);
        REGISTER(random_automaton_deterministic, ctx_t, unsigned);
        REGISTER(shortest, aut_t, unsigned);
        REGISTER(shuffle, aut_t, aut_t);
        REGISTER(synchronizing_word, aut_t, const std::string);
        REGISTER(u, ctx_t, unsigned);

        return true;
      }

      template <typename Ctx>
      bool
      register_functions_is_free(std::false_type)
      {
        return true;
      }

      template <typename Ctx>
      bool
      register_kind_functions(labels_are_one)
      {
        using ctx_t = Ctx;
        using aut_t = mutable_automaton<ctx_t>;

        using namespace dyn::detail;

        REGISTER(eliminate_state, aut_t, int);

        return true;
      }

      template <typename Ctx>
      bool
      register_kind_functions(labels_are_nullable)
      {
        using ctx_t = Ctx;

        using namespace dyn::detail;

        REGISTER(de_bruijn, ctx_t, unsigned);
        REGISTER(divkbaseb, ctx_t, unsigned, unsigned);
        REGISTER(double_ring, ctx_t, unsigned, const std::vector<unsigned>);
        REGISTER(ladybird, ctx_t, unsigned);
        REGISTER(u, ctx_t, unsigned);

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
        // expression polynomialset.
        using rps_t = rat::expression_polynomialset_t<rs_t>;
        // expansionset.
        using es_t = rat::expansionset<rs_t>;


        using namespace dyn::detail;

        register_automaton_functions<aut_t>();

        REGISTER(accessible, aut_t);
        REGISTER(are_isomorphic, aut_t, aut_t);
        REGISTER(chain, aut_t, int, int);
        REGISTER(chain_expression, rs_t, int, int);
        REGISTER(coaccessible, aut_t);
        REGISTER(complement_expression, rs_t);
        REGISTER(concatenate, aut_t, aut_t);
        REGISTER(concatenate_expression, rs_t, rs_t);
        REGISTER(conjunction_expression, rs_t, rs_t);
        REGISTER(constant_term, rs_t);
        REGISTER(context_of, aut_t);
        REGISTER(context_of_expression, rs_t);
        REGISTER(derived_term, rs_t, const std::string);
        REGISTER(expand, rs_t);
        REGISTER(identities, rs_t);
        REGISTER(info_expression, rs_t, std::ostream);
        REGISTER(is_empty, aut_t);
        REGISTER(is_eps_acyclic, aut_t);
        REGISTER(is_normalized, aut_t);
        REGISTER(is_out_sorted, aut_t);
        REGISTER(is_proper, aut_t);
        REGISTER(is_standard, aut_t);
        REGISTER(is_trim, aut_t);
        REGISTER(is_useless, aut_t);
        REGISTER(is_valid, aut_t);
        REGISTER(is_valid_expression, rs_t);
        REGISTER(left_mult, ws_t, aut_t);
        REGISTER(left_mult_expression, ws_t, rs_t);
        REGISTER(lift_automaton, aut_t);
        REGISTER(lift_expression, rs_t);
        REGISTER(list_polynomial, rps_t, std::ostream);
        REGISTER(make_automaton_editor, ctx_t);
        REGISTER(make_context, ctx_t);
        REGISTER(make_expressionset, ctx_t, rat::identities);
        REGISTER(make_word_context, ctx_t);
        REGISTER(minimize, aut_t, const std::string);
        REGISTER(multiply_weight, ws_t, ws_t);
        REGISTER(print_ctx, ctx_t, std::ostream, const std::string);
        REGISTER(print_expansion, es_t, std::ostream, const std::string);
        REGISTER(print_label, ls_t, std::ostream, const std::string);
        REGISTER(print_polynomial, lps_t, std::ostream, const std::string);
        REGISTER(print_polynomial, rps_t, std::ostream, const std::string);
        REGISTER(print_expression, rs_t, std::ostream, const std::string);
        REGISTER(print_weight, ws_t, std::ostream, const std::string);
        REGISTER(push_weights, aut_t);
        REGISTER(random_automaton, ctx_t, unsigned, float, unsigned, unsigned);
        REGISTER(read_label, ctx_t, std::istream);
        REGISTER(read_polynomial, ctx_t, std::istream);
        REGISTER(read_weight, ctx_t, std::istream);
        REGISTER(right_mult, aut_t, ws_t);
        REGISTER(right_mult_expression, rs_t, ws_t);
        REGISTER(sort, aut_t);
        REGISTER(split, rs_t);
        REGISTER(standard, aut_t);
        REGISTER(standard_expression, rs_t);
        REGISTER(star, aut_t);
        REGISTER(star_height, rs_t);
        REGISTER(sum, aut_t, aut_t);
        REGISTER(sum_expression, rs_t, rs_t);
        REGISTER(sum_weight, ws_t, ws_t);
        REGISTER(thompson, rs_t);
        REGISTER(to_expansion, rs_t);
        REGISTER(to_expression, aut_t, const std::string);
        REGISTER(transpose, aut_t);
        REGISTER(transpose_expression, rs_t);
        REGISTER(transposition_expression, rs_t);
        REGISTER(trim, aut_t);
        REGISTER(union_a, aut_t, aut_t);

        using is_free_t
          = std::integral_constant<bool, ctx_t::labelset_t::is_free()>;
        register_functions_is_free<ctx_t>(is_free_t());

        return register_kind_functions<ctx_t>(typename ctx_t::kind_t());
      }
    }
  }

#undef REGISTER

#if VCSN_INSTANTIATION
# define VCSN_CTX_INSTANTIATE_2(Ctx)                            \
  namespace ctx                                                 \
  {                                                             \
    namespace detail                                            \
    {                                                           \
      static bool registered = register_functions<Ctx>();       \
    }                                                           \
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
