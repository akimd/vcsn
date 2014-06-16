#ifndef VCSN_ALGOS_PREFIX_HH
# define VCSN_ALGOS_PREFIX_HH

# include <vcsn/algos/accessible.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/dyn/fwd.hh>

namespace vcsn
{
  template <typename Aut>
  Aut&
  prefix_here(Aut& aut)
  {
    for (auto s : coaccessible_states(aut))
      if (s != aut->pre() && s != aut->post())
	aut->set_final(s);
    return aut;
  }

  /// Create a prefix automaton from \a aut and return it.
  template <typename Aut>
  Aut
  prefix(const Aut& aut)
  {
    auto res = ::vcsn::copy(aut);
    prefix_here(res);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      automaton
      prefix(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(prefix(a));
      }

      REGISTER_DECLARE(prefix,
                       (const automaton& aut) -> automaton);
    }
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_PREFIX_HH
