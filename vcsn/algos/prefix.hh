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
        return make_automaton(::vcsn::suffix(a));
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
        return make_automaton(::vcsn::prefix(a));
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
	return make_automaton(::vcsn::factor(a));
      }

      REGISTER_DECLARE(factor,
		       (const automaton& aut) -> automaton);
    }
  }

  /// Create a subsequence automaton from \a aut and return it
  template <typename Aut>
  Aut&
  subsequence_here(Aut& aut)
  {
    const auto epsilon = aut->labelset()->one();
    using state_t = state_t_of<Aut>;

    for (auto s : aut->states())
      {
	std::vector<state_t> v;
	for (auto tr : aut->in(s))
	  {
	    auto si = aut->src_of(tr);
	    // Loop of set of transitions
	    // Do not add transtions here
	    // Put it to vector and add later
	    v.push_back(si);
	  }
	for (auto si : v)
	    if (s != si)
	      aut->add_transition(si, s, epsilon);
      }
    return aut;
  }

  template <typename Aut>
  auto
  subsequence(const Aut& aut)
    ->decltype(::vcsn::copy(aut))
  {
    auto res = ::vcsn::copy(aut);
    subsequence_here(res);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      // Brige.
      template <typename Aut>
      automaton
      subsequence(const automaton& aut)
      {
	const auto& a = aut->as<Aut>();
	return make_automaton(::vcsn::subsequence(a));
      }

      REGISTER_DECLARE(subsequence,
		       (const automaton& aut) -> automaton);
    }
  }
} // namespace vcsn

#endif // !VCSN_ALGOS_PREFIX_HH
