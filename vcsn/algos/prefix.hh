#pragma once

#include <vcsn/algos/accessible.hh>
#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/dyn/fwd.hh>
#include <vcsn/labelset/labelset.hh>

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
      if (s != aut->pre()
          && s != aut->post()
          && !aut->is_initial(s))
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
          if (!aut->is_initial(s))
            aut->set_initial(s);
          if (!aut->is_final(s))
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
      /// Bridge.
      template <typename Aut>
      automaton
      factor(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::factor(a));
      }
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

    auto ts = std::vector<transition_t_of<Aut>>{};
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
    -> fresh_automaton_t_of<Aut,
                            detail::nullableset_context_t<context_t_of<Aut>>>
  {
    using res_t
      = fresh_automaton_t_of<Aut,
                             detail::nullableset_context_t<context_t_of<Aut>>>;
    auto res = make_shared_ptr<res_t>(make_nullableset_context(aut->context()));
    copy_into(aut, res);
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
    }
  }
} // namespace vcsn
