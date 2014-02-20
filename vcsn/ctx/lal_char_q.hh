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
        using ctx_t = Ctx;
        using aut_t = mutable_automaton<ctx_t>;

        // Same labelset, but over Z.
        using z_ctx_t = context<typename ctx_t::labelset_t, z>;
        using z_aut_t = mutable_automaton<z_ctx_t>;

        // Same labelset, but of B RatE.
        using b_ctx_t  = context<typename ctx_t::labelset_t, b>;
        using br_ctx_t = context<lal_char, ratexpset<b_ctx_t>>;
        using br_aut_t = mutable_automaton<br_ctx_t>;

        // ratexpset on Q.
        using rs_t = ratexpset<ctx_t>;
        // ratexpset on Z.
        using z_rs_t = ratexpset<z_ctx_t>;
        // ratexpset on B.
        using b_rs_t = ratexpset<b_ctx_t>;

        using namespace dyn::detail;

#define REGISTER(Algo, ...)                                             \
        Algo ## _register(sname<__VA_ARGS__>(), Algo<__VA_ARGS__>)

        REGISTER(infiltration, aut_t, z_aut_t);
        REGISTER(infiltration, z_aut_t, aut_t);
        REGISTER(product, aut_t, z_aut_t);
        REGISTER(product, z_aut_t, aut_t);
        REGISTER(shuffle, aut_t, z_aut_t);
        REGISTER(shuffle, z_aut_t, aut_t);
        REGISTER(union_a, aut_t, z_aut_t);
        REGISTER(union_a, z_aut_t, aut_t);

        REGISTER(infiltration, aut_t, br_aut_t);
        REGISTER(infiltration, br_aut_t, aut_t);
        REGISTER(product, aut_t, br_aut_t);
        REGISTER(product, br_aut_t, aut_t);
        REGISTER(shuffle, aut_t, br_aut_t);
        REGISTER(shuffle, br_aut_t, aut_t);
        REGISTER(union_a, aut_t, br_aut_t);
        REGISTER(union_a, br_aut_t, aut_t);

        //        REGISTER(copy_exp, rs_t, b_rs_t);
        REGISTER(copy_exp, b_rs_t, rs_t);
        //        REGISTER(copy_exp, rs_t, z_rs_t);
        REGISTER(copy_exp, z_rs_t, rs_t);
#  undef REGISTER

        return true;
      }

      static bool register_lal_char_q = register_q_functions<ctx::lal_char_q>();
    }
  }
# endif // ! VCSN_INSTANTIATION
}

#endif // !VCSN_CTX_LAL_CHAR_Q_HH
