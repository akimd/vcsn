#ifndef VCSN_CTX_LAL_CHAR_B_HH
# define VCSN_CTX_LAL_CHAR_B_HH

# include <vcsn/ctx/context.hh>
# include <vcsn/ctx/lal_char.hh>
# include <vcsn/weights/b.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_b = context<lal_char, b>;
  }
}

# include <vcsn/misc/name.hh>
# include <vcsn/ctx/instantiate.hh>
# include <vcsn/algos/are-equivalent.hh>
# include <vcsn/algos/complement.hh>
# include <vcsn/algos/determinize.hh>
# include <vcsn/algos/grail.hh>
# include <vcsn/algos/universal.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(lal_char_b);

  MAYBE_EXTERN template
  auto
  determinize(const mutable_automaton<ctx::lal_char_b>& aut, bool complete)
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

#  define REGISTER(Algo, Type)                          \
        Algo ## _register(Type::sname(), Algo<Type>)
#  define REGISTER2(Algo, Type1, Type2)                         \
        Algo ## _register(sname<Type1>(), sname<Type2>(),       \
                          Algo<Type1, Type2>)

        REGISTER2(are_equivalent, aut_t, aut_t);
        REGISTER(complement, aut_t);
        REGISTER2(determinize, aut_t, bool);
        // FIXME: support more product types.
        REGISTER2(difference, aut_t, aut_t);
        REGISTER(fado, aut_t);
        REGISTER(grail, aut_t);
        REGISTER(universal, aut_t);
#  undef REGISTER2
#  undef REGISTER

        return true;
      }

      static bool register_lal_char_b = register_functions<ctx::lal_char_b>();
    }
  }
# endif // ! VCSN_INSTANTIATION
}

#endif // !VCSN_CTX_LAL_CHAR_B_HH
