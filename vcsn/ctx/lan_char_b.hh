#pragma once

#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/lan_char.hh>
#include <vcsn/weightset/b.hh>

namespace vcsn
{
  namespace ctx
  {
    using lan_char_b = context<lan_char, vcsn::b>;
  }
}

#include <vcsn/ctx/instantiate.hh>
#include <vcsn/algos/grail.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(ctx::lan_char_b);
#if 0
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
#endif

#if VCSN_INSTANTIATION
  namespace
  {
    namespace detail
    {
      template <typename Ctx>
      bool
      register_functions()
      {
        using aut_t = mutable_automaton<Ctx>;

# define REGISTER(Algo, ...)                                            \
        dyn::detail::Algo ## _register(ssignature<__VA_ARGS__>(),       \
                                       dyn::detail::Algo<__VA_ARGS__>)

        REGISTER(fado, aut_t, std::ostream);
        REGISTER(grail, aut_t, std::ostream);
# undef REGISTER
        return true;
      }

      static bool register_lan_char_b ATTRIBUTE_USED
        = register_functions<ctx::lan_char_b>();
    }
  }
#endif
}
