#ifndef VCSN_CTX_LAL_CHAR_Q_HH
# define VCSN_CTX_LAL_CHAR_Q_HH

# include <vcsn/ctx/context.hh>
# include <vcsn/ctx/lal_char.hh>
# include <vcsn/weights/q.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_q = context<lal_char, vcsn::q>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(ctx::lal_char_q);

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

        using namespace dyn::detail;

#define REGISTER(Algo, ...)                                             \
        Algo ## _register(sname<__VA_ARGS__>(), Algo<__VA_ARGS__>)

        REGISTER(infiltration, aut_t, z_aut_t);
        REGISTER(infiltration, z_aut_t, aut_t);
        REGISTER(product, aut_t, z_aut_t);
        REGISTER(product, z_aut_t, aut_t);
        REGISTER(shuffle, aut_t, z_aut_t);
        REGISTER(shuffle, z_aut_t, aut_t);
#  undef REGISTER

        return true;
      }

      static bool register_lal_char_q = register_q_functions<ctx::lal_char_q>();
    }
  }
# endif // ! VCSN_INSTANTIATION
}

#endif // !VCSN_CTX_LAL_CHAR_Q_HH
