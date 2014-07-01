#ifndef VCSN_ALGOS_PREFIX_HH
# define VCSN_ALGOS_PREFIX_HH

# include <vcsn/algos/accessible.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/dyn/fwd.hh>

namespace vcsn
{
  /*---------.
  | suffix.  |
  `---------*/

  /// Make all accessible states initial.
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


  /*---------.
  | prefix.  |
  `---------*/

  /// Make all coaccessible states final.
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

  /*---------.
  | factor.  |
  `---------*/

  /// Make each useful state both initial and final.
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
      // Bridge.
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

  /*----------.
  | subword.  |
  `----------*/

  /// Add spontaneous transitions for each non spontaneous transition,
  /// with same source, destination and weight.
  template <typename Aut>
  Aut&
  subword_here(Aut& aut)
  {
    const auto one = aut->labelset()->one();

    std::vector<transition_t_of<Aut>> ts;
    for (auto s : aut->states())
      {
        ts.clear();
        for (auto t : aut->out(s))
          if (!aut->labelset()->is_one(aut->label_of(t)))
            ts.emplace_back(t);
        for (auto t : ts)
          aut->add_transition(s, aut->dst_of(t), one, aut->weight_of(t));
      }

    return aut;
  }

  /// Apply subword_here() to a copy of \a aut.
  template <typename Aut>
  auto
  subword(const Aut& aut)
    ->decltype(::vcsn::copy(aut))
  {
    auto res = ::vcsn::copy(aut);
    subword_here(res);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      automaton
      subword(const automaton& aut)
      {
	const auto& a = aut->as<Aut>();
	return make_automaton(::vcsn::subword(a));
      }

      REGISTER_DECLARE(subword,
		       (const automaton& aut) -> automaton);
    }
  }
} // namespace vcsn

#endif // !VCSN_ALGOS_PREFIX_HH
