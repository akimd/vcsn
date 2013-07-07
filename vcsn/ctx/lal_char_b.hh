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
  determinize(const mutable_automaton<ctx::lal_char_b>& aut)
    -> mutable_automaton<ctx::lal_char_b>;

  /* grail. */
  MAYBE_EXTERN template
  std::ostream& grail<mutable_automaton<ctx::lal_char_b>>
  (const mutable_automaton<ctx::lal_char_b>& aut, std::ostream& out,
	const std::string& type);

  MAYBE_EXTERN template
  std::ostream&
  grail<vcsn::detail::transpose_automaton<mutable_automaton<ctx::lal_char_b>>>
  (const vcsn::detail::transpose_automaton<mutable_automaton<ctx::lal_char_b>>& aut,
   std::ostream& out, const std::string& type);

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

        are_equivalent_register(aut_t::sname(), aut_t::sname(),
                                are_equivalent<aut_t, aut_t>);
        complement_register(aut_t::sname(), complement<aut_t>);
        determinize_register(aut_t::sname(), determinize<aut_t>);
        grail_register(aut_t::sname(), grail<aut_t>);
        universal_register(aut_t::sname(), universal<aut_t>);
        return true;
      }

      static bool register_lal_char_b = register_functions<ctx::lal_char_b>();
    }
  }
#endif
}

#endif // !VCSN_CTX_LAL_CHAR_B_HH
