#ifndef VCSN_CTX_LAN_CHAR_B_HH
# define VCSN_CTX_LAN_CHAR_B_HH

# include <vcsn/ctx/ctx.hh>
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

  /* grail. */
  MAYBE_EXTERN template
  std::ostream& grail<mutable_automaton<ctx::lan_char_b>>
  (const mutable_automaton<ctx::lan_char_b>& aut, std::ostream& out,
	const std::string& type);

  MAYBE_EXTERN template
  std::ostream&
  grail<vcsn::detail::transpose_automaton<mutable_automaton<ctx::lan_char_b>>>
  (const vcsn::detail::transpose_automaton<mutable_automaton<ctx::lan_char_b>>& aut,
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

        grail_register(aut_t::sname(), grail<aut_t>);
        return true;
      }

      static bool register_lan_char_b = register_functions<ctx::lan_char_b>();
    }
  }
#endif
}

#endif // !VCSN_CTX_LAN_CHAR_B_HH
