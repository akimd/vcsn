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
# include <vcsn/algos/complete.hh>
# include <vcsn/algos/derivation.hh>
# include <vcsn/algos/enumerate.hh>
# include <vcsn/algos/eval.hh>
# include <vcsn/algos/is-ambiguous.hh>
# include <vcsn/algos/is-complete.hh>
# include <vcsn/algos/is-deterministic.hh>
# include <vcsn/algos/product.hh>

namespace vcsn
{
# if VCSN_INSTANTIATION
#define REGISTER(Algo, ...)                                     \
  Algo ## _register(sname<__VA_ARGS__>(), Algo<__VA_ARGS__>)

  namespace ctx
  {
    namespace detail
    {
      template <typename Ctx>
      bool
      register_kind_functions(labels_are_letters)
      {
        using aut_t = mutable_automaton<Ctx>;
        // Word polynomialset.
        using wps_t = typename vcsn::detail::law_traits<Ctx>::polynomialset_t;
        using rs_t = ratexpset<Ctx>;

        // Same labelset, but over Booleans.
        using b_ctx_t = context<typename Ctx::labelset_t, b>;
        using b_aut_t = mutable_automaton<b_ctx_t>;

        using namespace dyn::detail;

        REGISTER(complete, aut_t);
        REGISTER(de_bruijn, Ctx, unsigned);
        REGISTER(derivation, rs_t, const std::string);
        REGISTER(derived_term, rs_t);
        REGISTER(difference, aut_t, b_aut_t);
        REGISTER(divkbaseb, Ctx, unsigned, unsigned);
        REGISTER(double_ring, Ctx, unsigned, const std::vector<unsigned>);
        REGISTER(enumerate, aut_t, unsigned);
        REGISTER(eval, aut_t, const std::string);
        REGISTER(infiltration, aut_t, aut_t);
        REGISTER(infiltration, aut_t, b_aut_t);
        REGISTER(infiltration, b_aut_t, aut_t);
        REGISTER(is_ambiguous, aut_t);
        REGISTER(is_complete, aut_t);
        REGISTER(is_deterministic, aut_t);
        REGISTER(ladybird, Ctx, unsigned);
        REGISTER(list_polynomial, wps_t, std::ostream);
        REGISTER(print_polynomial, wps_t, std::ostream);
        REGISTER(power, aut_t, unsigned);
        REGISTER(product, aut_t, aut_t);
        REGISTER(product, aut_t, b_aut_t);
        REGISTER(product, b_aut_t, aut_t);
        REGISTER(random, Ctx, unsigned, float, unsigned, unsigned);
        REGISTER(shortest, aut_t, unsigned);
        REGISTER(shuffle, aut_t, aut_t);
        REGISTER(shuffle, aut_t, b_aut_t);
        REGISTER(shuffle, b_aut_t, aut_t);
        REGISTER(u, Ctx, unsigned);

        return true;
      }
    }
  }
# undef REGISTER
#endif // !VCSN_INSTANTIATION
}

#endif // !VCSN_CTX_LAL_CHAR_HH
