#ifndef VCSN_CTX_LAL_CHAR_B_HH
# define VCSN_CTX_LAL_CHAR_B_HH

# include <vcsn/ctx/ctx.hh>
# include <vcsn/ctx/lal_char.hh>
# include <vcsn/weights/b.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_b = context<lal_char, b>;
  }
}

# include <vcsn/ctx/instantiate.hh>
# include <vcsn/algos/determinize.hh>
# include <vcsn/algos/universal.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(lal_char_b);

  MAYBE_EXTERN template
  auto
  determinize(const mutable_automaton<ctx::lal_char_b>& aut)
    -> mutable_automaton<ctx::lal_char_b>;

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

        determinize_register(aut_t::sname(), determinize<aut_t>);
        universal_register(aut_t::sname(), universal<aut_t>);
        return true;
      }

      static bool register_lal_char_b = register_functions<ctx::lal_char_b>();
    }
  }
#endif
}

#endif // !VCSN_CTX_LAL_CHAR_B_HH
