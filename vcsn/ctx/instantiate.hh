#ifndef VCSN_CTX_INSTANTIATE_HH
# define VCSN_CTX_INSTANTIATE_HH

# include <vcsn/algos/accessible.hh>
# include <vcsn/algos/are-isomorphic.hh>
# include <vcsn/algos/aut-to-exp.hh>
# include <vcsn/algos/concatenate.hh>
# include <vcsn/algos/constant-term.hh>
# include <vcsn/algos/copy.hh>
# include <vcsn/algos/dot.hh>
# include <vcsn/algos/edit-automaton.hh>
# include <vcsn/algos/efsm.hh>
# include <vcsn/algos/expand.hh>
# include <vcsn/algos/first-order.hh>
# include <vcsn/algos/info.hh>
# include <vcsn/algos/is-eps-acyclic.hh>
# include <vcsn/algos/is-normalized.hh>
# include <vcsn/algos/is-proper.hh>
# include <vcsn/algos/is-valid.hh>
# include <vcsn/algos/left-mult.hh>
# include <vcsn/algos/lift.hh>
# include <vcsn/algos/make-context.hh>
# include <vcsn/algos/print.hh>
# include <vcsn/algos/proper.hh>
# include <vcsn/algos/read.hh>
# include <vcsn/algos/split.hh>
# include <vcsn/algos/standard.hh>
# include <vcsn/algos/sort.hh>
# include <vcsn/algos/star.hh>
# include <vcsn/algos/star-height.hh>
# include <vcsn/algos/star-normal-form.hh>
# include <vcsn/algos/sum.hh>
# include <vcsn/algos/thompson.hh>
# include <vcsn/algos/tikz.hh>
# include <vcsn/algos/transpose.hh>
# include <vcsn/algos/union.hh>

# include <vcsn/factory/de-bruijn.hh>
# include <vcsn/factory/divkbaseb.hh>
# include <vcsn/factory/double-ring.hh>
# include <vcsn/factory/ladybird.hh>
# include <vcsn/factory/random.hh>
# include <vcsn/factory/u.hh>

# include <vcsn/weights/b.hh>
# include <vcsn/weights/polynomialset.hh>

# include <vcsn/misc/name.hh>

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
  LIBVCSN_API                                                           \
  std::ostream& Format<Aut>(const Aut& aut, std::ostream& out)

  /*-------------------------------------------------------.
  | Instantiate the function that work for every context.  |
  `-------------------------------------------------------*/

# define VCSN_CTX_INSTANTIATE_1(Ctx)                                    \
  MAYBE_EXTERN template                                                 \
  class LIBVCSN_API mutable_automaton<Ctx>;                             \
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
  VCSN_CTX_INSTANTIATE_PRINT(efsm, mutable_automaton<Ctx>);             \
  VCSN_CTX_INSTANTIATE_PRINT                                            \
  (efsm, vcsn::detail::transpose_automaton<mutable_automaton<Ctx>>);    \
                                                                        \
  /* lift. */                                                           \
  MAYBE_EXTERN template                                                 \
  detail::lifted_automaton_t<mutable_automaton<Ctx>>                    \
  lift<mutable_automaton<Ctx>>(const mutable_automaton<Ctx>& aut);      \
                                                                        \
  /* print. */                                                          \
  MAYBE_EXTERN template                                                 \
  std::ostream&                                                         \
  print<ratexpset<Ctx>>(const ratexpset<Ctx>& rs,                       \
                        const Ctx::ratexp_t& e, std::ostream& o,        \
                        const std::string& format);                     \
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
  class LIBVCSN_API detail::transposer<ratexpset<Ctx>>


  /*----------------------------------.
  | Register the abstract functions.  |
  `----------------------------------*/

