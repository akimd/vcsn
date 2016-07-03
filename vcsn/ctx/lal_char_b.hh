#pragma once

#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/lal_char.hh>
#include <vcsn/weightset/b.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_b = context<lal_char, b>;
  }
}

#include <vcsn/algos/are-equivalent.hh>
#include <vcsn/algos/complement.hh>
#include <vcsn/algos/star-normal-form.hh>
#include <vcsn/algos/universal.hh>
#include <vcsn/ctx/instantiate.hh>
#include <vcsn/misc/attributes.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(ctx::lal_char_b);
#if 0
  MAYBE_EXTERN template
  auto
  determinize(const mutable_automaton<ctx::lal_char_b>& aut)
    -> mutable_automaton<ctx::lal_char_b>;

  /* fado. */
  MAYBE_EXTERN template
  std::ostream& fado<mutable_automaton<ctx::lal_char_b>>
  (const mutable_automaton<ctx::lal_char_b>& aut, std::ostream& out);

  MAYBE_EXTERN template
  std::ostream&
  fado<vcsn::detail::transpose_automaton<mutable_automaton<ctx::lal_char_b>>>
  (const vcsn::detail::transpose_automaton<mutable_automaton<ctx::lal_char_b>>& aut,
   std::ostream& out);

  /* grail. */
  MAYBE_EXTERN template
  std::ostream& grail<mutable_automaton<ctx::lal_char_b>>
  (const mutable_automaton<ctx::lal_char_b>& aut, std::ostream& out);

  MAYBE_EXTERN template
  std::ostream&
  grail<vcsn::detail::transpose_automaton<mutable_automaton<ctx::lal_char_b>>>
  (const vcsn::detail::transpose_automaton<mutable_automaton<ctx::lal_char_b>>& aut,
   std::ostream& out);
#endif

#if VCSN_INSTANTIATION
  namespace ctx
  {
    namespace detail
    {
      template <typename Ctx>
      bool
      register_b_functions()
      {
        using ctx_t = Ctx;
        using aut_t = mutable_automaton<ctx_t>;
        using rs_t = expressionset<ctx_t>;

# define REGISTER(Algo, ...)                                            \
        dyn::detail::Algo ## _register(ssignature<__VA_ARGS__>(),       \
                                       dyn::detail::Algo<__VA_ARGS__>)

        REGISTER(are_equivalent, aut_t, aut_t);
        REGISTER(complement, aut_t);
        REGISTER(star_normal_form, rs_t);
        REGISTER(universal, aut_t);
# undef REGISTER

        return true;
      }

      static bool register_lal_char_b ATTRIBUTE_USED
        = register_b_functions<ctx::lal_char_b>();
    }
  }
#endif // ! VCSN_INSTANTIATION
}
