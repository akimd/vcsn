#ifndef VCSN_CTX_LAN_CHAR_B_HH
# define VCSN_CTX_LAN_CHAR_B_HH

# include <vcsn/ctx/context.hh>
# include <vcsn/ctx/lan_char.hh>
# include <vcsn/weights/b.hh>

namespace vcsn
{
  namespace ctx
  {
    using lan_char_b = context<lan_char, vcsn::b>;
  }
}

# include <vcsn/ctx/instantiate.hh>
# include <vcsn/algos/grail.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(lan_char_b);

  /* fado. */
  MAYBE_EXTERN template
  std::ostream& fado<mutable_automaton<ctx::lan_char_b>>
  (const mutable_automaton<ctx::lan_char_b>& aut, std::ostream& out);

  MAYBE_EXTERN template
  std::ostream&
  fado<vcsn::detail::transpose_automaton<mutable_automaton<ctx::lan_char_b>>>
  (const vcsn::detail::transpose_automaton<mutable_automaton<ctx::lan_char_b>>& aut,
   std::ostream& out);

  /* grail. */
  MAYBE_EXTERN template
  std::ostream& grail<mutable_automaton<ctx::lan_char_b>>
  (const mutable_automaton<ctx::lan_char_b>& aut, std::ostream& out);

  MAYBE_EXTERN template
  std::ostream&
  grail<vcsn::detail::transpose_automaton<mutable_automaton<ctx::lan_char_b>>>
  (const vcsn::detail::transpose_automaton<mutable_automaton<ctx::lan_char_b>>& aut,
   std::ostream& out);

# if VCSN_INSTANTIATION
  namespace
  {
    namespace detail
    {
      template <typename Ctx>
      bool
      register_functions()
      {
        using aut_t = mutable_automaton<Ctx>;
        using namespace dyn::detail;

#  define REGISTER2(Algo, Type1, Type2)                         \
        Algo ## _register(sname<Type1>(), sname<Type2>(),       \
                          Algo<Type1, Type2>)
        REGISTER2(fado, aut_t, std::ostream);
        REGISTER2(grail, aut_t, std::ostream);
# undef REGISTER2
        return true;
      }

      static bool register_lan_char_b = register_functions<ctx::lan_char_b>();
    }
  }
#endif
}

#endif // !VCSN_CTX_LAN_CHAR_B_HH