#define REGISTER(Algo, ...)                                     \
  Algo ## _register(ssignature<__VA_ARGS__>(), Algo<__VA_ARGS__>)

  namespace ctx
  {
    namespace detail
    {
      template <typename Ctx>
      bool
      register_kind_functions(labels_are_letters);

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
      register_kind_functions(labels_are_ratexps)
      {
        using ctx_t = Ctx;
        using aut_t = mutable_automaton<ctx_t>;

        using namespace dyn::detail;

        REGISTER(eliminate_state, aut_t, int);

        return true;
      }

      template <typename Ctx>
      bool
      register_kind_functions(labels_are_tuples)
      ATTRIBUTE_CONST;

      template <typename Ctx>
      bool
      register_kind_functions(labels_are_tuples)
      {
        return true;
      }

      template <typename Ctx>
      bool
      register_kind_functions(labels_are_nullable)
      {
        using ctx_t = Ctx;
        using aut_t = mutable_automaton<ctx_t>;
        // Word polynomialset.

        // Same weightset, but letterset.
        using letterset_t = letterset<typename ctx_t::labelset_t::genset_t>;
        using l_ctx_t = context<letterset_t, typename ctx_t::weightset_t>;
        using l_aut_t = mutable_automaton<l_ctx_t>;

        // Letterset -> B RatE.
        using lb_ctx_t = context<letterset_t, b>;
        using lbr_ctx_t = context<letterset_t, ratexpset<lb_ctx_t>>;
        using lbr_aut_t = mutable_automaton<lbr_ctx_t>;

        using namespace dyn::detail;

        REGISTER(de_bruijn, ctx_t, unsigned);
        REGISTER(divkbaseb, ctx_t, unsigned, unsigned);
        REGISTER(double_ring, ctx_t, unsigned, const std::vector<unsigned>);
        REGISTER(ladybird, ctx_t, unsigned);
        REGISTER(random, ctx_t, unsigned, float, unsigned, unsigned);
        REGISTER(u, ctx_t, unsigned);

        REGISTER(union_a, aut_t, l_aut_t);
        REGISTER(union_a, l_aut_t, aut_t);
        REGISTER(union_a, aut_t, lbr_aut_t);
        REGISTER(union_a, lbr_aut_t, aut_t);

        // Ensure that we can at least print the resulting
        // automata.
        // FIXME: we really need something fine grained.
        REGISTER(dot, lbr_aut_t, std::ostream);
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
      register_functions(std::true_type)
      {
        using ctx_t = Ctx;
        using rs_t = ratexpset<ctx_t>;
        using namespace dyn::detail;

        REGISTER(thompson, rs_t);
        return true;
      }

      template <typename Ctx>
      bool
      register_functions(std::false_type)
      {
        return true;
      }

      template <typename Ctx>
      bool
      register_functions()
      {
        using ctx_t = Ctx;
        using aut_t = mutable_automaton<ctx_t>;
        using taut_t = vcsn::detail::transpose_automaton<aut_t>;
        using rs_t = ratexpset<ctx_t>;
        using ws_t = typename ctx_t::weightset_t;

        // Same labelset, but over Booleans.
        using b_ctx_t = context<typename ctx_t::labelset_t, b>;
        using b_aut_t = mutable_automaton<b_ctx_t>;

        // label polynomialset.
        using lps_t = polynomialset<ctx_t>;
        // ratexp polynomialset.
        using rps_t = typename rat::ratexp_polynomialset_t<rs_t>;

        using namespace dyn::detail;

        REGISTER(accessible, aut_t);
        REGISTER(are_isomorphic, aut_t, aut_t);
        REGISTER(aut_to_exp, aut_t);
        REGISTER(chain, aut_t, unsigned);
        REGISTER(chain_exp, rs_t, int, int);
        REGISTER(coaccessible, aut_t);
        REGISTER(concatenate, aut_t, aut_t);
        REGISTER(concatenate_ratexp, rs_t, rs_t);
        REGISTER(constant_term, rs_t);
        REGISTER(copy, aut_t);
        REGISTER(dot, aut_t, std::ostream);
        REGISTER(dot, taut_t, std::ostream);
        REGISTER(efsm, aut_t, std::ostream);
        REGISTER(efsm, taut_t, std::ostream);
        REGISTER(expand, rs_t);
        REGISTER(first_order, rs_t, bool);
        REGISTER(info, aut_t, std::ostream);
        REGISTER(info, taut_t, std::ostream);
        REGISTER(info_ratexp, rs_t, std::ostream);
        REGISTER(intersection_ratexp, rs_t, rs_t);
        REGISTER(is_empty, aut_t);
        REGISTER(is_eps_acyclic, aut_t);
        REGISTER(is_normalized, aut_t);
        REGISTER(is_proper, aut_t);
        REGISTER(is_standard, aut_t);
        REGISTER(is_trim, aut_t);
        REGISTER(is_useless, aut_t);
        REGISTER(is_valid, aut_t);
        REGISTER(is_valid_exp, rs_t);
        REGISTER(left_mult, ws_t, aut_t);
        REGISTER(left_mult_ratexp, ws_t, rs_t);
        REGISTER(lift_automaton, aut_t);
        REGISTER(lift_exp, rs_t);
        REGISTER(linear, rs_t, bool);
        REGISTER(list_polynomial, rps_t, std::ostream);
        REGISTER(make_automaton_editor, ctx_t);
        REGISTER(make_context, ctx_t);
        REGISTER(make_ratexpset, ctx_t);
        REGISTER(print_ctx, ctx_t, std::ostream, const std::string);
        REGISTER(print_ratexp, rs_t, std::ostream, const std::string);
        REGISTER(print_polynomial, lps_t, std::ostream, const std::string);
        REGISTER(print_polynomial, rps_t, std::ostream, const std::string);
        REGISTER(print_weight, ws_t, std::ostream, const std::string);
        REGISTER(proper, aut_t, bool);
        REGISTER(read_polynomial, ctx_t, std::istream);
        REGISTER(read_weight, ctx_t, std::istream);
        REGISTER(right_mult, aut_t, ws_t);
        REGISTER(right_mult_ratexp, rs_t, ws_t);
        REGISTER(split, rs_t);
        REGISTER(standard, aut_t);
        REGISTER(standard_exp, rs_t);
        REGISTER(sort, aut_t);
        REGISTER(sort, taut_t);
        REGISTER(star, aut_t);
        REGISTER(star_height, rs_t);
        REGISTER(star_normal_form, rs_t);
        REGISTER(sum, aut_t, aut_t);
        REGISTER(sum_ratexp, rs_t, rs_t);
        REGISTER(tikz, aut_t, std::ostream);
        REGISTER(tikz, taut_t, std::ostream);
        REGISTER(transpose, aut_t);
        REGISTER(transpose_exp, rs_t);
        REGISTER(trim, aut_t);
        REGISTER(union_a, aut_t, aut_t);
        REGISTER(union_a, aut_t, b_aut_t);
        REGISTER(union_a, b_aut_t, aut_t);

        register_functions<ctx_t>(std::integral_constant<bool, ctx_t::has_one()>());

        return register_kind_functions<ctx_t>(typename ctx_t::kind_t());
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
  VCSN_CTX_INSTANTIATE_1(Ctx);                  \
  VCSN_CTX_INSTANTIATE_2(Ctx)

# ifndef MAYBE_EXTERN
#  define MAYBE_EXTERN extern
# endif

}

#endif // !VCSN_CTX_INSTANTIATE_HH
