#pragma once

#include <vcsn/algos/accessible.hh>
#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/dyn/fwd.hh>
#include <vcsn/labelset/labelset.hh>

namespace vcsn
{
  /*----------------.
  | Function tags.  |
  `----------------*/

  CREATE_FUNCTION_TAG(factor);
  CREATE_FUNCTION_TAG(prefix);
  CREATE_FUNCTION_TAG(suffix);
  CREATE_FUNCTION_TAG(subword);

  /*---------.
  | suffix.  |
  `---------*/

  /// Make all accessible states initial.
  template <Automaton Aut>
  Aut&
  suffix_here(Aut& aut)
  {
    for (auto s : accessible_states(aut))
      if (s != aut->pre()
          && s != aut->post()
          && !aut->is_initial(s))
        aut->set_initial(s);
    aut->properties().update(suffix_ftag{});
    return aut;
  }

  template <Automaton Aut>
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
      template <Automaton Aut>
      automaton
      suffix(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::suffix(a);
      }
    }
  }


  /*---------.
  | prefix.  |
  `---------*/

  /// Make all coaccessible states final.
  template <Automaton Aut>
  Aut&
  prefix_here(Aut& aut)
  {
    auto t = transpose(aut);
    suffix_here(t);
    aut->properties().update(prefix_ftag{});
    return aut;
  }

  template <Automaton Aut>
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
      template <Automaton Aut>
      automaton
      prefix(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::prefix(a);
      }
    }
  }

  /*---------.
  | factor.  |
  `---------*/

  /// Make each useful state both initial and final.
  template <Automaton Aut>
  Aut&
  factor_here(Aut& aut)
  {
    auto& res = suffix_here(prefix_here(aut));
    res->properties().update(factor_ftag{});
    return res;
  }

  template <Automaton Aut>
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
      /// Bridge.
      template <Automaton Aut>
      automaton
      factor(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::factor(a);
      }
    }
  }

  /*----------.
  | subword.  |
  `----------*/

  /// Add spontaneous transitions for each non spontaneous transition,
  /// with same source, destination and weight.
  template <Automaton Aut>
  Aut&
  subword_here(Aut& aut)
  {
    const auto one = aut->labelset()->one();

    auto ts = std::vector<transition_t_of<Aut>>{};
    for (auto s : aut->states())
      {
        ts.clear();
        for (auto t : out(aut, s))
          if (!aut->labelset()->is_one(aut->label_of(t)))
            ts.emplace_back(t);
        for (auto t : ts)
          aut->add_transition(s, aut->dst_of(t), one, aut->weight_of(t));
      }

    aut->properties().update(subword_ftag{});
    return aut;
  }

  /// Apply subword_here() to a copy of \a aut.
  template <Automaton Aut>
  auto
  subword(const Aut& aut)
    -> decltype(make_nullable_automaton(aut->context()))
  {
    auto res = make_nullable_automaton(aut->context());
    copy_into(aut, res);
    subword_here(res);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      automaton
      subword(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::subword(a);
      }
    }
  }
} // namespace vcsn
