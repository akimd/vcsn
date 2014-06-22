#ifndef VCSN_ALGOS_PREFIX_HH
# define VCSN_ALGOS_PREFIX_HH

# include <vcsn/algos/accessible.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/dyn/fwd.hh>

namespace vcsn
{
  /// Create a suffix automaton from \a aut and return it.
  template <typename Aut>
  Aut&
  suffix_here(Aut& aut)
  {
    for (auto s : accessible_states(aut))
      if (s != aut->pre() && s != aut->post())
        aut->set_initial(s);
    return aut;
  }

  template <typename Aut>
  auto
  suffix(const Aut& aut)
    ->decltype(::vcsn::copy(aut))
  {
    auto res = ::vcsn::copy(aut);
    suffix_here(res);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      automaton
      suffix(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(suffix(a));
      }

      REGISTER_DECLARE(suffix,
                       (const automaton& aut) -> automaton);
    }
  }


  /// Create a prefix automaton from \a aut and return it.
  template <typename Aut>
  Aut&
  prefix_here(Aut& aut)
  {
    auto t = transpose(aut);
    suffix_here(t);
    return aut;
  }

  template <typename Aut>
  auto
  prefix(const Aut& aut)
    ->decltype(::vcsn::copy(aut))
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

  /// Create a factor automaton from \a aut and return it
  template <typename Aut>
  Aut&
  factor_here(Aut& aut)
  {
    for (auto s : useful_states(aut))
      if (s != aut->pre() && s != aut->post())
	{
	  aut->set_initial(s);
	  aut->set_final(s);
	}
    return aut;
  }

  template <typename Aut>
  auto
  factor(const Aut& aut)
    ->decltype(::vcsn::copy(aut))
  {
    auto res = ::vcsn::copy(aut);
    factor_here(res);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      // Brige.
      template <typename Aut>
      automaton
      factor(const automaton& aut)
      {
	const auto& a = aut->as<Aut>();
	return make_automaton(factor(a));
      }

      REGISTER_DECLARE(factor,
		       (const automaton& aut) -> automaton);
    }
  }
} // namespace vcsn

#endif // !VCSN_ALGOS_PREFIX_HH
