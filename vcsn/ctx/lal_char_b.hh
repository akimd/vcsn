#ifndef VCSN_CTX_LAL_CHAR_B_HH
# define VCSN_CTX_LAL_CHAR_B_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/ctx/ctx.hh>
# include <vcsn/ctx/letterset.hh>
# include <vcsn/weights/b.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_b = context<letterset<vcsn::set_alphabet<vcsn::char_letters>>,
                               vcsn::b>;
  }
}

# include <vcsn/ctx/instantiate.hh>

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
    namespace details
    {
      template <typename Ctx>
      bool
      register_functions()
      {
        using aut_t = mutable_automaton<Ctx>;
        using namespace dyn::details;

        determinize_register(aut_t::sname(), determinize<aut_t>);
        return true;
      }

      static bool register_lal_char_b = register_functions<ctx::lal_char_b>();
    }
  }
#endif
}

#endif // !VCSN_CTX_LAL_CHAR_B_HH
