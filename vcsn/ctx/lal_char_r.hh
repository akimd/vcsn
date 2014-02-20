#ifndef VCSN_CTX_LAL_CHAR_R_HH
# define VCSN_CTX_LAL_CHAR_R_HH

# include <vcsn/ctx/context.hh>
# include <vcsn/ctx/lal_char.hh>
# include <vcsn/weights/r.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_r = context<lal_char, vcsn::r>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(ctx::lal_char_r);

# if VCSN_INSTANTIATION
  namespace ctx
  {
    namespace detail
    {
      template <typename Ctx>
      bool
      register_q_functions()
      {
        using aut_t = mutable_automaton<Ctx>;

        // Same labelset, but over Z.
        using z_ctx_t = context<typename Ctx::labelset_t, z>;
        using z_aut_t = mutable_automaton<z_ctx_t>;

        // Same labelset, but over Q.
        using q_ctx_t = context<typename Ctx::labelset_t, q>;
        using q_aut_t = mutable_automaton<q_ctx_t>;

        using namespace dyn::detail;

#define REGISTER(Algo, ...)                                             \
        Algo ## _register(sname<__VA_ARGS__>(), Algo<__VA_ARGS__>)

        // With Q.
        REGISTER(infiltration, aut_t, q_aut_t);
        REGISTER(infiltration, q_aut_t, aut_t);
        REGISTER(product, aut_t, q_aut_t);
        REGISTER(product, q_aut_t, aut_t);
        REGISTER(shuffle, aut_t, q_aut_t);
        REGISTER(shuffle, q_aut_t, aut_t);
        REGISTER(union_a, aut_t, q_aut_t);
        REGISTER(union_a, q_aut_t, aut_t);

        // With Z.
        REGISTER(infiltration, aut_t, z_aut_t);
        REGISTER(infiltration, z_aut_t, aut_t);
        REGISTER(product, aut_t, z_aut_t);
        REGISTER(product, z_aut_t, aut_t);
        REGISTER(shuffle, aut_t, z_aut_t);
        REGISTER(shuffle, z_aut_t, aut_t);
        REGISTER(union_a, aut_t, z_aut_t);
        REGISTER(union_a, z_aut_t, aut_t);
#  undef REGISTER

        return true;
      }

      static bool register_lal_char_r = register_q_functions<ctx::lal_char_r>();
    }
  }
# endif // ! VCSN_INSTANTIATION
}

#endif // !VCSN_CTX_LAL_CHAR_R_HH
