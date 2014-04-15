#ifndef VCSN_CTX_LAL_CHAR_HH
# define VCSN_CTX_LAL_CHAR_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/labelset/letterset.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char = letterset<vcsn::set_alphabet<vcsn::char_letters>>;
  }
}

# include <vcsn/ctx/instantiate.hh>
# include <vcsn/algos/are-equivalent.hh> // difference
# include <vcsn/algos/are-isomorphic.hh>
# include <vcsn/algos/complete.hh>
# include <vcsn/algos/derivation.hh>
# include <vcsn/algos/enumerate.hh>
# include <vcsn/algos/eval.hh>
# include <vcsn/algos/is-ambiguous.hh>
# include <vcsn/algos/is-complete.hh>
# include <vcsn/algos/is-deterministic.hh>
# include <vcsn/algos/minimize.hh>
# include <vcsn/algos/product.hh>
# include <vcsn/algos/synchronizing-word.hh>

namespace vcsn
{
# if VCSN_INSTANTIATION
#define REGISTER(Algo, ...)                                     \
  Algo ## _register(ssignature<__VA_ARGS__>(), Algo<__VA_ARGS__>)

  namespace ctx
  {
    namespace detail
    {

      template <typename Ctx>
      bool
      register_kind_functions(labels_are_letters)
      {
        using ctx_t = Ctx;
        using aut_t = mutable_automaton<ctx_t>;
        using rs_t = ratexpset<ctx_t>;

        // Wordset.
        using wls_t = vcsn::detail::law_t<typename Ctx::labelset_t>;

        // Word polynomialset.
        using wps_t = vcsn::detail::word_polynomialset_t<ctx_t>;

        // Same labelset, but over Booleans.
        using b_ctx_t = context<typename Ctx::labelset_t, b>;
        using b_aut_t = mutable_automaton<b_ctx_t>;
        using b_rs_t = ratexpset<b_ctx_t>;

        using namespace dyn::detail;

        REGISTER(are_isomorphic, aut_t, aut_t);
        REGISTER(complete, aut_t);
        REGISTER(de_bruijn, Ctx, unsigned);
        REGISTER(derivation, rs_t, wls_t, bool);
        REGISTER(derived_term, rs_t, bool);
        REGISTER(difference, aut_t, b_aut_t);
        REGISTER(difference_ratexp, rs_t, b_rs_t);
        REGISTER(divkbaseb, Ctx, unsigned, unsigned);
        REGISTER(double_ring, Ctx, unsigned, const std::vector<unsigned>);
        REGISTER(enumerate, aut_t, unsigned);
        REGISTER(eval, aut_t, wls_t);
        REGISTER(infiltration, aut_t, aut_t);
        REGISTER(is_ambiguous, aut_t);
        REGISTER(is_complete, aut_t);
        REGISTER(is_deterministic, aut_t);
        REGISTER(is_synchronized_by, aut_t, const std::string);
        REGISTER(ladybird, Ctx, unsigned);
        REGISTER(list_polynomial, wps_t, std::ostream);
        REGISTER(minimize, aut_t, const std::string);
        REGISTER(pair, aut_t);
        REGISTER(print_polynomial, wps_t, std::ostream, const std::string);
        REGISTER(power, aut_t, unsigned);
        REGISTER(product, aut_t, aut_t);
        REGISTER(random, Ctx, unsigned, float, unsigned, unsigned);
        REGISTER(shortest, aut_t, unsigned);
        REGISTER(shuffle, aut_t, aut_t);
        REGISTER(synchronizing_word, aut_t);
        REGISTER(u, Ctx, unsigned);

        return true;
      }
    }
  }
# undef REGISTER
#endif // !VCSN_INSTANTIATION
}

#endif // !VCSN_CTX_LAL_CHAR_HH
